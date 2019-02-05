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
    def parse_limit_order_create(data):
        fee = data['fee']
        parameters = struct.pack('<Q', fee['amount'])
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(fee['asset_id']))
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(data['seller']))
        amount = data['amount_to_sell']
        parameters += struct.pack('<Q', amount['amount'])
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(amount['asset_id']))
        minreceive = data['min_to_receive']
        parameters += struct.pack('<Q', minreceive['amount'])
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(minreceive['asset_id']))

        expiration = int(datetime.strptime(data['expiration'], '%Y-%m-%dT%H:%M:%S').strftime("%s"))
        parameters += struct.pack('<I', expiration)

        if bool(data['fill_or_kill']):
            parameters += struct.pack('B', 1)
        else:
            parameters += struct.pack('B', 0)

        # TODO: extensions
        parameters += struct.pack('B', 0)

        return parameters

    @staticmethod
    def parse_limit_order_cancel(data):
        fee = data['fee']
        parameters = struct.pack('<Q', fee['amount'])
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(fee['asset_id']))
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(data['fee_paying_account']))
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(data['order']))

        # TODO: extensions
        parameters += struct.pack('B', 0)

        return parameters

    @staticmethod
    def parse_owner_or_active(data):
        parameters = struct.pack('<I', data['weight_threshold'])

        account_auths = data['account_auths']
        parameters += struct.pack('B', len(account_auths))
        for account in account_auths:
            parameters += Transaction.pack_fc_uint(Transaction.id_to_number(account[0]))
            parameters += struct.pack('<H', account[1])

        key_auths = data['key_auths']
        parameters += struct.pack('B', len(key_auths))
        for key in key_auths:
            parameters += Transaction.parse_public_key(key[0])
            parameters += struct.pack('<H', key[1])

        # TODO: address_auths
        address_auths = data['address_auths']
        parameters += struct.pack('B', len(address_auths))

        return parameters

    @staticmethod
    def parse_votes(votes):
        parameters = struct.pack('B', len(votes))
        for vote in votes:
            v = vote.split(':')
            vote_type = int(v[0])
            instance  = int(v[1])
            parameters += struct.pack('<I', ((instance & 0xFFFFFF) << 8) | (vote_type & 0xFF))
        return parameters

    @staticmethod
    def parse_account_update(data):
        fee = data['fee']
        parameters = struct.pack('<Q', fee['amount'])
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(fee['asset_id']))
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(data['account']))

        if 'owner' in data:
            parameters += struct.pack('B', 1)
            parameters += Transaction.parse_owner_or_active(data['owner'])
        else:
            parameters += struct.pack('B', 0)

        if 'active' in data:
            parameters += struct.pack('B', 1)
            parameters += Transaction.parse_owner_or_active(data['active'])
        else:
            parameters += struct.pack('B', 0)

        if 'new_options' in data:
            new_options = data['new_options']
            parameters += struct.pack('B', 1)
            parameters += Transaction.parse_public_key(new_options['memo_key'])
            parameters += Transaction.pack_fc_uint(Transaction.id_to_number(new_options['voting_account']))
            parameters += struct.pack('<H', new_options['num_witness'])
            parameters += struct.pack('<H', new_options['num_committee'])
            parameters += Transaction.parse_votes(new_options['votes'])

            # TODO: extensions
            parameters += struct.pack('B', 0)
        else:
            parameters += struct.pack('B', 0)

        # TODO: extensions
        parameters += struct.pack('B', 0)

        return parameters

    @staticmethod
    def parse_account_upgrade(data):
        fee = data['fee']
        parameters = struct.pack('<Q', fee['amount'])
        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(fee['asset_id']))

        parameters += Transaction.pack_fc_uint(Transaction.id_to_number(data['account_to_upgrade']))

        if bool(data['upgrade_to_lifetime_member']):
            parameters += struct.pack('B', 1)
        else:
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
            elif op[0] == 1:
                parameters = Transaction.parse_limit_order_create(op[1])
            elif op[0] == 2:
                parameters = Transaction.parse_limit_order_cancel(op[1])
            elif op[0] == 6:
                parameters = Transaction.parse_account_update(op[1])
            elif op[0] == 8:
                parameters = Transaction.parse_account_upgrade(op[1])
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
