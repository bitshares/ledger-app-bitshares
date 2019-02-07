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

#ifndef __BTS_OP_LIMIT_ORDER_CREATE_H__
#define __BTS_OP_LIMIT_ORDER_CREATE_H__

#include "bts_t_asset.h"
#include "bts_t_time.h"
#include "bts_t_bool.h"

typedef struct bts_operation_limit_order_create_t {
    bts_asset_type_t feeAsset;
    uint64_t         sellerId;
    bts_asset_type_t sellAsset;
    bts_asset_type_t buyAsset;
    bts_time_type_t  expires;
    bts_bool_type_t  fillOrKill;
} bts_operation_limit_order_create_t;

uint32_t deserializeBtsOperationLimitOrderCreate(const uint8_t *buffer, uint32_t bufferLength, bts_operation_limit_order_create_t * op);

#endif
