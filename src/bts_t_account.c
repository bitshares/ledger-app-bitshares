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

#include "bts_t_account.h"
#include "bts_types.h"
#include "eos_utils.h"
#include "os.h"
#include <string.h>

uint32_t deserializeBtsAccountIdType(const uint8_t *buffer, uint32_t bufferLength, bts_account_id_type_t * account) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = unpack_varint48(buffer, account);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    PRINTF("DESERIAL: ACCOUNT_ID: Read %d bytes; %d bytes remain\n", read, bufferLength);

    return read;

}

uint32_t prettyPrintBtsAccountIdType(const bts_account_id_type_t account, char * buffer) {

    uint32_t written = 0;

    // To ASCII:
    switch (account) {
    case 0:
        snprintf(buffer+written, 32, "committee-account");
        written = strlen(buffer);
        break;        
    case 1:
        snprintf(buffer+written, 32, "witness-account");
        written = strlen(buffer);
        break;        
    case 2:
        snprintf(buffer+written, 32, "relaxed-committee-account");
        written = strlen(buffer);
        break;        
    case 3:
        snprintf(buffer+written, 32, "null-account");
        written = strlen(buffer);
        break;        
    case 4:
        snprintf(buffer+written, 32, "temp-account");
        written = strlen(buffer);
        break;        
    case 5:
        snprintf(buffer+written, 32, "proxy-to-self");
        written = strlen(buffer);
        break;        
    default:
        ui64toa(account, buffer+written);
        written = strlen(buffer);
        break;
    }

    return written;
}
