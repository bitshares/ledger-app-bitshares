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

#include <string.h>
#include "bts_stream.h"
#include "os.h"
#include "cx.h"
#include "eos_types.h"
#include "eos_utils.h"
#include "eos_parse.h"
#include "eos_parse_token.h"
#include "eos_parse_unknown.h"

void initTxContext(txProcessingContext_t *context, 
                   cx_sha256_t *sha256,
                   cx_sha256_t *txIdSha256,
                   cx_sha256_t *dataSha256,
                   txProcessingContent_t *processingContent,
                   uint8_t dataAllowed) {
    os_memset(context, 0, sizeof(txProcessingContext_t));
    context->sha256 = sha256;
    context->txIdSha256 = txIdSha256;
    context->dataSha256 = dataSha256;
    context->content = processingContent;
    context->content->argumentCount = 0;
    context->state = TLV_CHAIN_ID;
    context->dataAllowed = dataAllowed;
    cx_sha256_init(context->sha256);
    cx_sha256_init(context->txIdSha256);
    cx_sha256_init(context->dataSha256);
}

uint8_t readTxByte(txProcessingContext_t *context) {
    uint8_t data;
    if (context->commandLength < 1) {
        PRINTF("readTxByte Underflow\n");
        THROW(EXCEPTION);
    }
    data = *context->workBuffer;
    context->workBuffer++;
    context->commandLength--;
    return data;
}

static void processTokenTransfer(txProcessingContext_t *context) {
    context->content->argumentCount = 3;
    uint32_t bufferLength = context->currentActionDataBufferLength;
    uint8_t *buffer = context->actionDataBuffer;

    buffer += 2 * sizeof(name_t) + sizeof(asset_t); 
    bufferLength -= 2 * sizeof(name_t) + sizeof(asset_t);
    uint32_t memoLength = 0;
    unpack_varint32(buffer, bufferLength, &memoLength);
    if (memoLength > 0) {
        context->content->argumentCount++;
    }
}


static void processUnknownAction(txProcessingContext_t *context) {
    cx_hash(&context->dataSha256->header, CX_LAST, context->dataChecksum, 0,
            context->dataChecksum);
    context->content->argumentCount = 3;  
}


/**
 *  TxID is first 20-bytes of hash of serialized transaction (excluding ChainID),
 *  displayed as 40 hex characters.  We display first and last 6 hex chars with
 *  '...' in the middle.
 */
void printTxId(txProcessingContext_t *context) {
    os_memset(context->content->txParamDisplayBuffer, 0, sizeof(context->content->txParamDisplayBuffer));
    array_hexstr(context->content->txParamDisplayBuffer, context->content->txIdHash, 3);
    os_memset(context->content->txParamDisplayBuffer+6, '.', 3);
    array_hexstr(context->content->txParamDisplayBuffer+9, context->content->txIdHash+17, 3);
}

void printOperationName(uint32_t opId, txProcessingContext_t *context) {
    char * opName;
    if(opId == 0) {  // TODO: Implement proper map to known OpIds
        opName = "Transfer";
    } else {
        opName = "**Unknown Operation**";
    }
    os_memset(context->content->txParamDisplayBuffer, 0, sizeof(context->content->txParamDisplayBuffer));
    os_memmove(context->content->txParamDisplayBuffer, opName,
               MIN(sizeof(context->content->txParamDisplayBuffer)-1,strlen(opName)));
}

void printArgument(uint8_t argNum, txProcessingContext_t *context) {
    /*
    name_t contractName = context->contractName;
    name_t actionName = context->contractActionName;
    */ // TODO: Need to extract OpID instead of Action here for bitshares
    uint8_t *buffer = context->operationDataBuffer;
    uint32_t bufferLength = context->currentOperationDataLength;
    actionArgument_t *arg =  &context->content->arg;

    if (true /* TODO: needs to select TRANSFER op */) {
        parseTransferOperation(buffer, bufferLength, argNum, arg);
        return;
    }

    if (context->dataAllowed == 1) {
        parseUnknownAction(context->dataChecksum, sizeof(context->dataChecksum), argNum, arg);
    }
}

