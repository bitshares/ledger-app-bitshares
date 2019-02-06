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

#include "bts_stream.h"


/**
 * Updates operation-related members of the `content` structure to reflect `currentOperation`
 * member, which we assume has already been set.  Specifically, we update `argumentCount` and
 * `operationParser`.  As an ancillary side-effect, we also populate `txLabelDisplayBuffer`
 * and `txParamDisplayBuffer` with the current operation name to support user display.
 */
void updateOperationContent(txProcessingContent_t *content);

/**
 * Parsers for various known operations. Handles stringification of operation arguments
 * for display to user.
 */
void parseTransferOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum);
void parseLimitOrderCreateOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum);
void parseAccountUpgradeOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum);

/**
 * For operations that we know the name of but haven't written a parser for yet.
 */
void parseUnsupportedOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum);

/**
 * For operations that we just haven't a clue about.
 */
void parseUnknownOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum);

#endif
