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

#include "bts_op_account_update.h"
#include "bts_types.h"
#include "os.h"

uint32_t deserializeBtsOperationAccountUpdate(const uint8_t *buffer, uint32_t bufferLength, bts_operation_account_update_t * op) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = deserializeBtsAssetType(buffer, bufferLength, &op->feeAsset);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsAccountIdType(buffer, bufferLength, &op->accountId);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsBoolType(buffer, bufferLength, &op->ownerPermissionPresent);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    if (op->ownerPermissionPresent) {
        gobbled = deserializeBtsPermissionType(buffer, bufferLength, &op->ownerPermission);
        if (gobbled > bufferLength) {
            THROW(EXCEPTION);
        }
        read += gobbled; buffer += gobbled; bufferLength -= gobbled;
    }

    gobbled = deserializeBtsBoolType(buffer, bufferLength, &op->activePermissionPresent);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    if (op->activePermissionPresent) {
        gobbled = deserializeBtsPermissionType(buffer, bufferLength, &op->activePermission);
        if (gobbled > bufferLength) {
            THROW(EXCEPTION);
        }
        read += gobbled; buffer += gobbled; bufferLength -= gobbled;
    }

    gobbled = deserializeBtsBoolType(buffer, bufferLength, &op->accountOptionsPresent);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    // We will have "uninterpretable" data if *either* the included AccountOptions
    // object *or* the operation as a whole has extensions.
    op->containsUninterpretable = false;
    if (op->accountOptionsPresent) {
        gobbled = deserializeBtsAccountOptionsType(buffer, bufferLength, &op->accountOptions);
        if (gobbled > bufferLength) {
            THROW(EXCEPTION);
        }
        read += gobbled; buffer += gobbled; bufferLength -= gobbled;
        if (op->accountOptions.extensions.count > 0) {
            op->containsUninterpretable = true;
        }
    }

    if (!op->containsUninterpretable) {
        // Only decode Op-level extensions if there are NOT AccountOptions-level extensions.
        // (Because otherwise we are not aligned on the Op-level extensions.)
        gobbled = deserializeBtsExtensionArrayType(buffer, bufferLength, &op->extensions);
        if (gobbled > bufferLength) {
            THROW(EXCEPTION);
        }
        read += gobbled; buffer += gobbled; bufferLength -= gobbled;
    }   // Else op->extensions is uninitialized, but it doesn't matter because we've already
        // flagged presence of uninterpretable data.  Obvs, this will need to become more
        // sophistcated if we later implement support for AccountOptions-level extensions.

    if (op->containsUninterpretable || op->extensions.count > 0) {
        op->containsUninterpretable = true;  // (Not worried about false pos on op->exts.count,
    } else {                                 // since this could only happen if containsUnint
        op->containsUninterpretable = false; // is already true.)
    }

    PRINTF("DESERIAL: OP_ACCOUNT_UPDATE: Read %d bytes; Buffer remaining: %d bytes\n", read, bufferLength);

    return read;

}
