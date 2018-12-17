/*******************************************************************************
*
*  This file is a derivative work, and contains modifications from original
*  form.  The modifications are copyright of their respective contributors,
*  and are licensed under the same terms as the original work.
*
*  Portions Copyright (c) 2018 Christopher J. Sanborn
*
*  Original copyright and license notice follows:
*
*   Taras Shchybovyk
*   (c) 2018 Taras Shchybovyk
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

#ifndef __EOS_STREAM_H__
#define __EOS_STREAM_H__

#include "os.h"
#include "cx.h"
#include <stdbool.h>
#include "eos_types.h"
#include "eos_parse.h"

typedef struct txProcessingContent_t {
    char argumentCount;
    char contract[14];
    char action[14];
    actionArgument_t arg;
} txProcessingContent_t;

typedef enum txProcessingState_e {
   /*  Lists the order of the DER fields when processing a BitShares serialized transaction.
    *
    *  To adapt from the EOS wallet, I selected just the BTS-relevant fields (moving the rest
    *  below TLV_DONE) and repurposed a few. But to finish this, some will need to be removed
    *  and some renamed, and we can't just take the operation payload as a monolithic unit
    *  since we want to extract specifics for display to user for confirmation.
    */
    TLV_NONE = 0x0,
    TLV_CHAIN_ID = 0x1,         // Not part of serialized Tx but is prepended for hashing/signing
    TLV_HEADER_REF_BLOCK_NUM,
    TLV_HEADER_REF_BLOCK_PREFIX,
    TLV_HEADER_EXPITATION,      // TODO: Typo
    TLV_ACTION_LIST_SIZE,       // This will become TLV_OPERATION_LIST_SIZE; Throws if not == 1 (But we may want to support chained ops eventually)
    TLV_AUTHORIZATION_ACTOR,    // This will become TLV_OPERATION_ID
    TLV_CONTEXT_FREE_DATA,      // Using as OPERATION_PAYLOAD for now but will need to decide how to handle differnt ops.
    TLV_TX_EXTENSION_LIST_SIZE, // Size of Tx extension list.  Will throw if not zero.
    TLV_DONE,
    // Following state tags will likely be removed, or expanded into op-specific state tags
    TLV_HEADER_MAX_NET_USAGE_WORDS,  // NOT USED in BitShares
    TLV_HEADER_MAX_CPU_USAGE_MS,     // NOT USED in BitShares
    TLV_HEADER_DELAY_SEC,            // NOT USED in BitShares
    TLV_CFA_LIST_SIZE,               // No Parallel in BitShares
    TLV_ACTION_ACCOUNT,
    TLV_ACTION_NAME,
    TLV_AUTHORIZATION_LIST_SIZE,
    TLV_AUTHORIZATION_PERMISSION,
    TLV_ACTION_DATA_SIZE,
    TLV_ACTION_DATA
} txProcessingState_e;

/*
typedef enum txProcessingState_e {
    TLV_NONE = 0x0, 
    TLV_CHAIN_ID = 0x1,
    TLV_HEADER_EXPITATION,
    TLV_HEADER_REF_BLOCK_NUM,
    TLV_HEADER_REF_BLOCK_PREFIX,
    TLV_HEADER_MAX_NET_USAGE_WORDS,
    TLV_HEADER_MAX_CPU_USAGE_MS,
    TLV_HEADER_DELAY_SEC,
    TLV_CFA_LIST_SIZE,
    TLV_ACTION_LIST_SIZE,
    TLV_ACTION_ACCOUNT,
    TLV_ACTION_NAME,
    TLV_AUTHORIZATION_LIST_SIZE,
    TLV_AUTHORIZATION_ACTOR,
    TLV_AUTHORIZATION_PERMISSION,
    TLV_ACTION_DATA_SIZE,
    TLV_ACTION_DATA,
    TLV_TX_EXTENSION_LIST_SIZE,
    TLV_CONTEXT_FREE_DATA,
    TLV_DONE
} txProcessingState_e;
*/

typedef struct txProcessingContext_t {
    txProcessingState_e state;
    cx_sha256_t *sha256;
    cx_sha256_t *dataSha256;
    uint32_t currentFieldLength;
    uint32_t currentFieldPos;
    uint32_t currentAutorizationIndex;
    uint32_t currentAutorizationNumber;
    uint32_t currentActionDataBufferLength;
    bool processingField;
    uint8_t tlvBuffer[5];
    uint32_t tlvBufferPos;
    uint8_t *workBuffer;
    uint32_t commandLength;
    name_t contractName;
    name_t contractActionName;
    uint8_t sizeBuffer[12];
    uint8_t actionDataBuffer[512];
    uint8_t dataAllowed;
    checksum256 dataChecksum;
    txProcessingContent_t *content;
} txProcessingContext_t;

typedef enum parserStatus_e {
    STREAM_PROCESSING,
    STREAM_FINISHED,
    STREAM_FAULT
} parserStatus_e;

void initTxContext(
    txProcessingContext_t *context, 
    cx_sha256_t *sha256, 
    cx_sha256_t *dataSha256,
    txProcessingContent_t *processingContent, 
    uint8_t dataAllowed
);
parserStatus_e parseTx(txProcessingContext_t *context, uint8_t *buffer, uint32_t length);

void printArgument(uint8_t argNum, txProcessingContext_t *processingContext);

#endif // __EOS_STREAM_H__
