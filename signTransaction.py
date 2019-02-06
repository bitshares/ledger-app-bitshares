#!/usr/bin/env python
"""
/*******************************************************************************
*   Taras Shchybovyk
*   (c) 2018 Taras Shchybovyk
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/
"""

import binascii
import json
import struct
from btsBase import Transaction
from ledgerblue.comm import getDongle
import argparse


def parse_bip32_path(path):
    if len(path) == 0:
        return ""
    result = ""
    elements = path.split('/')
    for pathElement in elements:
        element = pathElement.split('\'')
        if len(element) == 1:
            result = result + struct.pack(">I", int(element[0]))
        else:
            result = result + struct.pack(">I", 0x80000000 | int(element[0]))
    return result


parser = argparse.ArgumentParser()
parser.add_argument('--chain_id', help="Blockchain chain ID")
parser.add_argument('--path', help="BIP 32 path to retrieve")
parser.add_argument('--file', help="Transaction in JSON format")
args = parser.parse_args()

if args.chain_id is None:
    chain_id = binascii.unhexlify("4018d7844c78f6a6c41c6a552b898022310fc5dec06da467ee7905a8dad512c8")
else:
    chain_id = binascii.unhexlify(args.chain_id)

if args.path is None:
    args.path = "48'/1'/1'/0'/0'"

if args.file is None:
    args.file = 'bts_transaction_transfer_usd_with_memo.json'

donglePath = parse_bip32_path(args.path)
pathSize = len(donglePath) / 4

with file(args.file) as f:
    obj = json.load(f)
    tx = Transaction.parse(obj)
    tx_raw = tx.encode(chain_id)
    signData = tx_raw
    print binascii.hexlify(tx_raw)

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
            apdu = "B5040000".decode('hex') + chr(totalSize) + chr(pathSize) + donglePath + chunk
            first = False
        else:
            totalSize = len(chunk)
            apdu = "B5048000".decode('hex') + chr(totalSize) + chunk

        offset += len(chunk)
        result = dongle.exchange(bytes(apdu))
        print binascii.hexlify(result)