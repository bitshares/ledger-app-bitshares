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

#ifndef __BTS_OP_ACCOUNT_UPGRADE_H__
#define __BTS_OP_ACCOUNT_UPGRADE_H__

#include "bts_asset_type.h"
#include "bts_type_bool.h"

typedef struct bts_operation_account_upgrade_t {
    bts_asset_type_t feeAsset;
    uint64_t         accountId;
    bts_bool_type_t  upgradeLtm;
} bts_operation_account_upgrade_t;

uint32_t deserializeBtsOperationAccountUpgrade(const uint8_t *buffer, uint32_t bufferLength, bts_operation_account_upgrade_t * op);

#endif