/**
 * Sequentially hash an incoming data.
 * Hash functionality is moved out here in order to reduce 
 * dependencies on specific hash implementation.
*/
static void hashTxData(txProcessingContext_t *context, uint8_t *buffer, uint32_t length) {
    cx_hash(&context->sha256->header, 0, buffer, length, NULL);
    cx_hash(&context->txIdSha256->header, 0, buffer, length, NULL);
    /* Second hash because TxId excludes ChainID but message hash (what we sign)
     * includes it. (We just reinitialize txIdSha256 after reading ChainID.) */
}

/**
 * Used in processX(...) functions to gather and hash bytes from the APDU buffer.  If the
 * remaining unread bytes in the APDU buffer number less than what's needed to complete
 * current TLV field, then we stop at the end of the APDU buffer. (It will be refreshed
 * on the next host communication cycle.) If `buffer` is non-NULL, then store the gathered
 * bytes in `buffer`.  Else only hashing is needed for this field.
*/
static void processHelperGobbleCommandBytes(txProcessingContext_t *context, uint8_t *buffer) {
    uint32_t length =
        (context->commandLength <
                 ((context->currentFieldLength - context->currentFieldPos))
            ? context->commandLength
            : context->currentFieldLength - context->currentFieldPos);

    hashTxData(context, context->workBuffer, length);

    if (buffer != NULL) {
        os_memmove(buffer + context->currentFieldPos, context->workBuffer, length);
    }

    context->workBuffer += length;
    context->commandLength -= length;
    context->currentFieldPos += length;
}

/**
 * Process all fields that do not requre any processing except hashing.
 * The data comes in by chucks, so it may happen that buffer may contain 
 * incomplete data for particular field. Function designed to process 
 * everything until it receives all data for a particular field 
 * and after that will move to next field.
*/
static void processField(txProcessingContext_t *context) {

    if (context->currentFieldPos < context->currentFieldLength) {
        processHelperGobbleCommandBytes(context, NULL);
    }

    if (context->currentFieldPos == context->currentFieldLength) {
        context->state++;
        context->processingField = false;
    }
}

/**
 * Same as the generic processField() function, except we reinitialize the txIdSha256
 * hashing context so that ChainID is effectively excluded from that hash.
*/
static void processChainIdField(txProcessingContext_t *context) {

    if (context->currentFieldPos < context->currentFieldLength) {
        processHelperGobbleCommandBytes(context, NULL);
    }

    if (context->currentFieldPos == context->currentFieldLength) {
        cx_sha256_init(context->txIdSha256);  // (Re-init to exclude ChainID)
        context->state++;
        context->processingField = false;
    }
}

/**
 * Process Size fields that are expected to have Zero value. Except hashing the data, function
 * caches an incomming data. So, when all bytes for particulat field are received
 * do additional processing: Read actual number of actions encoded in buffer.
 * Throw exception if number is not '0'.
*/
static void processZeroSizeField(txProcessingContext_t *context) {

    if (context->currentFieldPos < context->currentFieldLength) {
        processHelperGobbleCommandBytes(context, context->sizeBuffer);
    }

    if (context->currentFieldPos == context->currentFieldLength) {
        uint32_t sizeValue = 0;
        unpack_varint32(context->sizeBuffer, context->currentFieldPos + 1, &sizeValue);
        if (sizeValue != 0) {
            PRINTF("processCtxFreeAction Action Number must be 0\n");
            THROW(EXCEPTION);
        }
        // Reset size buffer
        os_memset(context->sizeBuffer, 0, sizeof(context->sizeBuffer));

        // Move to next state
        context->state++;
        context->processingField = false;
    }
}

/**
 * Process Operation Count Field. Initialize context members operationCount and
 * operationsRemaining.  Checks permitted values against TX_MIN/MAX_OPERATIONS.
 */
static void processOperationListSizeField(txProcessingContext_t *context) {

    if (context->currentFieldPos < context->currentFieldLength) {
        processHelperGobbleCommandBytes(context, context->sizeBuffer);
    }

    if (context->currentFieldPos == context->currentFieldLength) {
        uint32_t sizeValue = 0;
        unpack_varint32(context->sizeBuffer, context->currentFieldPos + 1, &sizeValue);
        context->operationsRemaining = sizeValue;
        context->content->operationCount = 0;   // (Initial; Increments as OpIds read.)

        // Reset size buffer
        os_memset(context->sizeBuffer, 0, sizeof(context->sizeBuffer));

        if (sizeValue < TX_MIN_OPERATIONS || sizeValue > TX_MAX_OPERATIONS) {
            PRINTF("processOperationListSizeField: Too many or too few operations.\n");
            THROW(EXCEPTION);
        }

        // Move to next state
        context->state++;
        context->processingField = false;
    }
}

