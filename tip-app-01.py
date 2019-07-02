# First attempt at tip app.
#

# Tkinter, apparently a standard lib:
import json
import binascii
import struct
from tkinter import *
from bitshares import BitShares
from bitsharesbase import operations
from bitsharesbase.signedtransactions import Signed_Transaction
from bitshares.account import Account
from bitshares.amount import Amount
from bitshares.asset import Asset
from bitshares.memo import Memo
from asn1 import Encoder, Numbers
from ledgerblue.comm import getDongle
from graphenecommon.exceptions import AccountDoesNotExistsException
from grapheneapi.exceptions import RPCError
from datetime import datetime, timedelta

blockchain = BitShares("wss://bitshares.openledger.info/ws")
bip32_path = "48'/1'/1'/0'/0'"
op_xfr_from = "ledger-demo" #"hw-lns-test-02"

def append_transfer_tx(append_to, dest_account_name):
    #
    account = Account(op_xfr_from, blockchain_instance=blockchain)
    amount = Amount(2.0, "BTS", blockchain_instance=blockchain)
    try:
        to = Account(dest_account_name, blockchain_instance=blockchain)
    except:
        Logger.Write("Problem locating destination account")
        raise
    memoObj = Memo(from_account=account, to_account=to, blockchain_instance=blockchain)
    memo_text = "" #"Signed by BitShares App on Ledger Nano S!"

    op = operations.Transfer(
        **{
            "fee": {"amount": 0, "asset_id": "1.3.0"},
            "from": account["id"],
            "to": to["id"],
            "amount": {"amount": int(amount), "asset_id": amount.asset["id"]},
            "memo": memoObj.encrypt(memo_text),
        }
    )

    append_to.appendOps(op)
    return append_to

def sendTip(to_name):
    #
    Logger.Write("Preparing to send 2.0 BTS to %s..." % (to_name))
    tx_head = blockchain.new_tx()    # Pull recent TaPoS
    dummy = tx_head['ref_block_num'] # Somehow this triggers tx_head to populate 'expiration'... (??)
    expiration = datetime.strptime(tx_head['expiration'], "%Y-%m-%dT%H:%M:%S") + timedelta(minutes=10)
    tx_head['expiration'] = expiration.strftime("%Y-%m-%dT%H:%M:%S%Z") # Longer expiration to accomodate device interaction
    try:
        tx = append_transfer_tx(tx_head, to_name)
    except:
        Logger.Write("Could not construct transaction!")
        return
    print("We have constructed the following transaction:")
    print(tx)
    tx_st = Signed_Transaction(
            ref_block_num=tx['ref_block_num'],
            ref_block_prefix=tx['ref_block_prefix'],
            expiration=tx['expiration'],
            operations=tx['operations'],
    )
    signData = encode(binascii.unhexlify(blockchain.rpc.chain_params['chain_id']), tx_st)
    print("Serialized:")
    print (binascii.hexlify(signData).decode())
    donglePath = parse_bip32_path(bip32_path)
    pathSize = int(len(donglePath) / 4)
    try:
        dongle = getDongle(True)
    except:
        Logger.Write("Ledger Nano not found! Is it plugged in and unlocked?")
        return
    Logger.Write("Prepared! Please confirm transaction on Ledger Nano S...")
    offset = 0
    first = True
    signSize = len(signData)
    while offset != signSize:
        if signSize - offset > 200:
            chunk = signData[offset: offset + 200]
        else:
            chunk = signData[offset:]

        if first:
            totalSize = len(donglePath) + 1 + len(chunk)
            apdu = binascii.unhexlify("B5040000" + "{:02x}".format(totalSize) + "{:02x}".format(pathSize)) + donglePath + chunk
            first = False
        else:
            totalSize = len(chunk)
            apdu = binascii.unhexlify("B5048000" + "{:02x}".format(totalSize)) + chunk

        offset += len(chunk)
        try:
            result = dongle.exchange(apdu)
        except:
            dongle.close()
            Logger.Write("User declined transaction.")
            return
        print (binascii.hexlify(result).decode())
    dongle.close()
    Logger.Write("Broadcasting transaction...")
    tx_sig = blockchain.new_tx(json.loads(str(tx_st)))
    tx_sig["signatures"].extend([binascii.hexlify(result).decode()])
    try:
        print (blockchain.broadcast(tx=tx_sig))
        Logger.Write("Success!  Transaction has been sent.")
    except RPCError as e:
        Logger.Write("Could not broadcast transaction!")
        Logger.Write(str(e))
    except:
        raise

