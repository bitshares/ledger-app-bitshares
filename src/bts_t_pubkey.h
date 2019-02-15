/*******************************************************************************
*  Copyright of the Contributing Authors, including:
*
*   (c) 2019 Christopher J. Sanborn
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

#ifndef __BTS_T_PUBKEY_H__
#define __BTS_T_PUBKEY_H__

#include "os.h"

/**
 * Deserialization and pretty-printing of on-the-wire public keys.  Pubkeys
 * are serialized in "compressed" form.
 *
 * For bts_public_key_type_t, note that internal element names of struct is
 * arbitrary and may change. Important thing is that it gives the correct sizeof()
 * and that it is mem-layout compatible with char[33], and that all 33 bytes will
 * pass if passed by value.
 */
typedef struct bts_public_key_type_t {
  uint8_t h[1];
  uint8_t x[32];
} bts_public_key_type_t;

uint32_t deserializeBtsPublicKeyType(const uint8_t *buffer, uint32_t bufferLength, bts_public_key_type_t * keydata);

/**
 * Pretty-prints the key in base58check including BTS prefix.
 */
uint32_t prettyPrintBtsPublicKeyType(bts_public_key_type_t pubkey, char * buffer);

#endif