/**
 * Process Operation ID Field. Initialize context member currentOperationId.
 */
static void processOperationIdField(txProcessingContext_t *context) {

    if (context->currentFieldPos < context->currentFieldLength) {
        processHelperGobbleCommandBytes(context, context->sizeBuffer);
    }

    if (context->currentFieldPos == context->currentFieldLength) {
        uint32_t sizeValue = 0;
        unpack_varint32(context->sizeBuffer, context->currentFieldPos + 1, &sizeValue);
        context->currentOperationId = sizeValue;

        // Push-back into Content structure
        uint32_t opIdx = context->content->operationCount++;
        context->content->operationIds[opIdx] = sizeValue;

        // Reset size buffer
        os_memset(context->sizeBuffer, 0, sizeof(context->sizeBuffer));

        // Move to next state
        context->state++;
        context->processingField = false;
    }
}

/**
 * Process current operation payload field and store in into operation data buffer.
*/
static void processOperationDataField(txProcessingContext_t *context) {

    if (context->currentFieldLength > sizeof(context->operationDataBuffer) - 1) {
        PRINTF("processOperationData buffer overflow\n");
        THROW(EXCEPTION);
    }

    if (context->currentFieldPos < context->currentFieldLength) {
        processHelperGobbleCommandBytes(context, context->operationDataBuffer);
    }

    if (context->currentFieldPos == context->currentFieldLength) {
        context->currentOperationDataLength = context->currentFieldLength;

        /*
        if (context->contractActionName == EOSIO_TOKEN_TRANSFER) {
            processTokenTransfer(context);
        } else {
            THROW(EXCEPTION);
        }
        */
        // TODO: Specific preprocessing based on OperationID instead of ActionName
        /* TEMP */ context->content->argumentCount = 4; /* should be in specific preprocess */

        context->state++;
        context->processingField = false;
    }
}

static parserStatus_e processTxInternal(txProcessingContext_t *context) {
    for(;;) {
        if (context->state == TLV_DONE) {
            return STREAM_FINISHED;
        }
        if (context->commandLength == 0) {
            return STREAM_PROCESSING;
        }
        if (!context->processingField) {
            // While we are not processing a field, we should TLV parameters
            bool decoded = false;
            while (context->commandLength != 0) {
                bool valid;
                // Feed the TLV buffer until the length can be decoded
                context->tlvBuffer[context->tlvBufferPos++] =
                    readTxByte(context);

                decoded = tlvTryDecode(context->tlvBuffer, context->tlvBufferPos, 
                    &context->currentFieldLength, &valid);

                if (!valid) {
                    PRINTF("TLV decoding error\n");
                    return STREAM_FAULT;
                }
                if (decoded) {
                    break;
                }

                // Cannot decode yet
                // Sanity check
                if (context->tlvBufferPos == sizeof(context->tlvBuffer)) {
                    PRINTF("TLV pre-decode logic error\n");
                    return STREAM_FAULT;
                }
            }
            if (!decoded) {
                return STREAM_PROCESSING;
            }
            context->currentFieldPos = 0;
            context->tlvBufferPos = 0;
            context->processingField = true;
        }
        switch (context->state) {
        case TLV_CHAIN_ID:
            processChainIdField(context);
            break;

        case TLV_HEADER_EXPIRATION:
        case TLV_HEADER_REF_BLOCK_NUM:
        case TLV_HEADER_REF_BLOCK_PREFIX:
            processField(context);
            break;

        case TLV_OPERATION_LIST_SIZE:
            processOperationListSizeField(context);
            break;

        case TLV_OPERATION_CHECK_REMAIN:
            if(context->operationsRemaining > 0) {
                context->operationsRemaining--;
                context->state = TLV_OPERATION_ID;
            } else {
                context->state = TLV_TX_EXTENSION_LIST_SIZE;
            }
            break;

        case TLV_OPERATION_ID:
            processOperationIdField(context);
            if(!context->processingField) {             // if (we extracted an OpId) {
                switch (context->currentOperationId) {  //    then pick next state based on OpId
                    case 0x0:
                        context->state = TLV_OP_TRANSFER;
                        break;
                    default:
                        PRINTF("Unknown Operation ID");
                        THROW(EXCEPTION);  // TODO: pick a status byte for this so user knows why
                        break;
                }
            }
            break;

        case TLV_OP_TRANSFER_PAYLOAD:
            // Temporary until operation actually mapped out
            processOperationDataField(context);
            break;

        case TLV_OP_TRANSFER_DONE:
            context->state = TLV_OPERATION_CHECK_REMAIN;    // Go back and see if more operations
            break;

        case TLV_TX_EXTENSION_LIST_SIZE:
            processZeroSizeField(context);
            break;

        default:
            PRINTF("Invalid TLV decoder context\n");
            return STREAM_FAULT;
        }
    }
}

