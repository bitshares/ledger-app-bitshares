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

#include "bts_t_asset.h"
#include "bts_types.h"
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

    uint8_t  p = desc.precision;
    uint64_t p10 = 1;
    while (p > 0) {
        p10 *= 10; --p;
    }   // (p is now zero)

    // Separate whole from fractional: (Note fractional will be zero if p10 == 1)
    uint64_t fractional = asset.amount % p10;
    uint64_t integral = (asset.amount - fractional)/p10;

    // Get leading zeros for fractional as p:
    while (fractional>0 && p10 > 1 && (p10/(fractional+1)) >= 10) {
        p10 /= 10;
        p++; // reusing p
    }

    // Clear trailing zeros for fractional part:
    while (fractional>0 && fractional%10==0) {
        fractional /= 10; // (Gobble a trailing zero)
    }

    // To ASCII:
    ui64toa(integral, buffer+written);          // Whole number part
    written = strlen(buffer);
    if (fractional > 0) {
        buffer[written] = '.';                  // Decimal separator
        written++;
        for ( ; p > 0 ; p--) {
            buffer[written] = '0';              // Fractional leading zeros
            written++;
        }
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

    // The bitAssets:
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
    case 110:
        desc->precision = 4;
        strcpy(desc->symbol, "bitRUB");
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
    case 1325:
        desc->precision = 5;
        strcpy(desc->symbol, "bitRUBLE");
        break;

    // Legacy gateway assets:
    case 850:
        desc->precision = 6;
        strcpy(desc->symbol, "OPEN.ETH");
        break;
    case 858:
        desc->precision = 8;
        strcpy(desc->symbol, "OPEN.DASH");
        break;
    case 859:
        desc->precision = 8;
        strcpy(desc->symbol, "OPEN.LTC");
        break;
    case 860:
        desc->precision = 4;
        strcpy(desc->symbol, "OPEN.DOGE");
        break;
    case 861:
        desc->precision = 8;
        strcpy(desc->symbol, "OPEN.BTC");
        break;
    case 973:
        desc->precision = 3;
        strcpy(desc->symbol, "OPEN.STEEM");
        break;
    case 1042:
        desc->precision = 6;
        strcpy(desc->symbol, "OPEN.USDT");
        break;
    case 1999:
        desc->precision = 6;
        strcpy(desc->symbol, "OPEN.EOS");
        break;
    case 3428:
        desc->precision = 4;
        strcpy(desc->symbol, "OPEN.XRP");
        break;
    case 4721:
        desc->precision = 3;
        strcpy(desc->symbol, "OPEN.IQ");
        break;
    case 1570:
        desc->precision = 8;
        strcpy(desc->symbol, "BRIDGE.BTC");
        break;
    case 3458:
        desc->precision = 8;
        strcpy(desc->symbol, "ESCODEX.BTC");
        break;

    // Autogenerated from script:
    case 1578:
        desc->precision = 4;
        strcpy(desc->symbol, "BADCOIN");
        break;
    case 1362:
        desc->precision = 5;
        strcpy(desc->symbol, "HERO");
        break;
    case 1382:
        desc->precision = 4;
        strcpy(desc->symbol, "HERTZ");
        break;
    case 4633:
        desc->precision = 5;
        strcpy(desc->symbol, "URTHR");
        break;
    case 4634:
        desc->precision = 5;
        strcpy(desc->symbol, "SKULD");
        break;
    case 4635:
        desc->precision = 5;
        strcpy(desc->symbol, "VERTHANDI");
        break;
    case 2241:
        desc->precision = 8;
        strcpy(desc->symbol, "GDEX.BTC");
        break;
    case 5286:
        desc->precision = 7;
        strcpy(desc->symbol, "GDEX.USDT");
        break;
    case 2598:
        desc->precision = 6;
        strcpy(desc->symbol, "GDEX.ETH");
        break;
    case 2635:
        desc->precision = 6;
        strcpy(desc->symbol, "GDEX.EOS");
        break;
    case 4200:
        desc->precision = 5;
        strcpy(desc->symbol, "GDEX.IQ");
        break;
    case 3926:
        desc->precision = 8;
        strcpy(desc->symbol, "RUDEX.BTC");
        break;
    case 4106:
        desc->precision = 4;
        strcpy(desc->symbol, "RUDEX.EOS");
        break;
    case 3715:
        desc->precision = 7;
        strcpy(desc->symbol, "RUDEX.ETH");
        break;
    case 1895:
        desc->precision = 3;
        strcpy(desc->symbol, "RUDEX.STEEM");
        break;
    case 4526:
        desc->precision = 3;
        strcpy(desc->symbol, "RUDEX.SMOKE");
        break;
    case 5542:
        desc->precision = 6;
        strcpy(desc->symbol, "RUDEX.USDT");
        break;
    case 2230:
        desc->precision = 4;
        strcpy(desc->symbol, "DEEX");
        break;
    case 3880:
        desc->precision = 8;
        strcpy(desc->symbol, "DEEX.BTC");
        break;
    case 4285:
        desc->precision = 6;
        strcpy(desc->symbol, "DEEX.EOS");
        break;
    case 3448:
        desc->precision = 6;
        strcpy(desc->symbol, "DEEX.ETH");
        break;
    case 3950:
        desc->precision = 8;
        strcpy(desc->symbol, "DEEX.LTC");
        break;
    case 4899:
        desc->precision = 8;
        strcpy(desc->symbol, "DEEX.STEEM");
        break;
    case 4248:
        desc->precision = 8;
        strcpy(desc->symbol, "DEEX.BCH");
        break;
    case 3948:
        desc->precision = 8;
        strcpy(desc->symbol, "DEEX.DASH");
        break;
    case 3951:
        desc->precision = 4;
        strcpy(desc->symbol, "DEEX.DOGE");
        break;
    case 4898:
        desc->precision = 8;
        strcpy(desc->symbol, "DEEX.MONERO");
        break;
    case 4386:
        desc->precision = 5;
        strcpy(desc->symbol, "DEEX.NEM");
        break;
    case 4157:
        desc->precision = 8;
        strcpy(desc->symbol, "XBTSX.BTC");
        break;
    case 4099:
        desc->precision = 6;
        strcpy(desc->symbol, "XBTSX.STH");
        break;
    case 5589:
        desc->precision = 6;
        strcpy(desc->symbol, "XBTSX.USDT");
        break;
    case 5259:
        desc->precision = 4;
        strcpy(desc->symbol, "XBTSX.EXR");
        break;
    case 5539:
        desc->precision = 7;
        strcpy(desc->symbol, "XBTSX.BCCX");
        break;
    case 4274:
        desc->precision = 8;
        strcpy(desc->symbol, "XBTSX.BCH");
        break;
    case 4275:
        desc->precision = 8;
        strcpy(desc->symbol, "XBTSX.BTG");
        break;
    case 4760:
        desc->precision = 7;
        strcpy(desc->symbol, "XBTSX.ETH");
        break;
    case 4159:
        desc->precision = 8;
        strcpy(desc->symbol, "XBTSX.LTC");
        break;
    case 4631:
        desc->precision = 8;
        strcpy(desc->symbol, "XBTSX.WAVES");
        break;
    case 5152:
        desc->precision = 6;
        strcpy(desc->symbol, "XBTSX.MDL");
        break;
    case 5641:
        desc->precision = 4;
        strcpy(desc->symbol, "HONEST.CNY");
        break;
    case 5649:
        desc->precision = 4;
        strcpy(desc->symbol, "HONEST.USD");
        break;
    case 5650:
        desc->precision = 8;
        strcpy(desc->symbol, "HONEST.BTC");
        break;
    case 5651:
        desc->precision = 8;
        strcpy(desc->symbol, "HONEST.XAU");
        break;
    case 5652:
        desc->precision = 8;
        strcpy(desc->symbol, "HONEST.XAG");
        break;
    case 5659:
        desc->precision = 6;
        strcpy(desc->symbol, "HONEST.ETH");
        break;
    case 5660:
        desc->precision = 6;
        strcpy(desc->symbol, "HONEST.XRP");
        break;
    case 5661:
        desc->precision = 6;
        strcpy(desc->symbol, "HONEST.XRP1");
        break;
    case 5662:
        desc->precision = 6;
        strcpy(desc->symbol, "HONEST.ETH1");
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
