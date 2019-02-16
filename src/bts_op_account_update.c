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

    if (op->accountOptionsPresent) {
        gobbled = deserializeBtsAccountOptionsType(buffer, bufferLength, &op->accountOptions);
        if (gobbled > bufferLength) {
            THROW(EXCEPTION);
        }
        read += gobbled; buffer += gobbled; bufferLength -= gobbled;
    }

    gobbled = deserializeBtsVarint32Type(buffer, bufferLength, &op->extensionsListLength);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    PRINTF("DESERIAL: OP_ACCOUNT_UPDATE: Read %d bytes; Buffer remaining: %d bytes\n", read, bufferLength);

    return read; // NOTE: bytes read is less than full buffer length
                 // since we didn't bother extracting some things.

}
