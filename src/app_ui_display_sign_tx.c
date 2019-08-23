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

#include "os.h"
#include "os_io_seproxyhal.h"

#include "app_ui_displays.h"
#include "app_ux.h"
#include "bts_stream.h"
#include "glyphs.h"

#define WITH_SIZE(x) x, sizeof(x)

const bagl_element_t ui_approval_nanos[] = {
    // type                               userid    x    y   w    h  str rad
    // fill      fg        bg      fid iid  txt   touchparams...       ]
    {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x00, 3, 12, 7, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_CROSS},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x00, 117, 13, 8, 6, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_CHECK},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    //{{BAGL_ICON                           , 0x01,  21,   9,  14,  14, 0, 0, 0
    //, 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_TRANSACTION_BADGE  }, NULL, 0, 0,
    //0, NULL, NULL, NULL },
    {{BAGL_LABELINE, 0x01, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Confirm",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x01, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Transaction",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_LABELINE, 0x02, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Tx ID",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x02, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26},
     (char *)ui_buffers.sign_tx.paramValue,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_LABELINE, 0x03, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     (char *)ui_buffers.sign_tx.paramLabel,    /* Operation n of m */
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x03, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 50},
     (char *)ui_buffers.sign_tx.paramValue,    /* Operation Name */
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_LABELINE, 0x04, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     (char *)ui_buffers.sign_tx.paramLabel,    /* Op Argument Label */
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x04, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26},
     (char *)ui_buffers.sign_tx.paramValue,    /* Op Argument Value */
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t *  ui_approval_prepro(const bagl_element_t *element)
{
    unsigned int display = 1;
    if (element->component.userid > 0)
    {
        if (ux_step > 3 && element->component.userid == 4) {
            display = 1;
        } else {
            display = (ux_step == element->component.userid - 1);
        }
        if (display)
        {
            PRINTF("STEP: %d Display: %d Element: %d\n", ux_step, display, element->component.userid);
            switch (element->component.userid)
            {
            case 1:
                UX_CALLBACK_SET_INTERVAL(2000);
                PRINTF("Transaction Confirmation Notice\n");

                break;
            case 2:
                PRINTF("Transaction Id or Hash\n");
                UX_CALLBACK_SET_INTERVAL(MAX(
                  3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));

                printTxId((char *)WITH_SIZE(ui_buffers.sign_tx.paramValue));

                break;
            case 3:
                PRINTF("Operation\n");
                UX_CALLBACK_SET_INTERVAL(MAX(
                  3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
                /* Operation name and label are already printed as side effect of
                 * updateOperationContent(). */

                break;
            case 4:
                PRINTF("Argument: %d - (step: %d count %d)\n", ux_step - 3, ux_step, ux_step_count);
                PRINTF("  CurrentOpIdx: %d\n", txContent.currentOperation);

                UX_CALLBACK_SET_INTERVAL(MAX(
                    3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));

                printTxOpArgument(ux_step - 3);
                break;
            }
        }
    }
    return (void *) display;
}

void ui_display_signTxConfirmation_nanos() {
  UX_DISPLAY(ui_approval_nanos, ui_approval_prepro);
}
