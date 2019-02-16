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

#ifndef __BTS_T_MEMO_H__
#define __BTS_T_MEMO_H__

#include "bts_t_pubkey.h"
#include "bts_t_varint.h"
#include <stdbool.h>
#include "os.h"

typedef struct bts_memo_type_t {
    bts_public_key_type_t fromPubkey;
    bts_public_key_type_t toPubkey;
    uint64_t              nonce;
    bts_varint32_type_t   cipherTextLength;
    const uint8_t *       cipherText; // message start in OpData buffer
} bts_memo_type_t;

uint32_t deserializeBtsMemoType(const uint8_t *buffer, uint32_t bufferLength, bts_memo_type_t * memo);

#endif
