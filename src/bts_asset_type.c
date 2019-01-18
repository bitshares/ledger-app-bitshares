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

#include "bts_asset_type.h"
#include "eos_types.h"
#include "os.h"

uint32_t deserializeBtsAssetType(uint8_t *buffer, uint32_t bufferLength, bts_asset_type_t * asset) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = sizeof(uint64_t);
    os_memmove(&asset->amount, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = unpack_varint48(buffer, &asset->instanceId);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    PRINTF("DESERIAL: ASSET: %d [1.3.%d]; Read %d bytes; %d bytes remain\n",
           (int)asset->amount, (int)asset->instanceId, read, bufferLength);

    return read;

}
