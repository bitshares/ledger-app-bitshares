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

#ifndef __BTS_T_PERMISSION_H__
#define __BTS_T_PERMISSION_H__

#include "bts_t_account.h"
#include "bts_t_varint.h"
#include "bts_t_nullset.h"
#include "bts_t_pubkey.h"
#include <stdbool.h>
#include "os.h"

typedef struct bts_account_auth_type_t bts_account_auth_type_t; // Fwd decl
typedef struct bts_key_auth_type_t bts_key_auth_type_t;

/**
 * Permission structure; as in an "owner permission" or an "active permission."
 * Contains weighted lists of accounts or keys that are authorized to act with the
 * particular permission.
 *
 * Contains variable length arrays. Rather than deserialize the elements into this
 * structure, we simply point to them in the OpData buffer, where we can deserialize
 * the individual later elements as needed.  The auth arrays need to be void pointers
 * because the serialized form isn't necessarily constant size or same layout as
 * decoded form.
 */
typedef struct bts_permission_type_t {
    uint32_t              weightThreshold;
    bts_varint32_type_t   numAccountAuths;
    const void *          firstAccountAuth;
    bts_varint32_type_t   numKeyAuths;
    const void *          firstKeyAuth;
    bts_null_set_type_t   numAddressAuths;  // (Deprecated auth type. Should be zero.)
} bts_permission_type_t;

/**
 * Pairs an account Id with a weight.
 */
struct bts_account_auth_type_t {
    bts_account_id_type_t accountId;
    uint16_t weight;
};

/**
 * Pairs a pub key with a weight.
 */
struct bts_key_auth_type_t {
    bts_public_key_type_t pubkey;
    uint16_t weight;
};
#define SIZEOF_BTS_KEY_AUTH_TYPE 35 // Serialized size of

uint32_t deserializeBtsPermissionType(const uint8_t *buffer, uint32_t bufferLength, bts_permission_type_t * asset);

/**
 * Seek Deserialize is different than deserialize in two ways: (1) We deserialize
 * `seek` records and return the `seek`th one in the ovalue, and (2) the retval is the
 * total number of bytes deserialized, not just those of the record returned.  The
 * function(s) work this way because they access arrays of variable-length records, so
 * the only way to find a specific one is to scan through them sequentially.
 */
uint32_t seekDeserializeBtsAccountAuthType(const uint8_t *buffer, uint32_t bufferLength, bts_account_auth_type_t * auth, uint32_t seek);
uint32_t deserializeBtsKeyAuthType(const uint8_t *buffer, uint32_t bufferLength, bts_key_auth_type_t * auth);

/**
 * Pretty-prints a list of Account Auths into a buffer. Will truncate at bufferLength.
 * E.g: (1.2.39247 w:1), (1.2.129476 w:1)
 */
uint32_t prettyPrintBtsAccountAuthsList(bts_permission_type_t asset, char * buffer, uint32_t bufferLength);
uint32_t prettyPrintBtsKeyAuthsList(bts_permission_type_t asset, char * buffer, uint32_t bufferLength);

#endif
