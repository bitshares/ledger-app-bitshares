#!/usr/bin/env python3

import binascii
import json
import struct
from asn1 import Encoder, Numbers
from bitsharesbase.signedtransactions import Signed_Transaction
from ledgerblue.comm import getDongle
import argparse

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

def encode(tx):
    encoder = Encoder()

    chain_id = binascii.unhexlify(tx.getKnownChains()['BTS']['chain_id'])

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

parser = argparse.ArgumentParser()
parser.add_argument('--path', help="BIP 32 path to retrieve")
parser.add_argument('--file', help="Transaction in JSON format")
args = parser.parse_args()

if args.path is None:
    args.path = "48'/1'/1'/0'/0'"

if args.file is None:
    args.file = 'bts_transaction_transfer_usd_with_memo.json'

donglePath = parse_bip32_path(args.path)
pathSize = int(len(donglePath) / 4)

with open(args.file) as f:
    obj = json.load(f)
    tx = Signed_Transaction(
            ref_block_num=obj['ref_block_num'],
            ref_block_prefix=obj['ref_block_prefix'],
            expiration=obj['expiration'],
            operations=obj['operations'],
        )
    tx_raw = encode(tx)
    signData = tx_raw
    print (binascii.hexlify(tx_raw).decode())

    dongle = getDongle(True)
    offset = 0
    first = True
    singSize = len(signData)
    while offset != singSize:
        if singSize - offset > 200:
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
        result = dongle.exchange(apdu)
        print (binascii.hexlify(result).decode())
