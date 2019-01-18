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

from asn1 import Encoder, Numbers
from datetime import datetime
import struct
import binascii
from base58 import b58decode
import hashlib


class Transaction:
    def __init__(self):
        pass

    @staticmethod
    def id_to_number(num):
        dot_pos = num.rfind('.')

        if dot_pos != -1:
            return int(num[dot_pos+1:])

        return int(num)

    @staticmethod
    def parse_memo(memo):
        parameters = struct.pack('B', 1)
        parameters += Transaction.parse_public_key(memo['from'])
        parameters += Transaction.parse_public_key(memo['to'])
        parameters += struct.pack('<Q', int(memo['nonce']))
        message = binascii.unhexlify(memo['message'])
        length = len(message)
        parameters += Transaction.pack_fc_uint(length)
        parameters += struct.pack(str(length) + 's', message)
        return parameters

    @staticmethod
    def parse_transfer(data):
        fee = data['fee']
        parameters = struct.pack('<Q', fee['amount'])
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(fee['asset_id']))
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(data['from']))
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(data['to']))
        amount = data['amount']
        parameters += struct.pack('<Q', amount['amount'])
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(amount['asset_id']))

        if 'memo' in data:
            parameters += Transaction.parse_memo(data['memo'])
        else:
            parameters += struct.pack('B', 0)

        # TODO: extensions
        parameters += struct.pack('B', 0)

        return parameters

    @staticmethod
    def pack_fc_uint(value):
        out = ''
        i = 0
        val = value
        while True:
            b = val & 0x7f
            val >>= 7
            b |= ((val > 0) << 7)
            i += 1
            out += chr(b)

            if val == 0:
                break

        return out

    @staticmethod
    def unpack_fc_uint(buffer):
        i = 0
        v = 0
        b = 0
        by = 0

        k = 0
        while True:
            b = ord(buffer[k])
            k += 1
            i += 1
            v |= (b & 0x7f) << by
            by += 7

            if (b & 0x80) == 0 or by >= 32:
                break

        return v

    @staticmethod
    def parse_public_key(data):
        data = str(data[3:])
        decoded = b58decode(data)
        decoded = decoded[:-4]
        return decoded

    @staticmethod
    def parse_unknown(data):
        data = data * 1000
        parameters = struct.pack(str(len(data)) + 's', str(data))
        return parameters

    @staticmethod
    def parse(json):
        tx = Transaction()
        tx.json = json

        tx.ref_block_num = struct.pack('<H', json['ref_block_num'])
        tx.ref_block_prefix = struct.pack('<I', json['ref_block_prefix'])

        expiration = int(datetime.strptime(json['expiration'], '%Y-%m-%dT%H:%M:%S').strftime("%s"))
        tx.expiration = struct.pack('<I', expiration)

        operations = json['operations']
        tx.n_op = Transaction.pack_fc_uint(len(operations))
        tx.operations = []
        for op in operations:
            tx.operations.append(Transaction.pack_fc_uint(op[0]))
            # TODO: other parsers.
            if op[0] == 0:
                parameters = Transaction.parse_transfer(op[1])
            else:
                parameters = Transaction.parse_unknown(op[1])
            tx.operations.append(parameters)

        # TODO: extensions
        tx.n_ext = struct.pack('B', 0)

        return tx

    def encode(self, chain_id):
        encoder = Encoder()

        encoder.start()

        length = len(chain_id)
        parameters = struct.pack(str(length) + 's', chain_id)
        encoder.write(parameters, Numbers.OctetString)

        encoder.write(self.ref_block_num, Numbers.OctetString)
        encoder.write(self.ref_block_prefix, Numbers.OctetString)
        encoder.write(self.expiration, Numbers.OctetString)
        encoder.write(self.n_op, Numbers.OctetString)
        for op in self.operations:
            encoder.write(op, Numbers.OctetString)
        encoder.write(self.n_ext, Numbers.OctetString)

        return encoder.output()
