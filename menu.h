#pragma once

#include <MD_Menu.h>
#include "ArduinoCatLaserpointer.h"

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 32     // OLED display height, in pixels
#define SCREEN_RESET 4         // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define RST_STATE 1          //1-> Reset=close switch, 0->Reset=open switch

#define MENU_TIMEOUT 15000 // in milliseconds

#define MAG_MENU_ARRAY 1 // Used for the menu to make calculations of the mag array easier

// TEST CONFIG
#define LEFT_ESCAPE_SW 7  //Pin S2 enter or count up
#define RIGHT_ENTER_SW 8    //Pin S1 menue or count down

#define SIGNAL_PIN_N 1  //Pin signal negative in
#define SIGNAL_PIN_P 2  //Pin signal positive in
#define RESET_PIN 9     //Pin mag switch/reset switch
#define OUTPUT_PIN 3    //Pin output for LED or similar max. 5V 20mA


extern Adafruit_SSD1306 display;
extern MD_Menu::userNavAction_t menuNavAction;
extern MD_Menu MainMenu;

extern MD_Menu::value_t menuValueBuffer;  // Global variable to pass around menu callback values

MD_Menu::value_t *menuSetMinimumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMaximumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMinimumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMaximumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);

bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg = nullptr);
MD_Menu::userNavAction_t navigateMenu();

