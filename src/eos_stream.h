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

    TLV_NONE = 0x0,             // Implies processing context not initialized
    TLV_CHAIN_ID = 0x1,         // Not part of serialized Tx but is prepended for hashing/signing
    TLV_HEADER_REF_BLOCK_NUM,   // Transaction begins here
    TLV_HEADER_REF_BLOCK_PREFIX,
    TLV_HEADER_EXPIRATION,
    TLV_OPERATION_LIST_SIZE,
    TLV_OPERATION_CHECK_REMAIN, // Branchpoint: Can go to _OPERATION_ID or _TX_EXTENSION_LIST_SIZE
    TLV_OPERATION_ID,           // Branchpoint: Goes to _TX_OP_XXX
    TLV_TX_EXTENSION_LIST_SIZE,
    TLV_DONE,

    /* Following regions define specific operations: */

    TLV_OP_TRANSFER,
    TLV_OP_TRANSFER_PAYLOAD = TLV_OP_TRANSFER,
    TLV_OP_TRANSFER_DONE,       // Return: goes back to _OPERATION_CHECK_REMAIN

    /* Following state tags will be removed, but are temporarily being retained because
     * their handling presents useful case studies. */
    TLV_ACTION_DATA_SIZE,
    TLV_ACTION_DATA
} txProcessingState_e;

/* Limits on allowed transaction parameters that we will accept. (These
 * are not BitShares limits but rather limits in what we will handle.) */
#define TX_MIN_OPERATIONS 1
#define TX_MAX_OPERATIONS 1

typedef struct txProcessingContext_t {
    txProcessingState_e state;
    cx_sha256_t *sha256;
    cx_sha256_t *dataSha256;
    uint32_t currentFieldLength;
    uint32_t currentFieldPos;
    uint32_t currentAutorizationIndex;
    uint32_t currentAutorizationNumber;
    uint32_t currentActionDataBufferLength;
    uint32_t operationCount;        // bitshares
    uint32_t operationsRemaining;   // bitshares
    uint32_t currentOperationId;    // bitshares
    bool processingField;     // True: processing a field; False: decoding TLV header.
    uint8_t tlvBuffer[5];     // TODO: Does this need to be six?
    uint32_t tlvBufferPos;
    uint8_t *workBuffer;      // Points into the APDU buffer. Increment as we process.
    uint32_t commandLength;   // Bytes remaining in APDU buffer rel to workBuffer.
    name_t contractName;
    name_t contractActionName;
    uint8_t sizeBuffer[12];   // Used for caching VarInts for decoding
    uint8_t actionDataBuffer[512];  // Used for caching Operation data
    uint8_t dataAllowed;      // Accept unknown Operation types?  Or throw?
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
