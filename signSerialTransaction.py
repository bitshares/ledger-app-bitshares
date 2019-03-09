#!/usr/bin/env python3
"""
/*******************************************************************************
*  Copyright of the Contributing Authors; see CONTRIBUTORS.md.
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

  Reads an already serialized transaction from a file and sends it to
  the dongle to be signed.  The transaction is expected to be in an
  ascii file containing the hex representation of the serialized
  transaction.

"""

import binascii
import string
import struct
from ledgerblue.comm import getDongle
import argparse
import sys
import os

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

parser = argparse.ArgumentParser()
parser.add_argument('--path', help="BIP 32 path to retrieve")
parser.add_argument('--file', help="Serialized transaction in ascii hex format")
args = parser.parse_args()

if args.path is None:
    args.path = "48'/1'/1'/0'/0'"

if args.file is None:
    args.file = 'example-tx/tx_transfer.derhex'

if not os.path.isfile(args.file):
    parser.print_help()
    sys.exit()

donglePath = parse_bip32_path(args.path)
pathSize = int(len(donglePath) / 4)

with open(args.file, 'r') as f:
    tx_ser_hex = "".join(f.read().split())
    tx_raw = binascii.unhexlify(tx_ser_hex)
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
