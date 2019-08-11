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

#include "app_nvm.h"

WIDE internalStorage_t N_storage_real;
#define N_storage (*(WIDE internalStorage_t *)PIC(&N_storage_real))

void init_nvmstorage_ifneeded()
{
  if (N_storage.initialized != 0x01) {
      internalStorage_t storage;
      storage.dataAllowed = 0x00;
      storage.initialized = 0x01;
      nvm_write(&N_storage, (void *)&storage, sizeof(internalStorage_t));
  }
}

void set_nvmstorage_dataAllowed(unsigned int enabled) {
  uint8_t dataAllowed = enabled ? 1 : 0;
  nvm_write(&N_storage.dataAllowed, (void *)&dataAllowed, sizeof(uint8_t));
}

uint8_t get_nvmstorage_dataAllowed() {
  return N_storage.dataAllowed ? 1 : 0;
}
