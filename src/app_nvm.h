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

/****************************************************************
 *  app_nvm.h:  Application Non-Volatile Memory (NVM) handling.
 *
 ****************************************************************/

#ifndef __APP_NVM_H__
#define __APP_NVM_H__

#include "os.h"

/*
 * Struct internalStorage_s is technically an implementation detail,
 * and could be moved to app_nvm.c, but I keep it here for ease of
 * reference/documentation.
*/

typedef struct internalStorage_s internalStorage_t;

struct internalStorage_s {
    uint8_t dataAllowed;
    uint8_t initialized;
};

/*
 * Public interface:
*/

void    init_nvmstorage_ifneeded();

uint8_t get_nvmstorage_dataAllowed();
void    set_nvmstorage_dataAllowed(unsigned int enabled);


#endif
/// __APP_NVM_H__
//
