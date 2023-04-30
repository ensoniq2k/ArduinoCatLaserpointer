#include "menu.h"

// Menu Headers --------
// ID, Label, First mnuItem_t ID, Last mnuItem_t ID, CurrentItem (only used for internal track keeping so set to 0)
const PROGMEM MD_Menu::mnuHeader_t menuHeader[] =
{
  { 10, "Main Menu",    10, 10, 0 },
  { 20, "X/Y Limits",   20, 23, 0 },
};

// Menu Items ----------
// ID, Label, 
// mnuAction_t = action to perform (Open a (sub)menu / change input value / input value with real time feedback, 
// ID of either the mnuHeader_t or menuInputsut_t depending on mnuAction_t
const PROGMEM MD_Menu::mnuItem_t menuItems[] =
{
  // Main (Root) menu
  { 10, "X/Y Limits",     MD_Menu::MNU_MENU,  20 },

  // X/Y Limits submenu
  { 20, "X Minimum",    MD_Menu::MNU_INPUT_FB, 20 },
  { 21, "X Maximum",    MD_Menu::MNU_INPUT_FB, 21 },
  { 22, "Y Minimum",    MD_Menu::MNU_INPUT_FB, 22 },
  { 23, "Y Maximum",    MD_Menu::MNU_INPUT_FB, 23 },
};

// Input Items ---------
//const PROGMEM char listOrientation[] = "Lefthanded|Top -> Bottom|Righthanded|Bottom -> Top";

// ID, Label, inputAction_t = type of input, callback for get/set of value, width of field on display, 
// min value, power min value, max value, power max value, base, list pointer vfor lists or engineering units only 
const PROGMEM MD_Menu::mnuInput_t menuInputs[] =
{
  // LED Output     
  { 20, "X Minimum",     MD_Menu::INP_INT,   menuSetMinimumX,  3,   1, 0,    180, 0, 10, nullptr },
  { 21, "X Maximum",     MD_Menu::INP_INT,   menuSetMaximumX,  3,   1, 0,    180, 0, 10, nullptr },
  { 22, "Y Minimum",     MD_Menu::INP_INT,   menuSetMinimumY,  3,   1, 0,    180, 0, 10, nullptr },
  { 23, "Y Minimum",     MD_Menu::INP_INT,   menuSetMaximumY,  3,   1, 0,    180, 0, 10, nullptr },
};

MD_Menu MainMenu(navigateMenu, displayMenu,        // user navigation and display
          menuHeader, ARRAY_SIZE(menuHeader), // menu header data
          menuItems, ARRAY_SIZE(menuItems), // menu item data
          menuInputs, ARRAY_SIZE(menuInputs));// menu input data

void initMenu() {
  displayMenu(MD_Menu::DISP_INIT);
  MainMenu.begin();
  MainMenu.setTimeout(MENU_TIMEOUT);
  MainMenu.setMenuWrap(true);
}