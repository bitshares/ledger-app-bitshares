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

#ifndef __APP_UX_H__
#define __APP_UX_H__

#include "os.h"
#include "os_io_seproxyhal.h"

extern ux_state_t ux;

// For display stepped screens:
extern unsigned int ux_step;
extern unsigned int ux_step_count;

unsigned int ui_address_nanos_button(unsigned int button_mask,
                                     unsigned int button_mask_counter);

unsigned int ui_approval_nanos_button(unsigned int button_mask,
                                      unsigned int button_mask_counter);

#endif
