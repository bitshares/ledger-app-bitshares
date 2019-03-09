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
"""
from ledgerblue.comm import getDongle
import struct
from base58 import b58encode
import hashlib
import binascii


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

role_names = ["owner", "active", "", "memo"]

dongle = getDongle(False)
path = "48'/1'/"
for account in range(0, 2):
    for key in range(0, 3):
        print ("---------------------------- account: {}' key: {}' ----------------------------".format(account, key))
        for role in (0, 1, 3):
            derPath = path + str(role) + "'/" + str(account) + "'/" + str(key) + "'"
            donglePath = parse_bip32_path(derPath)
            apdu = binascii.unhexlify("B5020001" + "{:02x}".format(len(donglePath) + 1) + "{:02x}".format(int(len(donglePath) / 4))) + donglePath

            result = dongle.exchange(apdu)
            offset = 1 + result[0]
            address = bytes(result[offset + 1: offset + 1 + result[offset]])

            public_key = result[1: 1 + result[0]]
            head = 0x03 if (public_key[64] & 0x01) == 1 else 0x02
            public_key_compressed = bytearray([head]) + public_key[1:33]

            ripemd = hashlib.new('ripemd160')
            ripemd.update(public_key_compressed)
            check = ripemd.digest()[:4]

            buff = public_key_compressed + check
            wif_public_key = "BTS" + b58encode(bytes(buff)).decode()
            print ("{}:\t{} {}".format(role_names[role], wif_public_key, derPath))
            assert wif_public_key == address.decode()
