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

#ifndef __BTS_TIME_TYPE_H__
#define __BTS_TIME_TYPE_H__

#include "os.h"

typedef uint32_t bts_time_type_t;

/**
 * Deserializes a Time element from a bytestream. Reads from `buffer`, not to exceed
 * `bufferLength`, and puts result into `time`. Returns number of bytes read from
 * `buffer`.  The bts time format is a 32-bit seconds-since-1970.  Fortunately, this
 * matches the time_t structure in BOLOS, so we can use library functions to get ascii
 * representation.  Unfortunately, leaves us with a year 2038 problem.  Not sure how
 * BitShares addresses this... but got a few years to find out...
 */
uint32_t deserializeBtsTimeType(const uint8_t *buffer, uint32_t bufferLength, bts_time_type_t * time);

/**
 * Print an ascii representation of the `time` element into buffer.  We get away with
 * using the library functions in time.h since our bts_time_type_t matches system
 * time_t.  (For now.)  Return value is number of chars written into buffer.
 */
uint32_t prettyPrintBtsTimeType(bts_time_type_t time, char * buffer);

#endif
