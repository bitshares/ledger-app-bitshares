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

#ifndef __BTS_T_BOOL_H__
#define __BTS_T_BOOL_H__

#include <stdbool.h>
#include "os.h"

typedef bool bts_bool_type_t;

/**
 * Conversion from over-the-wire to stdlib bool type.  BitShares uses a single
 * byte for bool.  Stdlib may use a different width.
 */
uint32_t deserializeBtsBoolType(const uint8_t *buffer, uint32_t bufferLength, bts_bool_type_t * asset);

/**
 * Prints to buffer as "True" or "False".
 */
uint32_t prettyPrintBtsBoolType(bts_bool_type_t asset, char * buffer);

/**
 * Prints to buffer as "Yes" or "No".
 */
uint32_t prettyPrintBtsBoolTypeYesNo(bts_bool_type_t asset, char * buffer);

#endif
