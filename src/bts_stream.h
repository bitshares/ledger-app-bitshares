/*******************************************************************************
*
*  This file is a derivative work, and contains modifications from original
*  form.  The modifications are copyright of their respective contributors,
*  and are licensed under the same terms as the original work.
*
*  Portions Copyright (c) 2018, 2019 Christopher J. Sanborn
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

#ifndef __BTS_STREAM_H__
#define __BTS_STREAM_H__

#include "os.h"
#include "cx.h"
#include <stdbool.h>
#include "bts_types.h"

/* Limits on allowed transaction parameters that we will accept. (These
 * are not BitShares limits but rather limits in what we will handle.) */
#define TX_MIN_OPERATIONS 1
#define TX_MAX_OPERATIONS 4

enum {
    OP_TRANSFER = 0,
    OP_LIMIT_ORDER_CREATE,
    OP_LIMIT_ORDER_CANCEL,
    OP_CALL_ORDER_UPDATE,
    OP_FILL_ORDER, /* virtual */
    OP_ACCOUNT_CREATE,
    OP_ACCOUNT_UPDATE,
    OP_ACCOUNT_WHITELIST,
    OP_ACCOUNT_UPGRADE,
    OP_ACCOUNT_TRANSFER,
    OP_ASSET_CREATE,
    OP_ASSET_UPDATE,
    OP_ASSET_UPDATE_BITASSET,
    OP_ASSET_UPDATE_FEED_PRODUCERS,
    OP_ASSET_ISSUE,
    OP_ASSET_RESERVE,
    OP_ASSET_FUND_FEE_POOL,
    OP_ASSET_SETTLE,
    OP_ASSET_GLOBAL_SETTLE,
    OP_ASSET_PUBLISH_FEED,
    OP_WITNESS_CREATE,
    OP_WITNESS_UPDATE,
    OP_PROPOSAL_CREATE,
    OP_PROPOSAL_UPDATE,
    OP_PROPOSAL_DELETE,
    OP_WITHDRAW_PERMISSION_CREATE,
    OP_WITHDRAW_PERMISSION_UPDATE,
    OP_WITHDRAW_PERMISSION_CLAIM,
    OP_WITHDRAW_PERMISSION_DELETE,
    OP_COMMITTEE_MEMBER_CREATE,
    OP_COMMITTEE_MEMBER_UPDATE,
    OP_COMMITTEE_MEMBER_UPDATE_GLOBAL_PARAMETERS,
    OP_VESTING_BALANCE_CREATE,
    OP_VESTING_BALANCE_WITHDRAW,
    OP_WORKER_CREATE,
    OP_CUSTOM,
    OP_ASSERT,
    OP_BALANCE_CLAIM,
    OP_OVERRIDE_TRANSFER,
    OP_TRANSFER_TO_BLIND,
    OP_BLIND_TRANSFER,
    OP_TRANSFER_FROM_BLIND,
    OP_ASSET_SETTLE_CANCEL, /* virtual */
    OP_ASSET_CLAIM_FEES,
    OP_FBA_DISTRIBUTE,      /* virtual */
    OP_BID_COLLATERAL,
    OP_EXECUTE_BID,         /* virtual */
    OP_ASSET_CLAIM_POOL,
    OP_ASSET_UPDATE_ISSUER,
    OP_NUM_KNOWN_OPS
};
typedef uint32_t operationId_t;

/**
 * Function prototype for operation parsers.  Used so we can pass function reference as an
 * argument and resolve parser for correct operation type.
 */
typedef void operation_parser_f (const uint8_t *buffer, uint32_t bufferLength, uint8_t argIdx);

/***
 *  On Difference Between txProcessingContent_t and txProcessingContext_t:
 *
 *  (ConteNt vs ConteXt) ConteXt is used during decoding of the APDU workbuffer.  We are
 *  working with the DER-encoded serialized transaction data.  By the time we are done
 *  with that, we move on to the display-to-user phase, and we will have populated
 *  ConteNt.  The display phase reads from ConteNt and has no more need of ConteXt.
 *
 *  On Difference Between "processing" and "parsing":
 *
 *  Same distinction as above. You'll see functions named as "processTokenTransfer" or
 *  "parseTokenTransfer", e.g.  Processing is decoding from APDU workbuffer, and happens
 *  before display phase.  Parsing happens during display phase when we may need to
 *  extract text representation of a cached serialized argument.  There are also
 *  "printXXX" functions that write strings into the buffers refered to in the
 *  bagl_element arrays. (called from the _prepro UI phase.)
 */

