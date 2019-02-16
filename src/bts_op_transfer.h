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

#ifndef __BTS_OP_TRANSFER_H__
#define __BTS_OP_TRANSFER_H__

#include "bts_t_asset.h"
#include "bts_t_account.h"
#include "bts_t_bool.h"
#include "bts_t_memo.h"

typedef struct bts_operation_transfer_t {
    bts_asset_type_t feeAsset;
    bts_account_id_type_t fromId;
    bts_account_id_type_t toId;
    bts_asset_type_t transferAsset;
    bts_bool_type_t  memoPresent;
    bts_memo_type_t  memo;
} bts_operation_transfer_t;

uint32_t deserializeBtsOperationTransfer(const uint8_t *buffer, uint32_t bufferLength, bts_operation_transfer_t * op);

#endif
