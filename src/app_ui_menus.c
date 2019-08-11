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

#include "app_ui_menus.h"
#include "app_nvm.h"
#include "glyphs.h"

const ux_menu_entry_t menu_main[];
const ux_menu_entry_t menu_settings[];
const ux_menu_entry_t menu_settings_arbdata[];
const ux_menu_entry_t menu_about[];

void menu_settings_arbdata_entry(unsigned int ignored);  // Called in Settings menu
void menu_settings_arbdata_change(unsigned int newval);  // ''

/**
 *  MainMenu:
 *   |
 *   |-> "Use Wallet..."   <--  ui_idle( ) brings us here.
 *   |
 *   |-> Settings
 *   |     |
 *   |     |-> Arbitrary Data
 *   |     |     |-> No
 *   |     |     \-> Yes
 *   |     |
 *   |     \-> Back
 *   |
 *   |-> About
 *   |     |-> Version
 *   |     \-> Back
 *   |
 *   \-> Quit
*/
const ux_menu_entry_t menu_main[] = {
    {NULL, NULL, 0, &C_nanos_badge_bitshares, "Use wallet to", "view accounts", 33, 12},
    {menu_settings, NULL, 0, NULL, "Settings", NULL, 0, 0},
    {menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
    {NULL, os_sched_exit, 0, &C_icon_dashboard, "Quit app", NULL, 50, 29},
    UX_MENU_END};

const ux_menu_entry_t menu_settings[] = {
    {NULL, menu_settings_arbdata_entry, 0, NULL, "Arbitrary data", NULL, 0, 0},
    {menu_main, NULL, 1, &C_icon_back, "Back", NULL, 61, 40},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_arbdata[] = {
    {NULL, menu_settings_arbdata_change, 0, NULL, "No", NULL, 0, 0},
    {NULL, menu_settings_arbdata_change, 1, NULL, "Yes", NULL, 0, 0},
    UX_MENU_END};

const ux_menu_entry_t menu_about[] = {
    {NULL, NULL, 0, NULL, "Version", APPVERSION, 0, 0},
    {menu_main, NULL, 2, &C_icon_back, "Back", NULL, 61, 40},
    UX_MENU_END};


/**
 * Called on entry to Main->Settings-ArbitraryData. Results in
 * selecting correct active entry from NVM setting.
*/
void menu_settings_arbdata_entry(unsigned int ignored) {
    UNUSED(ignored);
    UX_MENU_DISPLAY(get_nvmstorage_dataAllowed()?1:0, menu_settings_arbdata, NULL);
}

/**
 * Called from Main->Settings->ArbitrayData.  Changes the setting when
 * user selects either Yes or No.
*/
void menu_settings_arbdata_change(unsigned int newval) {
    set_nvmstorage_dataAllowed(newval);       // Set new value
    UX_MENU_DISPLAY(0, menu_settings, NULL);  // Return to Settings menu
}

/**
 * Returns UI to top of main menu ("Use wallet to...").
*/
void ui_idle(void)
{
    UX_MENU_DISPLAY(0, menu_main, NULL);
}