typedef struct txProcessingContent_t {

    uint8_t txIdHash[32];               /* Not same as message hash; this is for TxID as
                                         * would be shown on a block explorer. */
    uint8_t argumentCount;              /* Argument count for *current* operation being
                                         * parsed */
    uint8_t subargRemainP1;             /* Some arguments have subarguments. ux_step
                                         * should only advance if this is zero. This holds
                                         * remaining subarguments PLUS ONE. The plus one
                                         * is important. It allows exit from countdown
                                         * without looping because zero on first time
                                         * through is the signal to init the counter. */
    uint32_t operationCount;            /* How many operation payloads have been written
                                         *  to operationDataBuffer */
    uint32_t currentOperation;          /* Index of currently displaying operation
                                         */
    operation_parser_f *operationParser;/* Function pointer to parser appropriate for
                                         * current operation */
    operationId_t operationIds[TX_MAX_OPERATIONS];/* OpId's of cached operation
                                                   * payloads */
    uint32_t operationOffsets[TX_MAX_OPERATIONS]; /* Offsets of NEXT payloads in buffer.
                                                   * Last used is offset to end+1 of the
                                                   * buffer and gives a total used length
                                                   * of the buffer */
    uint8_t operationDataBuffer[768];   /* Cache for Operation data.  We transcribe
                                         * recognized transaction payloads back-to-back in
                                         * this buffer for later parsing.  We use the
                                         * offset array to figure out where each next one
                                         * begins. */
} txProcessingContent_t;
extern txProcessingContent_t txContent;

/**
 *  Defines the order of the DER fields when processing a BitShares serialized transaction.
 *  See processTxInternal() for where this enum gets used.
 */
typedef enum txProcessingState_e {
    TLV_NONE = 0x0,             // Implies processing context not initialized
    TLV_CHAIN_ID = 0x1,         // Not part of serialized Tx; Prepended for hashing/signing
    TLV_HEADER_REF_BLOCK_NUM,   // Transaction begins here
    TLV_HEADER_REF_BLOCK_PREFIX,
    TLV_HEADER_EXPIRATION,
    TLV_OPERATION_LIST_SIZE,
    TLV_OPERATION_CHECK_REMAIN, // Branchpoint: Can go to _OPERATION_ID or _TX_EXTENSION_LIST_SIZE
    TLV_OPERATION_ID,           // Branchpoint: Goes to _TX_OP_XXX
    TLV_TX_EXTENSION_LIST_SIZE,
    TLV_DONE,
    /* Following regions define specific operation classes: */
    TLV_OP_SIMPLE,              // For simple, known operations. Hash and cache payload.
    TLV_OP_SIMPLE_PAYLOAD = TLV_OP_SIMPLE,
    TLV_OP_SIMPLE_DONE,         //   Return: goes back to _OPERATION_CHECK_REMAIN
    TLV_OP_COMPLEX,             // For complex ops where payload needs special handling
    TLV_OP_COMPLEX_PAYLOAD = TLV_OP_COMPLEX,            // before cacheing.
    TLV_OP_COMPLEX_DONE,        //   Return: goes back to _OPERATION_CHECK_REMAIN
    TLV_OP_UNSUPPORTED,         // For unknown or unsupported operations. Hash, but
    TLV_OP_UNSUPPORTED_PAYLOAD = TLV_OP_UNSUPPORTED,    // do not cache.
    TLV_OP_UNSUPPORTED_DONE,    //   Return: goes back to _OPERATION_CHECK_REMAIN
} txProcessingState_e;

typedef struct txProcessingContext_t {
    txProcessingState_e state;
    cx_sha256_t *sha256;
    cx_sha256_t *txIdSha256;
    uint32_t currentFieldLength;
    uint32_t currentFieldPos;
    uint32_t operationsRemaining;   // bitshares
    uint32_t currentOperationId;    // bitshares
    bool processingField;     // True: processing a field; False: decoding TLV header.
    uint8_t tlvBuffer[5];     // TODO: Does this need to be six?
    uint32_t tlvBufferPos;
    const uint8_t *workBuffer;// Points into the APDU buffer. Increment as we process.
    uint32_t commandLength;   // Bytes remaining in APDU buffer rel to workBuffer.
    uint8_t sizeBuffer[12];   // Used for caching VarInts for decoding
} txProcessingContext_t;

typedef enum parserStatus_e {
    STREAM_PROCESSING,
    STREAM_FINISHED,
    STREAM_FAULT
} parserStatus_e;

void initTxProcessingContext(cx_sha256_t *sha256, cx_sha256_t *txIdSha256);
bool checkInitTxProcessingContext();

void initTxProcessingContent();

parserStatus_e processTxStream(const uint8_t *buffer, uint32_t length);
               // Ingests serialized transaction bytes from the APDU
               // stream, extracting operation fields and storing in a
               // buffer for later parsing and display.

void printTxOpArgument(uint8_t argNum);
void printTxId(char * dispbuffer, size_t length);

#endif // __BTS_STREAM_H__
