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

#include "bts_op_transfer.h"
#include "bts_types.h"
#include "os.h"

uint32_t deserializeBtsOperationTransfer(const uint8_t *buffer, uint32_t bufferLength, bts_operation_transfer_t * op) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = deserializeBtsAssetType(buffer, bufferLength, &op->feeAsset);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsAccountIdType(buffer, bufferLength, &op->fromId);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsAccountIdType(buffer, bufferLength, &op->toId);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsAssetType(buffer, bufferLength, &op->transferAsset);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsBoolType(buffer, bufferLength, &op->memoPresent);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    if (op->memoPresent) {
        gobbled = deserializeBtsMemoType(buffer, bufferLength, &op->memo);
        if (gobbled > bufferLength) {
            THROW(EXCEPTION);
        }
        read += gobbled; buffer += gobbled; bufferLength -= gobbled;
    }

    gobbled = deserializeBtsExtensionArrayType(buffer, bufferLength, &op->extensions);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    if (op->extensions.count > 0) {
      op->containsUninterpretable = true;
    } else {
      op->containsUninterpretable = false;
    }

    PRINTF("DESERIAL: OP_TRANSFER: Read %d bytes; Buffer remaining: %d bytes\n", read, bufferLength);

    return read;

}
