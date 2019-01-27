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
#include "eos_utils.h"
#include "os.h"
#include <string.h>

uint32_t deserializeBtsAssetType(const uint8_t *buffer, uint32_t bufferLength, bts_asset_type_t * asset) {

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

uint32_t prettyPrintBtsAssetType(const bts_asset_type_t asset, char * buffer) {

    uint32_t written = 0;
    bts_asset_description_t desc;
    getBtsAssetDescription(asset, &desc);

    int64_t p = desc.precision;
    int64_t p10 = 1;
    while (p > 0) {
        p10 *= 10; --p;
    }

    // Separate whole from fractional:
    uint64_t fractional = asset.amount % p10;
    uint64_t integral = (asset.amount - fractional)/p10;

    while (fractional>0 && fractional%10==0) {
        fractional /= 10; // (Gobble trailing zeros)
    }

    // To ASCII:
    ui64toa(integral, buffer+written);          // Whole number part
    written = strlen(buffer);
    if (fractional > 0) {
        buffer[written] = '.';                  // Decimal separator
        written++;
        ui64toa(fractional, buffer+written);    // Fractional part
        written = strlen(buffer);
    }
    buffer[written] = ' ';                      // Space
    written++;
    strcpy(buffer+written, desc.symbol);        // Symbol
    written = strlen(buffer);

    return written;
}

bool getBtsAssetDescription(const bts_asset_type_t asset, bts_asset_description_t *desc) {

    uint32_t written = 0;
    bool known = true;

    // TODO: Hand-coded popular known assets, but need to code this
    // more as a look up table.
    switch (asset.instanceId) {
    case 0:
        desc->precision = 5;
        strcpy(desc->symbol, "BTS");
        break;
    case 102:
        desc->precision = 4;
        strcpy(desc->symbol, "bitKRW");
        break;
    case 103:
        desc->precision = 8;
        strcpy(desc->symbol, "bitBTC");
        break;
    case 105:
        desc->precision = 4;
        strcpy(desc->symbol, "bitSILVER");
        break;
    case 106:
        desc->precision = 6;
        strcpy(desc->symbol, "bitGOLD");
        break;
    case 113:
        desc->precision = 4;
        strcpy(desc->symbol, "bitCNY");
        break;
    case 114:
        desc->precision = 4;
        strcpy(desc->symbol, "bitMXN");
        break;
    case 120:
        desc->precision = 4;
        strcpy(desc->symbol, "bitEUR");
        break;
    case 121:
        desc->precision = 4;
        strcpy(desc->symbol, "bitUSD");
        break;
    case 850:
        desc->precision = 6;
        strcpy(desc->symbol, "OPEN.ETH");
        break;
    case 861:
        desc->precision = 8;
        strcpy(desc->symbol, "OPEN.BTC");
        break;
    case 1325:
        desc->precision = 5;
        strcpy(desc->symbol, "bitRUBLE");
        break;
    case 1570:
        desc->precision = 8;
        strcpy(desc->symbol, "BRIDGE.BTC");
        break;
    case 1999:
        desc->precision = 6;
        strcpy(desc->symbol, "OPEN.EOS");
        break;
    case 3428:
        desc->precision = 4;
        strcpy(desc->symbol, "OPEN.XRP");
        break;
    case 3458:
        desc->precision = 8;
        strcpy(desc->symbol, "ESCODEX.BTC");
        break;

    default:
        known = false;
        desc->precision = 0;
        strcpy(desc->symbol, "[1.3.");
        written = strlen(desc->symbol);
        ui64toa(asset.instanceId, desc->symbol+written);
        written = strlen(desc->symbol);
        strcpy(desc->symbol+written, "]");
        break;
    }

    return known;
}
