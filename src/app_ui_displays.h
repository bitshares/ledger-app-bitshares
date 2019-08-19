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

#ifndef __APP_UI_DISPLAYS_H__
#define __APP_UI_DISPLAYS_H__

//
// UI "Buffers" - In what follows we define structs of display buffers
// used for the UI responses to various instruction requests.  Since we
// only ever service one instruction at a time, we allocate these
// structures as a union, and each instruction service cycle can use the
// union in its own way.
//

struct ui_buffers_address_s {
  char fullAddress[60];
};

struct ui_buffers_sign_tx_s {
  char paramLabel[48];  // Display line 1: Label for a tx parameter
  char paramValue[128]; // Display line 2: Value of a tx parameter
};

union ui_buffers_u {
  struct ui_buffers_address_s address;  // Used when serving INS_GET_PUBLIC_KEY
  struct ui_buffers_sign_tx_s sign_tx;  // Used when serving INS_SIGN_TX
};

extern union ui_buffers_u ui_buffers;   // Allocated in app_ui_displays.c

//
// UI definitions and supporting functions for each instruction workflow to be
// defined in app_ui_display_xxxxx.c.  Prototypes declared here so that only
// this one header file is needed for everything display related.
//

//
// UI support functions for INS_GET_PUBLIC_KEY workflow:
//      (defined in app_ui_display_address.c)
//

void ui_display_addressConfirmation_nanos();

//
// UI support functions for INS_SIGN_TX workflow:
//      (defined in app_ui_display_signtx.c)
//

void ui_display_signTxConfirmation_nanos();

// TBD

#endif
/// __APP_UI_DISPLAYS_H__
//
