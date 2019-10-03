# Super Simple BitShare GUI Python Wallet - Not going for sophisticated here.
#
#
# Run with:  (Old stuff from former life as a demo app follow....)
#
#   python3 tip-app-01.py --sender=[sender_account_name]
#
#   Options:
#
#     --sender=[name]      (Default: "ledger-demo")
#     --node=[api_node]    (Default: "wss://bitshares.openledger.info/ws")
#     --path=[bip32_path]  (Default: "48'/1'/1'/0'/0'")
#     --symbol=[asset_sym] (Default: "BTS".  OK to include "bit" in bitASSETS if desired for display)
#     --amount=[float]     (Default: 2.0) Amount of tip.
#
# Dependencies:
#
#   The python dependencies are identical to those of signTransaction.py and
#   getPublicKey.py except for the addition of the `tkinter` lib for the GUI.
#
#   The latter may need to be installed with:
#
#   $ sudo apt-get install python3-tk
#
#   Or something appropriate for your system.
#
#   Also: $ pip3 install --user pyttk
#

##
## Imports:
##
import json
import binascii
import struct
import argparse
from tkinter import *
import ttk
from bitshares import BitShares
from bitsharesbase import operations
from bitsharesbase.signedtransactions import Signed_Transaction
from bitshares.account import Account
from bitshares.amount import Amount
from bitshares.asset import Asset
from bitshares.memo import Memo
from asn1 import Encoder, Numbers
from graphenecommon.exceptions import AccountDoesNotExistsException
from graphenecommon.exceptions import AssetDoesNotExistsException
from grapheneapi.exceptions import RPCError
from grapheneapi.exceptions import NumRetriesReached
from wallet_forms import *
from wallet_actions import *
import Logger

##
## Args and defaults:
##
parser = argparse.ArgumentParser()
parser.add_argument('--node', help="BitShares API node to be use.")
parser.add_argument('--sender', help="BitShares account name from which to send tips.")
parser.add_argument('--path', help="BIP 32 path to use for signing.")
parser.add_argument('--symbol', help="Asset to use for tips. Default 'BTS'.")
parser.add_argument('--amount', type=float, help="Amount of tip. Default 2.0.")
args = parser.parse_args()

if args.node is None:
    args.node = "wss://bitshares.openledger.info/ws"

if args.path is None:
    args.path = "48'/1'/1'/0'/0'"

if args.sender is None:
    args.sender = "ledger-demo"

if args.symbol is None:
    args.symbol = "BTS"

if args.amount is None:
    args.amount = 2.0

bip32_path = args.path
default_sender = args.sender
tip_amount = args.amount
tip_asset_display_symbol = args.symbol
tip_asset_symbol = tip_asset_display_symbol.replace("bit","")

if tip_amount > 10.0:
    tip_amount = 10.0
    print("Sanity Guard: Tip amount capped at %f. Modify source to override."%tip_amount)


##
## UX Stuff:
##
def log_print_startup_message():
    #Logger.Write("**** COMMODORE 64 BASIC V2  64K RAM SYSTEM  38911 BASIC BYTES FREE ****", echo=False)
    Logger.Clear()
    Logger.Write("READY.", echo=False)


