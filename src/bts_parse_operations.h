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

#ifndef __BTS_PARSE_OPERATIONS_H__
#define __BTS_PARSE_OPERATIONS_H__

#include "eos_parse.h"
#include "bts_stream.h"

/**
 * Prints current operation name into a buffer inside the `content` structure.
 */
void printCurrentOperationName(txProcessingContent_t *content);

/**
 * Retrieves the number of arguments that need display for the current Operation
 * identified in the `content` structure.  If need be, it will parse the operation
 * data to determine if there are optional args.  (E.g. if memo is present in a
 * transfer operation, then arg count will be 5 instead of 4.)
 */
uint32_t getOperationArgumentCount(txProcessingContent_t *content);

void parseTransferOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum, actionArgument_t *arg);

#endif
