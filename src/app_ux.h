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

//
// App UX and UI:
//
// In the app_ux... files, we define what happens when events (primarily button
// presses and UX_TICKER events to cycle the display screen) occur during the
// servicing of an Instruction Service Routine (ISR).  This can include some
// complex app logic, like signing a transaction and sending back the signature
// over the APDU.  The files are generally organized by ISR:
//
//  >  app_ux.c           -  Global state and ticker event handling.
//  >  app_ux_address.c   -  Serves INS_GET_PUBLIC_KEY.
//  >  app_ux_sign_tx.c   -  Serves INS_SIGN.
//
// Do not confuse these files with the app_ui... files. In those, we are
// primarily concerned with what is displayed on the screen.  So what the app
// "displays" to the user, not what the app "does" in response to the user's
// actions.
//

//
// UX State and supporting variables:
//

extern ux_state_t ux;

extern unsigned int ux_step;        // For display stepped screens
extern unsigned int ux_step_count;  //

//
// Instruction Service Routine (ISR) Contexts:
//
// Basic work cycle of the app is to listen for payloads on the APDU transport
// stream. Each payload codes an "Instruction".  We service the instruction,
// and then go back to listening for the next instruction.  The instruction
// codes are defined in main.c, but the only some are relevent here. These are
// INS_GET_PUBLIC_KEY and INS_SIGN, as these are the ones that require context
// variables, which we map out here and store in a union (since we only service
// ONE instruction at a time, their contexts can overlap to save RAM).
//

typedef struct publicKeyContext_t {
    cx_ecfp_public_key_t publicKey;
    char address[60];
    uint8_t chainCode[32];
    bool getChaincode;
} publicKeyContext_t;

#define MAX_BIP32_PATH 10

typedef struct transactionContext_t {
    uint8_t pathLength;
    uint32_t bip32Path[MAX_BIP32_PATH];
    uint8_t hash[32];         // Message hash for which we will provide signature.
} transactionContext_t;

union ISRContext_u {
    publicKeyContext_t publicKeyContext;
    transactionContext_t transactionContext;
};

extern union ISRContext_u tmpCtx;

//
// Buttton Handlers for PublicKey and SignTransaction ISR's:
//

unsigned int ui_address_nanos_button(unsigned int button_mask,
                                     unsigned int button_mask_counter);

unsigned int ui_approval_nanos_button(unsigned int button_mask,
                                      unsigned int button_mask_counter);

uint32_t set_result_get_publicKey();

#endif
