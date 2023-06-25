#include "menu.h"

// Menu Headers --------
// ID, Label, First mnuItem_t ID, Last mnuItem_t ID, CurrentItem (only used for internal track keeping so set to 0)
const PROGMEM MD_Menu::mnuHeader_t menuHeader[] =
{
  { 10,                   MENUSTR_MAIN_MENU,       10, 14, 0 },
  { MENU_TIMER_SETTINGS, MENUSTR_TIMER_SETTINGS,  MENU_TIMER_ON_OFF,      MENU_TIMER_SLEEP_DAY,       0 },
  { MENU_LASER_SETTINGS, MENUSTR_LASER_SETTINGS,  MENU_LASER_BRIGHTNESS,  MENU_LASER_SHOW_BOUNDARIES, 0 },
};

// Menu Items ----------
// ID, Label, 
// mnuAction_t = action to perform (Open a (sub)menu / change input value / input value with real time feedback, 
// ID of either the mnuHeader_t or menuInputsut_t depending on mnuAction_t
const PROGMEM MD_Menu::mnuItem_t menuItems[] =
{
  // Main (Root) menu
  { MENU_NEXT_RUN,      MENUSTR_NEXT_RUN,        MD_Menu::MNU_INPUT,   MENU_NEXT_RUN },
  { MENU_RESTART_SLEEP, MENUSTR_RESTART_SLEEP,   MD_Menu::MNU_INPUT,   MENU_RESTART_SLEEP },
  { 12,                 MENUSTR_TIMER_SETTINGS,  MD_Menu::MNU_MENU,    MENU_TIMER_SETTINGS },
  { 13,                 MENUSTR_LASER_SETTINGS,  MD_Menu::MNU_MENU,    MENU_LASER_SETTINGS },
  { MENU_CENTER_SERVOS, MENUSTR_CENTER_SERVOS,   MD_Menu::MNU_INPUT,   MENU_CENTER_SERVOS },
  
  // X/Y Limits submenu
  { MENU_LASER_BRIGHTNESS,        MENUSTR_BRIGHTNESS,             MD_Menu::MNU_INPUT_FB,   MENU_LASER_BRIGHTNESS },
  { MENU_LASER_SIDE_LEFT,         MENUSTR_SIDE_LEFT,              MD_Menu::MNU_INPUT_FB,   MENU_LASER_SIDE_LEFT },
  { MENU_LASER_SIDE_RIGHT,        MENUSTR_SIDE_RIGHT,             MD_Menu::MNU_INPUT_FB,   MENU_LASER_SIDE_RIGHT },
  { MENU_LASER_FRONT_NEAR,        MENUSTR_FRONT_NEAR,             MD_Menu::MNU_INPUT_FB,   MENU_LASER_FRONT_NEAR },
  { MENU_LASER_FRONT_FAR,         MENUSTR_FRONT_FAR,              MD_Menu::MNU_INPUT_FB,   MENU_LASER_FRONT_FAR },
  { MENU_LASER_SPEED_MIN,         MENUSTR_SPEED_MIN,              MD_Menu::MNU_INPUT_FB,   MENU_LASER_SPEED_MIN },
  { MENU_LASER_SPEED_MAX,         MENUSTR_SPEED_MAX,              MD_Menu::MNU_INPUT_FB,   MENU_LASER_SPEED_MAX },
  { MENU_LASER_BLANKING_ON_OFF,   MENUSTR_LASER_BLANKING,         MD_Menu::MNU_INPUT,      MENU_LASER_BLANKING_ON_OFF },
  { MENU_LASER_SHOW_BOUNDARIES,   MENUSTR_LASER_SHOW_BOUNDARIES,  MD_Menu::MNU_INPUT,      MENU_LASER_SHOW_BOUNDARIES },

  // Auto Timer   
  { MENU_TIMER_ON_OFF,        MENUSTR_TIMER_ON_OFF,   MD_Menu::MNU_INPUT,   MENU_TIMER_ON_OFF },
  { MENU_TIMER_RUN_SEC,       MENUSTR_RUN_SECONDS,    MD_Menu::MNU_INPUT,   MENU_TIMER_RUN_SEC },
  { MENU_TIMER_RUN_MIN,       MENUSTR_RUN_MINUTES,    MD_Menu::MNU_INPUT,   MENU_TIMER_RUN_MIN },
  { MENU_TIMER_SLEEP_MIN,     MENUSTR_SLEEP_MINUTES,  MD_Menu::MNU_INPUT,   MENU_TIMER_SLEEP_MIN },
  { MENU_TIMER_SLEEP_HOUR,    MENUSTR_SLEEP_HOURS,    MD_Menu::MNU_INPUT,   MENU_TIMER_SLEEP_HOUR },
  { MENU_TIMER_SLEEP_DAY,     MENUSTR_SLEEP_DAYS,     MD_Menu::MNU_INPUT,   MENU_TIMER_SLEEP_DAY },
};

