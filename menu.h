#pragma once

#include <MD_Menu.h>
#include <Adafruit_SSD1306.h>

#include "ArduinoCatLaserpointer.h"

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 32     // OLED display height, in pixels
#define SCREEN_RESET -1      // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define RST_STATE 1          //1-> Reset=close switch, 0->Reset=open switch

#define MENU_TIMEOUT 15000 // in milliseconds


extern MD_Menu::userNavAction_t menuNavAction;
extern MD_Menu MainMenu;

extern MD_Menu::value_t menuValueBuffer;  // Global variable to pass around menu callback values

MD_Menu::value_t *menuSetMinimumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMaximumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMinimumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMaximumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);

void initMenu();
bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg = nullptr);
MD_Menu::userNavAction_t navigateMenu();

