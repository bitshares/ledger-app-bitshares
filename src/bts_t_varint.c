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

#include "bts_t_varint.h"
#include "bts_types.h"
#include "eos_utils.h"
#include "os.h"
#include <string.h>

uint32_t deserializeBtsVarint48Type(const uint8_t *buffer, uint32_t bufferLength, bts_varint48_type_t * asset) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = unpack_varint48(buffer, asset);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    return read;

}

uint32_t prettyPrintBtsVarint48Type(const bts_varint48_type_t asset, char * buffer) {

    uint32_t written = 0;

    ui64toa(asset, buffer+written);
    written = strlen(buffer);

    return written;

}

uint32_t deserializeBtsVarint32Type(const uint8_t *buffer, uint32_t bufferLength, bts_varint32_type_t * asset) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = unpack_varint32(buffer, asset);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    return read;

}

uint32_t prettyPrintBtsVarint32Type(const bts_varint32_type_t asset, char * buffer) {

    uint32_t written = 0;

    ui64toa((uint64_t)asset, buffer+written);
    written = strlen(buffer);

    return written;

}