##
## Main()
##
## Setup and start the GUI:
##
if __name__ == "__main__":

    bkgnd = "light blue"

    # Create a GUI window, then three top-to-bottom subregions as frames:
    gui = Tk()
    gui.configure(background=bkgnd)
    gui.title("Super-Simple BitShares Wallet for Ledger Nano")
    gui.geometry("800x600")
    gui.minsize(640,480)
    gui_style = ttk.Style()
    gui_style.theme_use('clam')
    #
    # Window Regions:
    #  +---------------------+
    #  |     frame_top       |
    #  +---------+-----------+
    #  | frame_  | frame_    |
    #  |    left |  center   |
    #  +---------+-----------+
    #  |    frame_bottom     |
    #  +---------------------+
    #
    frame_top = ttk.Frame(gui)
    frame_top.pack(fill="both")

    paned_middle_bottom = ttk.PanedWindow(gui, orient=VERTICAL)
    paned_middle_bottom.pack(fill=BOTH, expand=1)
    paned_left_center = ttk.PanedWindow(paned_middle_bottom, orient=HORIZONTAL)
    paned_left_center.pack(expand=False, fill="both")
    paned_middle_bottom.add(paned_left_center)
    frame_left = ttk.Frame(paned_left_center)
    paned_left_center.add(frame_left)
    frame_center = ttk.Frame(paned_left_center)
    paned_left_center.add(frame_center)
    frame_bottom = ttk.Frame(paned_middle_bottom)
    paned_middle_bottom.add(frame_bottom)

    # Form Variables:
    var_from_account_name = StringVar(gui, value = default_sender)
    var_bip32_path = StringVar(gui, value = bip32_path)
    var_selected_asset = StringVar(gui)
    var_tx_json = StringVar(gui)
    var_tx_serial = StringVar(gui)      # Hex representation of serial bytes
    var_tx_signature = StringVar(gui)   # Hex representation of signature

    def serializeTxJSON():  # var_tx_json ->(turn crank)-> var_tx_serial:
        try:
            signData = getSerializedTxBytes(var_tx_json.get())
            var_tx_serial.set(binascii.hexlify(signData).decode())
        except json.decoder.JSONDecodeError as e:
            var_tx_serial.set("<<TX COULD NOT BE SERIALIZED>>")
            Logger.Write("JSON Decode Error: " + str(e))
            raise
        except:
            var_tx_serial.set("<<TX COULD NOT BE SERIALIZED>>")
            raise
        finally:
            pass

    def signTxHexBytes():   # var_tx_serial ->(turn crank)-> var_tx_signature:
        try:
            txHex = "".join(var_tx_serial.get().split())
            signData = binascii.unhexlify(txHex)
            sig_bytes = getSignatureFromNano(signData, var_bip32_path.get())
            var_tx_signature.set(binascii.hexlify(sig_bytes).decode())
        except:
            var_tx_signature.set("<<COULD NOT GET SIGNATURE>>")
            raise

    def broadcastSignedTx():  # Combine var_tx_json & var_tx_signature, broadcast
        sigHex = var_tx_signature.get().strip()
        sig_bytes = binascii.unhexlify(sigHex)
        broadcastTxWithProvidedSignature(var_tx_json.get(), sig_bytes)
        account_info_refresh()

    def sendTransfer(from_name, to_name, amount, symbol):
        try:
            var_tx_json.set("")
            var_tx_serial.set("")
            var_tx_signature.set("")
            Logger.Write("Preparing to send %f %s from \"%s\" to \"%s\"..." % (amount, symbol, from_name, to_name))
            tx_json = generateTransferTxJSON(from_name, to_name, amount, symbol)
            var_tx_json.set(tx_json)
            serializeTxJSON()
            signTxHexBytes()
            broadcastSignedTx()
        except Exception as e:
            pass

    ##
    ## Whoami Frame:
    ##
    def account_info_refresh():
        try:
            spending_account = Account(var_from_account_name.get(), blockchain_instance=blockchain)
            balances = spending_account.balances
            history = spending_account.history(limit=16)
        except AccountDoesNotExistsException:
            Logger.Write("ERROR: Specified account does not exist on BitShares network.")
            balances = []
            history = []
        frameAssets.setBalances(balances)
        frameHistory.setHistory(history)

    frameWhoAmI = WhoAmIFrame(frame_top, textvariable=var_from_account_name,
                              textvariablebip32=var_bip32_path,
                              command=account_info_refresh)
    frameWhoAmI.pack(padx=10, pady=(16,16), fill="both")

    ##
    ## Asset List and History frames in tabbed_AccountInfo Notebook:
    ##

    tabbed_AccountInfo = ttk.Notebook(frame_left)

    frameAssets = AssetListFrame(tabbed_AccountInfo, assettextvariable=var_selected_asset)
    frameAssets.pack(side="left", expand=False, fill="y")

    frameHistory = HistoryListFrame(tabbed_AccountInfo, jsonvar=var_tx_json)
    frameHistory.pack()

    tabbed_AccountInfo.add(frameAssets, text = 'Assets')
    tabbed_AccountInfo.add(frameHistory, text = 'History')
    tabbed_AccountInfo.pack(padx=(8,1), expand=True, fill="both")

    ##
    ## Active Operation Tabbed Notebook container:
    ##

    tabbed_Active = ttk.Notebook(frame_center)

    ##
    ## Transfer tab:
    ##

    def transferSendPreprocess(to_account, amount, asset_symbol):
        sendTransfer(var_from_account_name.get(), to_account, amount, asset_symbol)
    form_transfer = TransferOpFrame(tabbed_Active, command=transferSendPreprocess, assettextvariable=var_selected_asset)
    form_transfer.pack(expand=True, fill="both")

    ##
    ## Public Keys Tab:
    ##

    form_pubkeys = QueryPublicKeysFrame(tabbed_Active, lookupcommand=getPublicKeyListFromNano)
    form_pubkeys.pack(expand=True, fill="both")

    ##
    ## Raw Transactions Tab:
    ##

    form_raw_tx = RawTransactionsFrame(tabbed_Active,
                            serializecommand=serializeTxJSON,
                            signcommand=signTxHexBytes,
                            broadcastcommand=broadcastSignedTx,
                            jsonvar=var_tx_json, serialvar=var_tx_serial,
                            signaturevar=var_tx_signature)
    form_raw_tx.pack()

    ## Finalize tabbed container:

    tabbed_Active.add(form_transfer, text = 'Transfer')
    tabbed_Active.add(form_pubkeys, text = 'Get Pubkeys')
    tabbed_Active.add(form_raw_tx, text = 'Raw Transactions')

    tabbed_Active.pack(padx=(1,8), expand=True, fill="both")

    ##
    ## Logging window
    ##
    form_activity = ActivityMessageFrame(frame_bottom)
    form_activity.pack(side="bottom", expand=True, fill="both", padx=8, pady=(4,8))
    Logger.SetMessageWidget(form_activity.messages)
    ##

    ##
    ## Startup:
    ##
    Logger.Write("Initializing: Looking for BitShares network...")
    blockchain = initBlockchainObject(args.node)
    Logger.Write("Getting account info for '%s'..."%var_from_account_name.get())
    account_info_refresh()
    log_print_startup_message()
    # start the GUI
    gui.mainloop()

##
## END
##
