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

#ifndef __APP_UI_MENUS_H__
#define __APP_UI_MENUS_H__

#include "os.h"
#include "os_io_seproxyhal.h"

/**
 * Returns the UI back to the first item of main menu ("Use wallet
 * to...").  Called at app start and any time we complete handling of
 * a user instruction request.
*/
void ui_idle(void);

#endif
/// __APP_UI_MENUS_H__
//
