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

uint32_t deserializeBtsKeyAuthType(const uint8_t *buffer, uint32_t bufferLength, bts_key_auth_type_t * auth) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = deserializeBtsPublicKeyType(buffer, bufferLength, &auth->pubkey);
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

    PRINTF("DESERIAL: KEY_AUTH: Read %d bytes; %d bytes remain\n", read, bufferLength);

    return read;

}

uint32_t prettyPrintBtsAccountAuth(bts_account_auth_type_t auth, char * buffer, uint32_t bufferLength) {

    uint32_t written = 0;
    char     tmpStr[24];

    snprintf(buffer+written, bufferLength-written, "[1.2.");
    written = strlen(buffer);

    ui64toa(auth.accountId, tmpStr);
    snprintf(buffer+written, bufferLength-written, "%s", tmpStr);
    written = strlen(buffer);
    snprintf(buffer+written, bufferLength-written, ", w: %u]", (uint32_t)auth.weight);
    written = strlen(buffer);

    return written;
}

uint32_t prettyPrintBtsAccountAuthsList(bts_permission_type_t perm, char * buffer, uint32_t bufferLength) {

    uint32_t written = 0;

    if (perm.numAccountAuths == 0) {
        snprintf(buffer, bufferLength, "(None)");
        written = strlen(buffer);
    } else {
        for (uint32_t i = 0; i < perm.numAccountAuths; i++) {
            bts_account_auth_type_t tmpAccountAuth;
            seekDeserializeBtsAccountAuthType(perm.firstAccountAuth,-1,&tmpAccountAuth,i+1);
            prettyPrintBtsAccountAuth(tmpAccountAuth, buffer+written, bufferLength-written);
            written = strlen(buffer);
            if (i+1 != perm.numAccountAuths) {
                snprintf(buffer+written, bufferLength-written, ",  ");
                written = strlen(buffer);
            } else {
                snprintf(buffer+written, bufferLength-written, " ");
                written = strlen(buffer); // terminal spc sidesteps graphical glitch #uglyhack
            }
        }
    }

    return written;
}

uint32_t prettyPrintBtsKeyAuth(bts_key_auth_type_t auth, char * buffer, uint32_t bufferLength) {

    uint32_t written = 0;
    char     tmpStr[56];  // Enough for longest b58check + prefix up to 5 chars

    snprintf(buffer+written, bufferLength-written, "[");
    written = strlen(buffer);

    prettyPrintBtsPublicKeyType(auth.pubkey, tmpStr);
    snprintf(buffer+written, bufferLength-written, "%s", tmpStr);
    written = strlen(buffer);
    snprintf(buffer+written, bufferLength-written, ", w: %u]", (uint32_t)auth.weight);
    written = strlen(buffer);

    return written;
}

uint32_t prettyPrintBtsKeyAuthsList(bts_permission_type_t perm, char * buffer, uint32_t bufferLength) {

    uint32_t written = 0;

    if (perm.numKeyAuths == 0) {
        snprintf(buffer, bufferLength, "(None)");
        written = strlen(buffer);
    } else {
        for (uint32_t i = 0; i < perm.numKeyAuths; i++) {
            bts_key_auth_type_t tmpAuth;
            deserializeBtsKeyAuthType(perm.firstKeyAuth + i * SIZEOF_BTS_KEY_AUTH_TYPE, -1, &tmpAuth);
            prettyPrintBtsKeyAuth(tmpAuth, buffer+written, bufferLength-written);
            written = strlen(buffer);
            if (i+1 != perm.numKeyAuths) {
                snprintf(buffer+written, bufferLength-written, ",  ");
                written = strlen(buffer);
            } else {
                snprintf(buffer+written, bufferLength-written, " ");
                written = strlen(buffer); // terminal spc sidesteps graphical glitch #uglyhack
            }
        }
    }

    return written;
}
