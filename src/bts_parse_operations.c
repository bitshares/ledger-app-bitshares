/*******************************************************************************
*
*  This file is a derivative work, and contains modifications from original
*  form.  The modifications are copyright of their respective contributors,
*  and are licensed under the same terms as the original work.
*
*  Portions Copyright (c) 2019 Christopher J. Sanborn
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

#include "bts_parse_operations.h"
#include "bts_op_transfer.h"
#include "bts_op_limit_order_create.h"
#include "bts_types.h"
#include "eos_utils.h"
#include "os.h"
#include <string.h>

/**
 * Shortcuts for printing into the txContent Display buffers:
 */
#define printfContentLabel(...) snprintf(txContent.txLabelDisplayBuffer, sizeof(txContent.txLabelDisplayBuffer), __VA_ARGS__)
#define printfContentParam(...) snprintf(txContent.txParamDisplayBuffer, sizeof(txContent.txParamDisplayBuffer), __VA_ARGS__)

void updateOperationContent(txProcessingContent_t *content) {

    char * opName;

    switch (content->operationIds[content->currentOperation]) {
    case OP_TRANSFER:
        content->argumentCount = 4;
        content->operationParser = parseTransferOperation;
        opName = "Transfer";
        break;
    case OP_LIMIT_ORDER_CREATE:
        content->argumentCount = 6;
        content->operationParser = parseLimitOrderCreateOperation;
        opName = "Create Limit Order";
        break;
    case OP_LIMIT_ORDER_CANCEL:
        content->argumentCount = 2;
        content->operationParser = parseUnsupportedOperation;
        opName = "Cancel Limit Order";
        break;
    default:
        content->argumentCount = 2;
        content->operationParser = parseUnknownOperation;
        opName = "** Unknown Operation **";
        break;
    }

    os_memset(content->txParamDisplayBuffer, 0, sizeof(content->txParamDisplayBuffer));
    os_memmove(content->txParamDisplayBuffer, opName,
               MIN(sizeof(content->txParamDisplayBuffer)-1,strlen(opName)));
    os_memset(content->txLabelDisplayBuffer, 0, sizeof(content->txLabelDisplayBuffer));
    snprintf(content->txLabelDisplayBuffer, sizeof(content->txLabelDisplayBuffer),
             "Operation %u of %u", content->currentOperation+1, content->operationCount);

}

void parseTransferOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum) {
    uint32_t read = 0;
    bts_operation_transfer_t op;

    // Read fields:
    read += deserializeBtsOperationTransfer(buffer, bufferLength, &op);

    if (argNum == 0) {
        printfContentLabel("Amount");
        prettyPrintBtsAssetType(op.transferAsset, txContent.txParamDisplayBuffer);
    } else if (argNum == 1) {
        printfContentLabel("From");
        ui64toa(op.fromId, txContent.txParamDisplayBuffer);
    } else if (argNum == 2) {
        printfContentLabel("To");
        ui64toa(op.toId, txContent.txParamDisplayBuffer);
    } else if (argNum == 3) {
        printfContentLabel("Fee");
        prettyPrintBtsAssetType(op.feeAsset, txContent.txParamDisplayBuffer);
    }
}

void parseLimitOrderCreateOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum) {
    uint32_t read = 0;
    bts_operation_limit_order_create_t op;

    // Read fields:
    read += deserializeBtsOperationLimitOrderCreate(buffer, bufferLength, &op);

    if (argNum == 0) {
        printfContentLabel("Seller");
        ui64toa(op.sellerId, txContent.txParamDisplayBuffer);
    } else if (argNum == 1) {
        printfContentLabel("Amount to Sell");
        prettyPrintBtsAssetType(op.sellAsset, txContent.txParamDisplayBuffer);
    } else if (argNum == 2) {
        printfContentLabel("Amount to Buy");
        prettyPrintBtsAssetType(op.buyAsset, txContent.txParamDisplayBuffer);
    } else if (argNum == 3) {
        printfContentLabel("Expires");
        prettyPrintBtsTimeType(op.expires, txContent.txParamDisplayBuffer);
    } else if (argNum == 4) {
        printfContentLabel("Fill or Kill");
        printfContentParam("[bool]");
    } else if (argNum == 5) {
        printfContentLabel("Fee");
        prettyPrintBtsAssetType(op.feeAsset, txContent.txParamDisplayBuffer);
    }
}

void parseUnsupportedOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum) {

    if (argNum == 0) {
        printfContentLabel("Unsupported Operation");
        printfContentParam("Cannot display details.");
    } else if (argNum == 1) {
        printfContentLabel("Warning!");
        printfContentParam("Use Discretion; Confirm at Own Risk.");
    }
}

void parseUnknownOperation(const uint8_t *buffer, uint32_t bufferLength, uint8_t argNum) {

    if (argNum == 0) {
        printfContentLabel("Unrecognized Operation");
        printfContentParam("Cannot display details.");
    } else if (argNum == 1) {
        printfContentLabel("Warning!");
        printfContentParam("Use Discretion; Confirm at Own Risk.");
    }
}
