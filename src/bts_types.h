/*******************************************************************************
*
*  This file is a derivative work, and contains modifications from original
*  form.  The modifications are copyright of their respective contributors,
*  and are licensed under the same terms as the original work.
*
*  Portions Copyright (c) 2019 Christopher J. Sanborn
*
*  Original copyright and license notice follows:
*
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

#ifndef __BTS_TYPES_H__
#define __BTS_TYPES_H__

#include <stdint.h>

typedef uint8_t checksum256[32];
typedef uint8_t public_key_t[33];

/**
 * Unpacks a variable-length encoded unsigned integer from a byte buffer into a
 * uint32.  Decoded value is written at `value`.  Returns number of bytes read from
 * input buffer.  Throws EXCEPTION_OVERFLOW if decoding exceeds 32 bits.
 */
uint32_t unpack_varint32(const uint8_t *in, uint32_t *value);

/**
 * Unpacks a variable-length unsigned integer up to 48-bits into a uint64. Similar
 * to 32-bit version, but this one can be used for BitShares instance_id's which can
 * be up to 48 bits. Throws EXCEPTION_OVERFLOW if decoding exceeds 48 bits.
 */
uint32_t unpack_varint48(const uint8_t *in, uint64_t *value);

uint32_t public_key_to_wif(uint8_t *publicKey, uint32_t keyLength, char *out, uint32_t outLength);
uint32_t compressed_public_key_to_wif(uint8_t *publicKey, uint32_t keyLength, char *out, uint32_t outLength);

#endif // __BTS_TYPES_H__
