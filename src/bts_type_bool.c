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

#include <string.h>
#include "bts_type_bool.h"
#include "os.h"

uint32_t deserializeBtsBoolType(const uint8_t *buffer, uint32_t bufferLength, bts_bool_type_t * asset) {

    uint32_t read = 0;
    uint32_t gobbled = 0;
    uint8_t temp;

    gobbled = sizeof(uint8_t);          // BTS Bool is one byte, on the wire.
    os_memmove(&temp, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    *asset = temp ? true : false;       // But stdlib bool could be a different width

    PRINTF("DESERIAL: BOOL: %d; Read %d bytes; %d bytes remain\n",
           (int)asset, read, bufferLength);

    return read;

}

uint32_t prettyPrintBtsBoolType(const bts_bool_type_t asset, char * buffer) {

    uint32_t written = 0;

    // To ASCII:
    if (asset) {
        strcpy(buffer+written, "True");
    } else {
        strcpy(buffer+written, "False");
    }
    written = strlen(buffer);

    return written;
}

uint32_t prettyPrintBtsBoolTypeYesNo(const bts_bool_type_t asset, char * buffer) {

    uint32_t written = 0;

    // To ASCII:
    if (asset) {
        strcpy(buffer+written, "Yes");
    } else {
        strcpy(buffer+written, "No");
    }
    written = strlen(buffer);

    return written;
}
