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

#include "bts_t_pubkey.h"
#include "bts_types.h"
#include "eos_utils.h"
#include "os.h"
#include <string.h>

uint32_t deserializeBtsPublicKeyType(const uint8_t *buffer, uint32_t bufferLength, bts_public_key_type_t * keydata) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = sizeof(bts_public_key_type_t);
    os_memmove(keydata, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    PRINTF("DESERIAL: PUBKEY: Read %d bytes; %d bytes remain\n", read, bufferLength);

    return read;

}

uint32_t prettyPrintBtsPublicKeyType(const bts_public_key_type_t key, char * buffer) {

    uint32_t written = 0;

    compressed_public_key_to_wif((uint8_t*)&key, sizeof(key), buffer, 56);
    written = strlen(buffer);

    return written;
}
