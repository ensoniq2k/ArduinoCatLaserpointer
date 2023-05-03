#include "menu.h"

// Menu Headers --------
// ID, Label, First mnuItem_t ID, Last mnuItem_t ID, CurrentItem (only used for internal track keeping so set to 0)
const PROGMEM MD_Menu::mnuHeader_t menuHeader[] =
{
  { 10, "Main Menu",      10, 13, 0 },
  { 20, "X/Y Limits",     20, 23, 0 },
  { 30, "Timer Settings", 30, 31, 0 },
};

// Menu Items ----------
// ID, Label, 
// mnuAction_t = action to perform (Open a (sub)menu / change input value / input value with real time feedback, 
// ID of either the mnuHeader_t or menuInputsut_t depending on mnuAction_t
const PROGMEM MD_Menu::mnuItem_t menuItems[] =
{
  // Main (Root) menu
  { 10, "X/Y Limits",               MD_Menu::MNU_MENU,        20 },
  { 11, "Timer Settings",           MD_Menu::MNU_MENU,        30 },
  { 12, "Display Font",             MD_Menu::MNU_INPUT_FB,    11 },
  { 13, "Save Settings",            MD_Menu::MNU_INPUT,       12 },
  
  // X/Y Limits submenu
  { 20, "X Minimum",                MD_Menu::MNU_INPUT_FB,    20 },
  { 21, "X Maximum",                MD_Menu::MNU_INPUT_FB,    21 },
  { 22, "Y Minimum",                MD_Menu::MNU_INPUT_FB,    22 },
  { 23, "Y Maximum",                MD_Menu::MNU_INPUT_FB,    23 },

  // Auto Timer   
  { 30, "Run duration",             MD_Menu::MNU_INPUT,       30 },
  { 31, "Run interval",             MD_Menu::MNU_INPUT,       31 },
};

// Input Items ---------
//const PROGMEM char listOrientation[] = "Lefthanded|Top -> Bottom|Righthanded|Bottom -> Top";
const PROGMEM char listFonts[] = "Adafruit|Arial|Arial bold|Callibri|Corsiva|Times|Verdana";


// ID, Label, inputAction_t = type of input, callback for get/set of value, width of field on display, 
// min value, power min value, max value, power max value, base, list pointer vfor lists or engineering units only 
const PROGMEM MD_Menu::mnuInput_t menuInputs[] =
{
  // Main
  { 11, "Font",             MD_Menu::INP_LIST,   menuSetFont,     10,   0, 0,    0, 0, 0, listFonts },
  { 12, "Save Settings",    MD_Menu::INP_RUN,    menuSaveConfig,  10,   0, 0,    0, 0, 0, nullptr },

  // X/Y Limits     
  { 20, "X Minimum",     MD_Menu::INP_INT,   menuSetMinimumX,  3,   1, 0,    180, 0, 10, nullptr },
  { 21, "X Maximum",     MD_Menu::INP_INT,   menuSetMaximumX,  3,   1, 0,    180, 0, 10, nullptr },
  { 22, "Y Minimum",     MD_Menu::INP_INT,   menuSetMinimumY,  3,   1, 0,    180, 0, 10, nullptr },
  { 23, "Y Minimum",     MD_Menu::INP_INT,   menuSetMaximumY,  3,   1, 0,    180, 0, 10, nullptr },

  // Auto Timer
  { 30, "Seconds",     MD_Menu::INP_INT,   menuSetRunDuration,  3,   1, 0,   900,   0, 10, nullptr },
  { 31, "Minutes",     MD_Menu::INP_INT,   menuSetRunInterval,  4,   15, 0,  1440,  0, 10, nullptr },
};

MD_Menu mainMenu(navigateMenu, displayMenu,        // user navigation and display
          menuHeader, ARRAY_SIZE(menuHeader), // menu header data
          menuItems, ARRAY_SIZE(menuItems), // menu item data
          menuInputs, ARRAY_SIZE(menuInputs));// menu input data

void initMenu() {
  displayMenu(MD_Menu::DISP_INIT);
  mainMenu.begin();
  mainMenu.setTimeout(MENU_TIMEOUT);
  mainMenu.setMenuWrap(false);
}