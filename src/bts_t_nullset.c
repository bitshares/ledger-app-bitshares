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

#include "bts_t_nullset.h"
#include "os.h"

uint32_t deserializeBtsNullSetType(const uint8_t *buffer, uint32_t bufferLength, bts_null_set_type_t * asset) {

    uint32_t read = 0;
    uint32_t gobbled = 0;
    uint8_t temp;

    gobbled = deserializeBtsVarint32Type(buffer, bufferLength, asset);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    if (*asset != 0) {
        THROW(EXCEPTION);
    }

    PRINTF("DESERIAL: NULL_SET: %d; Read %d bytes; %d bytes remain\n",
           (int)(*asset), read, bufferLength);

    return read;

}
