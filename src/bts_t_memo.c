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

#include "bts_t_memo.h"
#include "os.h"

uint32_t deserializeBtsMemoType(const uint8_t *buffer, uint32_t bufferLength, bts_memo_type_t * memo) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = deserializeBtsPublicKeyType(buffer, bufferLength, &memo->fromPubkey);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsPublicKeyType(buffer, bufferLength, &memo->toPubkey);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = sizeof(uint64_t);
    os_memmove(&memo->nonce, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsVarint32Type(buffer, bufferLength, &memo->cipherTextLength);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    memo->cipherText = buffer;
    gobbled = memo->cipherTextLength;
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    PRINTF("DESERIAL: MEMO: %u cipher text bytes; Read %d bytes; %d bytes remain\n",
           memo->cipherTextLength, read, bufferLength);

    return read;

}
