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

#ifndef __BTS_T_NULLSET_H__
#define __BTS_T_NULLSET_H__

#include "bts_t_varint.h"
#include "os.h"

/**
 * This is a placeholder type for unsupported list types. Used primarily for
 * extension lists that we are not yet supporting.  The type is essentially an alias
 * for varint32, which is used for set sizes. Only difference in implementation here
 * is we throw an exception if we deserialize a set size other than zero.  (Becuase,
 * presumably, we don't know how to deserialize whatever set elements might follow if
 * the size is non-zero.)
 */
typedef bts_varint32_type_t bts_null_set_type_t;

uint32_t deserializeBtsNullSetType(const uint8_t *buffer, uint32_t bufferLength, bts_null_set_type_t * asset);

#endif
