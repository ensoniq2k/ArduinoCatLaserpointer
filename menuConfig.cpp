#include "menu.h"

// Menu Headers --------
// ID, Label, First mnuItem_t ID, Last mnuItem_t ID, CurrentItem (only used for internal track keeping so set to 0)
const PROGMEM MD_Menu::mnuHeader_t menuHeader[] =
{
  { 10, "Main Menu",            10, 15, 0 },
  { 20, "Laser limits",         20, 24, 0 },
  { 30, "Timer Settings",       30, 36, 0 },
};

// Menu Items ----------
// ID, Label, 
// mnuAction_t = action to perform (Open a (sub)menu / change input value / input value with real time feedback, 
// ID of either the mnuHeader_t or menuInputsut_t depending on mnuAction_t
const PROGMEM MD_Menu::mnuItem_t menuItems[] =
{
  // Main (Root) menu
  { 10, "Laser limits",             MD_Menu::MNU_MENU,        20 },
  { 11, "Timer Settings",           MD_Menu::MNU_MENU,        30 },
  { 12, "Display Font",             MD_Menu::MNU_INPUT_FB,    11 },
  { 13, "Save Settings",            MD_Menu::MNU_INPUT,       12 },
  { 14, "Center Servos",            MD_Menu::MNU_INPUT,       13 },
  { 15, "Laser Brightness",         MD_Menu::MNU_INPUT_FB,    14 },
  
  
  // X/Y Limits submenu
  { MENU_SIDE_LEFT,             "Side left",          MD_Menu::MNU_INPUT_FB,   MENU_SIDE_LEFT },
  { MENU_SIDE_RIGHT,            "Side right",         MD_Menu::MNU_INPUT_FB,   MENU_SIDE_RIGHT },
  { MENU_FRONT_FAR,             "Front far",          MD_Menu::MNU_INPUT_FB,   MENU_FRONT_FAR },
  { MENU_FRONT_NEAR,            "Front near",         MD_Menu::MNU_INPUT_FB,   MENU_FRONT_NEAR },
  { MENU_LASER_SHOW_BOUNDARIES, "Show boundaries",    MD_Menu::MNU_INPUT,      MENU_LASER_SHOW_BOUNDARIES },

  // Auto Timer   
  { 30,              "Next run?",          MD_Menu::MNU_INPUT,        30 },
  { 31,              "Restart sleep",      MD_Menu::MNU_INPUT,       31 },
  { MENU_RUN_SEC,    "Run seconds",        MD_Menu::MNU_INPUT,       MENU_RUN_SEC },
  { MENU_RUN_MIN,    "Run minutes",        MD_Menu::MNU_INPUT,       MENU_RUN_MIN },
  { MENU_SLEEP_MIN,  "Sleep minutes",      MD_Menu::MNU_INPUT,       MENU_SLEEP_MIN },
  { MENU_SLEEP_HOUR, "Sleep hours",        MD_Menu::MNU_INPUT,       MENU_SLEEP_HOUR },
  { MENU_SLEEP_DAY,  "Sleep days",         MD_Menu::MNU_INPUT,       MENU_SLEEP_DAY },
};

// Input Items ---------
//const PROGMEM char listOrientation[] = "Lefthanded|Top -> Bottom|Righthanded|Bottom -> Top";
const PROGMEM char listFonts[] = "Adafruit|Arial|Arial bold|Corsiva|Verdana";


// ID, Label, inputAction_t = type of input, callback for get/set of value, width of field on display, 
// min value, power min value, max value, power max value, base, list pointer vfor lists or engineering units only 
const PROGMEM MD_Menu::mnuInput_t menuInputs[] =
{
  // Main
  { 11, "Font",             MD_Menu::INP_LIST,    menuSetFont,         10,   0, 0,    0,  0, 0, listFonts },
  { 12, "Save Settings",    MD_Menu::INP_RUN,     menuSaveConfig,      10,   0, 0,    0,  0, 0, nullptr },
  { 13, "Center Servos",    MD_Menu::INP_RUN,     menuCenterServos,    10,   0, 0,    0,  0, 0, nullptr },
  { 14, "Brightness",       MD_Menu::INP_INT,     menuLaserBrightness,  2,   1, 0,    25, 0, 10, nullptr },

  // X/Y Limits     
  { MENU_SIDE_LEFT,             "Side left",          MD_Menu::INP_INT,   menuSetMinimumX,          3,   1, 0,    MENU_XY_MAX,  0, 10, nullptr },
  { MENU_SIDE_RIGHT,            "Side right",         MD_Menu::INP_INT,   menuSetMaximumX,          3,   1, 0,    MENU_XY_MAX,  0, 10, nullptr },
  { MENU_FRONT_FAR,             "Front far",          MD_Menu::INP_INT,   menuSetMinimumY,          3,   1, 0,    MENU_XY_MAX,  0, 10, nullptr },
  { MENU_FRONT_NEAR,            "Front near",         MD_Menu::INP_INT,   menuSetMaximumY,          3,   1, 0,    MENU_XY_MAX,  0, 10, nullptr },
  { MENU_LASER_SHOW_BOUNDARIES, "Show boundaries",    MD_Menu::INP_RUN,   menuLaserShowBoundaries,  0,   0, 0,    0,            0, 10, nullptr },

  // Auto Timer
  { 30,               "Next run?",            MD_Menu::INP_RUN,   menuShowNextRun,       0,   0,  0,  0,   0,  10, nullptr },
  { 31,               "Restart sleep",        MD_Menu::INP_RUN,   menuRestartSleepTimer, 0,   0,  0,  0,   0,  10, nullptr },
  { MENU_RUN_SEC,     "Run Seconds",          MD_Menu::INP_INT,   menuSetRunDuration,    2,   0,  0,  59,  0,  10, nullptr },
  { MENU_RUN_MIN,     "Run Minutes",          MD_Menu::INP_INT,   menuSetRunDuration,    2,   0,  0,  30,  0,  10, nullptr },
  { MENU_SLEEP_MIN,   "Sleep Minutes",        MD_Menu::INP_INT,   menuSetRunInterval,    2,   0,  0,  60,  0,  10, nullptr },
  { MENU_SLEEP_HOUR,  "Sleep Hours",          MD_Menu::INP_INT,   menuSetRunInterval,    2,   0,  0,  24,  0,  10, nullptr },
  { MENU_SLEEP_DAY,   "Sleep Days",           MD_Menu::INP_INT,   menuSetRunInterval,    2,   0,  0,  30,  0,  10, nullptr },
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