/**
 * Transaction processing should be done in a most efficient
 * way as possible, as EOS transaction size isn't fixed
 * and depends on action size. 
 * Also, Ledger Nano S have limited RAM resource, so data caching
 * could be very expencive. Due to these features and limitations
 * only some fields are cached before processing. 
 * All data is encoded by DER.ASN1 rules in plain way and serialized as a flat map.
 * 
 * Flat map is used in order to avoid nesting complexity.
 * 
 * Buffer serialization example:
 * [chain id][header][action number (1)][action 0][...]
 * Each field is encoded by DER rules.
 * Chain id field will be encoded next way:
 *  [Tag][Length][Value]
 * [0x04][ 0x20 ][chain id as octet string]
 * 
 * More infomation about DER Tag Length Value encoding is here: http://luca.ntop.org/Teaching/Appunti/asn1.html.
 * Only octet tag number is allowed. 
 * Value is encoded as octet string.
 * The length of the string is stored in Length byte(s)
 * 
 * Detailed flat map representation of incoming data:
 * [CHAIN ID][HEADER][CTX_FREE_ACTION_NUMBER][ACTION_NUMBER][ACTION 0][TX_EXTENSION_NUMBER][CTX_FREE_ACTION_DATA_NUMBER]
 * 
 * CHAIN ID:
 * [32 BYTES]
 * 
 * HEADER size may vary due to MAX_NET_USAGE_WORDS and DELAY_SEC serialization:
 * [EXPIRATION][REF_BLOCK_NUM][REF_BLOCK_PREFIX][MAX_NET_USAGE_WORDS][MAX_CPU_USAGE_MS][DELAY_SEC]
 * 
 * CTX_FREE_ACTION_NUMBER theoretically is not fixed due to serialization. Ledger accepts only 0 as encoded value.
 * ACTION_NUMBER theoretically is not fixed due to serialization. Ledger accepts only 1 as encoded value.
 * 
 * ACTION size may vary as authorization list and action data is dynamic:
 * [ACCOUNT][NAME][AUTHORIZATION_NUMBER][AUTHORIZATION 0][AUTHORIZATION 1]..[AUTHORIZATION N][ACTION_DATA]
 * ACCOUNT and NAME are 8 bytes long, both.
 * AUTHORIZATION_NUMBER theoretically is not fixed due to serialization.
 * ACTION_DATA is octet string of bytes.
 *  
 * AUTHORIZATION is 16 bytes long:
 * [ACTOR][PERMISSION]
 * ACTOR and PERMISSION are 8 bites long, both.
 * 
 * TX_EXTENSION_NUMBER theoretically is not fixed due to serialization. Ledger accepts only 0 as encoded value.
 * CTX_FREE_ACTION_DATA_NUMBER theoretically is not fixed due to serialization. Ledger accepts only 0 as encoded value.
*/
parserStatus_e parseTx(txProcessingContext_t *context, uint8_t *buffer, uint32_t length) {
    parserStatus_e result;
#ifdef DEBUG_APP
    // Do not catch exceptions.
    context->workBuffer = buffer;
    context->commandLength = length;
    result = processTxInternal(context);
#else
    BEGIN_TRY {
        TRY {
            context->workBuffer = buffer;
            context->commandLength = length;
            result = processTxInternal(context);
        }
        CATCH_OTHER(e) {
            result = STREAM_FAULT;
        }
        FINALLY {
        }
    }
    END_TRY;
#endif
    return result;
}