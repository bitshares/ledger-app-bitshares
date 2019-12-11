from bitshares import BitShares
from bitsharesbase import operations
from bitsharesbase.signedtransactions import Signed_Transaction
from bitshares.account import Account
from bitshares.amount import Amount
from bitshares.asset import Asset
from bitshares.memo import Memo
from graphenecommon.exceptions import AccountDoesNotExistsException
from graphenecommon.exceptions import AssetDoesNotExistsException
from grapheneapi.exceptions import RPCError
from grapheneapi.exceptions import NumRetriesReached
from ledgerblue.comm import getDongle
from ledgerblue.commException import CommException
from asn1 import Encoder, Numbers
from datetime import datetime, timedelta
import binascii
import struct
import json
from logger import Logger


def initBlockchainObject(api_node):
    global blockchain
    try:
        blockchain = BitShares(api_node, num_retries=0)
        return blockchain
    except:
        print("ERROR: Could not connect to API node at %s" % api_node)
        exit()


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


def getHeadBlockBlankTx():
    tx_head = blockchain.new_tx()    # Pull recent TaPoS
    try:
        dummy = tx_head['ref_block_num'] # Somehow this triggers tx_head to populate 'expiration'... (??)
    except NumRetriesReached:
        Logger.Write("ERROR: Can't reach API node: 'NumRetries' reached.  Check network connection.")
        raise
    expiration = datetime.strptime(tx_head['expiration'], "%Y-%m-%dT%H:%M:%S") + timedelta(minutes=10)
    tx_head['expiration'] = expiration.strftime("%Y-%m-%dT%H:%M:%S%Z") # Longer expiration to accomodate device interaction
    return tx_head



##
#  `builder` is a TransactionBuilder object. (E.g. from BitShares.new_tx())
#  `dest_account_name` is a string account name.
def appendTransferOpToTx(builder, from_name, to_name, amount, symbol):

    ## TODO: Cleanup exception catching for better user feedback

    try:
        account = Account(from_name, blockchain_instance=blockchain)
        to = Account(to_name, blockchain_instance=blockchain)
        amountAsset = Amount(amount, symbol, blockchain_instance=blockchain)
    except NumRetriesReached:
        Logger.Write("ERROR: Can't reach API node: 'NumRetries' reached.  Check network connection.")
        raise
    except AssetDoesNotExistsException as e:
        Logger.Write("ERROR: Asset or token '%s' not known."%str(e))
        raise
    except AccountDoesNotExistsException as e:
        Logger.Write("ERROR: Account '%s' not known."%str(e))
        raise
    except Exception as e:
        Logger.Write("Unknown problem constructing Transfer operation: %s"%str(e))
        raise

    memoObj = Memo(from_account=account, to_account=to, blockchain_instance=blockchain)
    memo_text = "" #"Signed by BitShares App on Ledger Nano S!"

    op = operations.Transfer(
        **{
            "fee": {"amount": 0, "asset_id": "1.3.0"},
            "from": account["id"],
            "to": to["id"],
            "amount": {"amount": int(amountAsset), "asset_id": amountAsset.asset["id"]},
            "memo": memoObj.encrypt(memo_text),
        }
    )

    builder.appendOps(op)
    return builder

##
# Generate a TaPoS current transaction with a single Transfer operation
# in it, return as JSON string.
#   _names's symbol are strings, amount is float
def generateTransferTxJSON(from_name, to_name, amount, symbol):
    builder = getHeadBlockBlankTx()
    builder = appendTransferOpToTx(builder, from_name, to_name, amount, symbol)
    builder.constructTx()
    return json.dumps(builder)

##
#
def getSerializedTxBytes(tx_json):
    tx = json.loads(tx_json)  # from json to dict
    st = Signed_Transaction(
            ref_block_num = tx['ref_block_num'],
            ref_block_prefix = tx['ref_block_prefix'],
            expiration = tx['expiration'],
            operations = tx['operations'],
    )
    serialized = encodeTlvTx(binascii.unhexlify(blockchain.rpc.chain_params['chain_id']), st)
    return serialized

##
#
def encodeTlvTx(chain_id, tx):

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

##
#
def getSignatureFromNano(serial_tx_bytes, bip32_path):
    donglePath = parse_bip32_path(bip32_path)
    pathSize = int(len(donglePath) / 4)
    try:
        dongle = getDongle(True)
    except:
        Logger.Write("Ledger Nano not found! Is it plugged in and unlocked?")
        raise
    Logger.Write("Please review and confirm transaction on Ledger Nano S...")
    offset = 0
    first = True
    signSize = len(serial_tx_bytes)
    while offset != signSize:
        if signSize - offset > 200:
            chunk = serial_tx_bytes[offset: offset + 200]
        else:
            chunk = serial_tx_bytes[offset:]

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
        except CommException as e:
            dongle.close()
            if e.sw == 0x6e00:
                Logger.Write("BitShares App not running on Nano.  Please check.")
            else:
                Logger.Write("User declined - transaction not signed.")
            raise
        except:
            dongle.close()
            Logger.Write("An unknown error occured.  Was device unplugged?")
            raise
    dongle.close()
    return result

def broadcastTxWithProvidedSignature(tx_json, sig_bytes):

    signed_tx = blockchain.new_tx(json.loads(str(tx_json)))
    signed_tx["signatures"].extend([binascii.hexlify(sig_bytes).decode()])

    Logger.Write("Broadcasting transaction...", echo=True)
    try:
        print (blockchain.broadcast(tx=signed_tx))
        Logger.Write("Success!  Transaction has been sent.", echo=True)
    except RPCError as e:
        Logger.Write("Could not broadcast transaction!", echo=True)
        Logger.Write(str(e))
        raise
    except NumRetriesReached:
        Logger.Write("ERROR: Could not broadcast transaction: 'NumRetries' reached.  Check network connection.")
        raise

def getPublicKeyListFromNano(bip32_paths, confirm_on_device = False):
    # Captures all exceptions and does not re-raise. Will return an empty or partial
    # list if we don't suceed in retrieving all keys. To determine success or
    # (partial) failure, compare length of return list to length of key list.
    Addresses = []
    try:
        dongle = getDongle(True)
    except:
        Logger.Write("Ledger Nano not found! Is it plugged in and unlocked?")
        return []

    for path in bip32_paths:
        donglePath = parse_bip32_path(path)
        apdu = binascii.unhexlify("B502"
                                  + ("01" if confirm_on_device else "00")
                                  + "00"
                                  + "{:02x}".format(len(donglePath) + 1)
                                  + "{:02x}".format(int(len(donglePath) / 4))
        ) + donglePath

        try:
            result = dongle.exchange(apdu)
        except CommException as e:
            dongle.close()
            if e.sw == 0x6e00:
                Logger.Write("BitShares App not running on Nano.  Please check.")
            elif e.sw == 0x6985:
                Logger.Write("Warning! Address not confirmed by user - may not be valid!")
                raise
            else:
                Logger.Write("Warning! Address not confirmed by user, or other error.")
            return Addresses
        except Exception:
            dongle.close()
            Logger.Write("An unknown error occured.  Was device unplugged?")
            return Addresses

        offset = 1 + result[0]
        address = bytes(result[offset + 1: offset + 1 + result[offset]]).decode("utf-8")

        ## TODO: Also extract pubkey and assert that it produces same address

        Addresses.append(address)

    dongle.close()
    return Addresses