// ID, Label, inputAction_t = type of input, callback for get/set of value, width of field on display, 
// min value, power min value, max value, power max value, base, list pointer vfor lists or engineering units only 
const PROGMEM MD_Menu::mnuInput_t menuInputs[] =
{
  // Main
  { MENU_NEXT_RUN,      "",  MD_Menu::INP_RUN,   menuShowNextRun,        0,  0,  0,  0,  0,  10, nullptr },
  { MENU_RESTART_SLEEP, "",  MD_Menu::INP_RUN,   menuRestartSleepTimer,  0,  0,  0,  0,  0,  10, nullptr },
  { MENU_CENTER_SERVOS, "",  MD_Menu::INP_RUN,   menuCenterServos,      10,  0,  0,  0,  0,   0, nullptr },

  // Laser settings     
  { MENU_LASER_BRIGHTNESS,        MENUSTR_BRIGHTNESS,             MD_Menu::INP_INT,   menuLaserBrightness,      3, 1, 0, 10,              0, 10, nullptr },
  { MENU_LASER_SIDE_LEFT,         MENUSTR_SIDE_LEFT,              MD_Menu::INP_INT,   menuSetMinimumSide,       3, 1, 0, MENU_SIDE_MAX,   0, 10, nullptr },
  { MENU_LASER_SIDE_RIGHT,        MENUSTR_SIDE_RIGHT,             MD_Menu::INP_INT,   menuSetMaximumSide,       3, 1, 0, MENU_SIDE_MAX,   0, 10, nullptr },
  { MENU_LASER_FRONT_NEAR,        MENUSTR_FRONT_NEAR,             MD_Menu::INP_INT,   menuSetMaximumFront,      3, 1, 0, MENU_FRONT_MAX,  0, 10, nullptr },
  { MENU_LASER_FRONT_FAR,         MENUSTR_FRONT_FAR,              MD_Menu::INP_INT,   menuSetMinimumFront,      3, 1, 0, MENU_FRONT_MAX,  0, 10, nullptr },
  { MENU_LASER_SPEED_MIN,         MENUSTR_SPEED_MIN,              MD_Menu::INP_INT,   menuSetSpeedMin,          3, 1, 0, MENU_SPEED_MAX,  0, 10, nullptr },
  { MENU_LASER_SPEED_MAX,         MENUSTR_SPEED_MAX,              MD_Menu::INP_INT,   menuSetSpeedMax,          3, 1, 0, MENU_SPEED_MAX,  0, 10, nullptr },
  { MENU_LASER_BLANKING_ON_OFF,   MENUSTR_LASER_BLANKING,         MD_Menu::INP_BOOL,  menuSetLaserBlankout,     0, 0, 0, 0,               0, 10, nullptr },
  { MENU_LASER_SHOW_BOUNDARIES,   MENUSTR_LASER_SHOW_BOUNDARIES,  MD_Menu::INP_RUN,   menuLaserShowBoundaries,  0, 0, 0, 0,               0, 10, nullptr },

  // Timer settings
  { MENU_TIMER_ON_OFF,     MENUSTR_TIMER_ON_OFF,  MD_Menu::INP_BOOL,  menuSetTimerOnOff,   2,  0,  0,   0,  0,  10, nullptr },
  { MENU_TIMER_RUN_SEC,    MENUSTR_RUN_SECONDS,   MD_Menu::INP_INT,   menuSetRunDuration,  2,  0,  0,  59,  0,  10, nullptr },
  { MENU_TIMER_RUN_MIN,    MENUSTR_RUN_MINUTES,   MD_Menu::INP_INT,   menuSetRunDuration,  2,  0,  0,  30,  0,  10, nullptr },
  { MENU_TIMER_SLEEP_MIN,  MENUSTR_SLEEP_MINUTES, MD_Menu::INP_INT,   menuSetRunInterval,  2,  0,  0,  60,  0,  10, nullptr },
  { MENU_TIMER_SLEEP_HOUR, MENUSTR_SLEEP_HOURS,   MD_Menu::INP_INT,   menuSetRunInterval,  2,  0,  0,  24,  0,  10, nullptr },
  { MENU_TIMER_SLEEP_DAY,  MENUSTR_SLEEP_DAYS,    MD_Menu::INP_INT,   menuSetRunInterval,  2,  0,  0,  30,  0,  10, nullptr },
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
  mainMenu.setMenuItemIntWrap(false);
}