# from signTransaction.py
def encode(chain_id, tx):
    encoder = Encoder()

    encoder.start()

    encoder.write(struct.pack(str(len(chain_id)) + 's', chain_id), Numbers.OctetString)
    encoder.write(bytes(tx['ref_block_num']), Numbers.OctetString)
    encoder.write(bytes(tx['ref_block_prefix']), Numbers.OctetString)
    encoder.write(bytes(tx['expiration']), Numbers.OctetString)
    encoder.write(bytes(tx['operations'].length), Numbers.OctetString)
    for opIdx in range(0, len(tx.toJson()['operations'])):
        encoder.write(bytes([tx['operations'].data[opIdx].opId]), Numbers.OctetString)
        encoder.write(bytes(tx['operations'].data[opIdx].op), Numbers.OctetString)

    if 'extension' in tx:
        encoder.write(bytes(tx['extension']), Numbers.OctetString)
    else:
        encoder.write(bytes([0]), Numbers.OctetString)

    return encoder.output()

def parse_bip32_path(path):
    if len(path) == 0:
        return bytes([])
    result = bytes([])
    elements = path.split('/')
    for pathElement in elements:
        element = pathElement.split('\'')
        if len(element) == 1:
            result = result + struct.pack(">I", int(element[0]))
        else:
            result = result + struct.pack(">I", 0x80000000 | int(element[0]))
    return result

class Logger:
    message_window = None
    message_body = ""
    mirror_to_stdout = True

    @classmethod
    def Write(self, msgtext, *, echo=None):
        if echo == True or (self.mirror_to_stdout==True and echo!=False):
            print(msgtext)
        self.message_body += msgtext + "\n"
        self.message_window.configure(text=self.message_body)
        self.message_window.update()

    @classmethod
    def Clear(self):
        self.message_body = ""
        self.message_window.configure(text=self.message_body)
        self.message_window.update()

def log_print_startup_message():
    Logger.Write("**** COMMODORE 64 BASIC V2  64K RAM SYSTEM  38911 BASIC BYTES FREE ****", echo=False)
    spending_account = Account(op_xfr_from, blockchain_instance=blockchain)
    Logger.Write("Spending tips from acount: \"%s\" (%s)" % (spending_account.name, spending_account.identifier))
    Logger.Write("Available balance: %0.5f BTS" % spending_account.balance("BTS"))
    Logger.Write("READY.", echo=False)


# Main()
if __name__ == "__main__":

    bkgnd = "light blue"

    # create a GUI window
    gui = Tk()
    gui.configure(background=bkgnd)
    gui.title("BitShares Ledger Nano Tip Bot")
    gui.geometry("640x500")

    # Labels and Such
    label01 = Label(gui, text="BitShares: Now Secured by Ledger Nano!",
                    font=("Times", 24, "bold"),
                    background=bkgnd,
                    #relief="groove"
                   )
    label01.pack(pady=40)


    # Destination Frame
    frame01 = Frame(gui, background = bkgnd)
    frame01.pack(pady=10)

    label02 = Label(frame01, text="Send To: (BitShares User Account)",
                    font=("Helvetica", 16),
                    background=bkgnd,
                    #relief="groove"
                   )
    label02.pack(side="left")

    to_account_name = Entry(frame01)
    to_account_name.pack(side="left", padx=10)

    # The Button
    def button_handler_Send(button, box):
        button.configure(state="disabled")
        Logger.Clear()
        try:
            sendTip(to_account_name.get())
        finally:
            button.update() # Eat any clicks that occured while disabled
            button.configure(state="normal") # Return to enabled state
    button_send = Button(gui, text="Send Tip!", command=lambda: button_handler_Send(button_send, to_account_name))
    button_send.pack(pady=40)


    # Bottom text label
    label03 = Label(gui,
                    text="Click ''Send!'' to receive 2.0 BTS tip signed by Ledger Nano S hardware wallet...",
                    font=("Helvetica", 12, "italic"),
                    background=bkgnd,
                   )
    label03.pack()
    spacer = Frame(gui, background=bkgnd)
    spacer.pack(pady=5)


    # Logging window
    log_frame = LabelFrame(gui, text="Activity", background=bkgnd,
                           relief="groove")
    messages = Message(log_frame,
                       text="",
                       width=580, background="light gray",
                       anchor="n", pady=8, font="fixed")
    messages.pack(expand=True, fill="both")
    log_frame.pack(expand=True, fill="both", padx=8, pady=5)
    Logger.message_window = messages
    log_print_startup_message()

    # start the GUI
    gui.mainloop()
