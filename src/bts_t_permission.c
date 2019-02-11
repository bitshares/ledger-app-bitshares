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

#include "bts_t_permission.h"
#include "bts_types.h"
#include "eos_utils.h"
#include "os.h"
#include <string.h>

uint32_t deserializeBtsPermissionType(const uint8_t *buffer, uint32_t bufferLength, bts_permission_type_t * perm) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = sizeof(uint32_t);
    os_memmove(&perm->weightThreshold, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsVarint32Type(buffer, bufferLength, &perm->numAccountAuths);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    perm->firstAccountAuth = buffer;

    bts_account_auth_type_t dummy;
    gobbled = seekDeserializeBtsAccountAuthType(buffer, bufferLength, &dummy, perm->numAccountAuths);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsVarint32Type(buffer, bufferLength, &perm->numKeyAuths);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    perm->firstKeyAuth = buffer;

    gobbled = perm->numKeyAuths * SIZEOF_BTS_KEY_AUTH_TYPE; // Seek past key auth array
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsVarint32Type(buffer, bufferLength, &perm->numAddressAuths);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    if (perm->numAddressAuths != 0) {   // Need to get size right for permission record,
        THROW(EXCEPTION);               // so throw if this count not zero, since I don't
    }                                   // know proper way to decode array if present.
    
    PRINTF("DESERIAL: PERMISSION: Thresh %u nAcc %u; 1st@ %p; nKey %u; 1st@ %p; Read %d bytes; %d bytes remain\n",
           perm->weightThreshold, perm->numAccountAuths, perm->firstAccountAuth, perm->numKeyAuths, perm->firstKeyAuth, read, bufferLength);

    return read;

}

uint32_t seekDeserializeBtsAccountAuthType(const uint8_t *buffer, uint32_t bufferLength, bts_account_auth_type_t * auth, uint32_t seek) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    for ( ; seek > 0; seek--) {
        gobbled = deserializeBtsAccountIdType(buffer, bufferLength, &auth->accountId);
        if (gobbled > bufferLength) {
            THROW(EXCEPTION);
        }
        read += gobbled; buffer += gobbled; bufferLength -= gobbled;

        gobbled = sizeof(uint16_t);
        os_memmove(&auth->weight, buffer, gobbled);
        if (gobbled > bufferLength) {
            THROW(EXCEPTION);
        }
        read += gobbled; buffer += gobbled; bufferLength -= gobbled;
    }

    PRINTF("DESERIAL: ACCT_AUTH: [1.3.%d]w%d; Read %d bytes; %d bytes remain\n",
           (int)auth->accountId, (int)auth->weight, read, bufferLength);

    return read;

}
