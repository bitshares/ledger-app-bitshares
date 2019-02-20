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

#ifndef __BTS_T_ACCOUNT_OPTIONS_H__
#define __BTS_T_ACCOUNT_OPTIONS_H__

#include "bts_t_pubkey.h"
#include "bts_t_account.h"
#include "bts_t_varint.h"
#include "bts_t_extensions.h"
#include <stdbool.h>
#include "os.h"

typedef struct bts_account_options_type_t {
    bts_public_key_type_t memoPubkey;
    bts_account_id_type_t votingAccount;
    uint16_t              numWitnesses;
    uint16_t              numCommittee;
    bts_varint32_type_t   numVotes;
    const void *          votes; // Point to first vote in OpData buffer
    bts_extension_array_type_t extensions;
} bts_account_options_type_t;

typedef uint32_t bts_vote_type_t;

uint32_t deserializeBtsAccountOptionsType(const uint8_t *buffer, uint32_t bufferLength, bts_account_options_type_t * opts);
uint32_t deserializeBtsVoteType(const uint8_t *buffer, uint32_t bufferLength, bts_vote_type_t * vote);

uint32_t prettyPrintBtsVoteType(bts_vote_type_t vote, char * buffer, uint32_t bufferLength);
uint32_t prettyPrintBtsVotesList(bts_account_options_type_t opts, char * buffer, uint32_t bufferLength);

#endif
