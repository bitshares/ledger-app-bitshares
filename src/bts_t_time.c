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

#include "bts_t_time.h"
#include "os.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

static void bloodyHackyDateDecode(struct tm * tm, time_t ctime); /* defined at end */

uint32_t deserializeBtsTimeType(const uint8_t *buffer, uint32_t bufferLength, bts_time_type_t * btsTime) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = sizeof(uint32_t);
    os_memmove(btsTime, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    PRINTF("DESERIAL: TIME: %.*H; Read %d bytes; %d bytes remain\n",
           sizeof(bts_time_type_t), btsTime, read, bufferLength);

    return read;

}

uint32_t prettyPrintBtsTimeType(const bts_time_type_t btsTime, char * buffer) {

    PRINTF("ONE\n");
    uint32_t written = 0;
    time_t ctime = (time_t)btsTime;     // Promote if time_t bigger than bts_time_type_t
    PRINTF("TWO\n");
    PRINTF("TWO %.*H\n", sizeof(time_t), &ctime);
    struct tm gmt;
    bloodyHackyDateDecode(&gmt, ctime);
    PRINTF("TWO-and-a-HALF\n");
    const char * fmt = "%04d-%02d-%02d T %02d:%02d:%02d UTC";

    PRINTF("THREE\n");
    // To ASCII:
    snprintf(buffer, 100, fmt, gmt.tm_year+1900, gmt.tm_mon+1, gmt.tm_mday+1,
                               gmt.tm_hour, gmt.tm_min, gmt.tm_sec);
    written = strlen(buffer);
    PRINTF("FOUR\n");

    return written;
}

/**
 * Ugh, incomplete time function availability in time.h. I *think* the following will
 * work up to the year 2100 then it will break due to extra leap year.  (We run out of
 * seconds in 2106 anyway.)
 */
static void bloodyHackyDateDecode(struct tm * tm, time_t ctime) {

    bool leap = false;
    int year = 0;
    int month = 0;
    int tmpdays;

    const unsigned int quadannums = (unsigned)ctime / 126230400u;
    ctime -= (quadannums * 126230400u);                               // 1970
    if (ctime >= 31536000) { year++; ctime-=31536000; leap=false;}    // 1971
    if (ctime >= 31536000) { year++; ctime-=31536000; leap=true;}     // 1972
    if (ctime >= 31622400) { year++; ctime-=31622400; leap=false;}    // 1973
    year += (70 + (4 * quadannums));    // Years since 1900

    const int yday = ctime / 86400;
    ctime -= (yday * 86400);
    tmpdays = yday;

    do {
        if (tmpdays >= 31) { month++; tmpdays-=31; } else break; // Feb
        if (tmpdays >= (leap?29:28))
                 { month++; tmpdays-=(leap?29:28); } else break; // March
        if (tmpdays >= 31) { month++; tmpdays-=31; } else break; // April
        if (tmpdays >= 30) { month++; tmpdays-=30; } else break; // May
        if (tmpdays >= 31) { month++; tmpdays-=31; } else break; // June
        if (tmpdays >= 30) { month++; tmpdays-=30; } else break; // July
        if (tmpdays >= 31) { month++; tmpdays-=31; } else break; // August
        if (tmpdays >= 31) { month++; tmpdays-=31; } else break; // September
        if (tmpdays >= 30) { month++; tmpdays-=30; } else break; // October
        if (tmpdays >= 31) { month++; tmpdays-=31; } else break; // November
        if (tmpdays >= 30) { month++; tmpdays-=30; } else break; // December
    } while (false);
    const int mday = tmpdays;

    tm->tm_year = year;
    tm->tm_yday = yday;
    tm->tm_mon  = month;
    tm->tm_mday = mday;

    tm->tm_hour = ctime / 3600;  ctime -= (tm->tm_hour * 3600);
    tm->tm_min = ctime / 60;
    tm->tm_sec = ctime % 60;

}
