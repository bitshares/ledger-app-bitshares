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

#include "eos_types.h"
#include "eos_utils.h"
#include "os.h"
#include <stdbool.h>
#include "string.h"

uint32_t unpack_varint32(const uint8_t *in, uint32_t length, uint32_t *value) {
    uint32_t i = 0;
    uint64_t v = 0; char b = 0; uint8_t by = 0;
    do {
        b = *in; ++in; ++i;
        v |= (uint64_t)((uint8_t)b & 0x7f) << by;
        by += 7;
    } while( ((uint8_t)b) & 0x80 && by < 35 );

    if( ((uint8_t)b) & 0x80 ) {         // Didn't hit terminating byte
      THROW(EXCEPTION_OVERFLOW);
    }
    if ( v>>32 != 0 ) {                 // Too big
      THROW(EXCEPTION_OVERFLOW);
    }

    *value = v;
    return i;
}

uint32_t unpack_varint48(const uint8_t *in, uint64_t *value) {
    uint32_t i = 0;
    uint64_t v = 0; char b = 0; uint8_t by = 0;
    do {
        b = *in; ++in; ++i;
        v |= (uint64_t)((uint8_t)b & 0x7f) << by;
        by += 7;
    } while( ((uint8_t)b) & 0x80 && by < 49 );

    if( ((uint8_t)b) & 0x80 ) {         // Didn't hit terminating byte
      THROW(EXCEPTION_OVERFLOW);
    }
    if ( v>>48 != 0 ) {                 // Too big
      THROW(EXCEPTION_OVERFLOW);
    }

    *value = v;
    return i;
}

uint32_t public_key_to_wif(uint8_t *publicKey, uint32_t keyLength, char *out, uint32_t outLength) {
    if (publicKey == NULL || keyLength < 33) {
        THROW(INVALID_PARAMETER);
    }
    if (outLength < 40) {
        THROW(EXCEPTION_OVERFLOW);
    }

    uint8_t temp[33];
    // is even?
    temp[0] = (publicKey[64] & 0x1) ? 0x03 : 0x02;
    os_memmove(temp + 1, publicKey + 1, 32);
    return compressed_public_key_to_wif(temp, sizeof(temp), out, outLength);
}

uint32_t compressed_public_key_to_wif(uint8_t *publicKey, uint32_t keyLength, char *out, uint32_t outLength) {
    if (keyLength < 33) {
        THROW(INVALID_PARAMETER);
    }
    if (outLength < 40) {
        THROW(EXCEPTION_OVERFLOW);
    }
    
    uint8_t temp[37];
    os_memset(temp, 0, sizeof(temp));
    os_memmove(temp, publicKey, 33);
    
    uint8_t check[20];
    cx_ripemd160_t riprip;
    cx_ripemd160_init(&riprip);
    cx_hash(&riprip.header, CX_LAST, temp, 33, check);
    os_memmove(temp + 33, check, 4);
    
    os_memset(out, 0, outLength);
    out[0] = 'E';
    out[1] = 'O';
    out[2] = 'S';
    uint32_t addressLen = outLength - 3;
    b58enc(temp, sizeof(temp), out + 3, &addressLen);
    if (addressLen + 3 >= outLength) {
        THROW(EXCEPTION_OVERFLOW);
    }
    return addressLen + 3;
}
