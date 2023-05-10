#pragma once

#include <Arduino.h>
#include <MD_Menu.h>

#include "ArduinoCatLaserpointer.h"

#define SCREEN_ADDRESS 0x3C  /// See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

#define MENU_SIDE_LEFT 20
#define MENU_SIDE_RIGHT 21
#define MENU_FRONT_FAR 22
#define MENU_FRONT_NEAR 23

#define MENU_RUN_SEC 32
#define MENU_RUN_MIN 33
#define MENU_SLEEP_MIN 34
#define MENU_SLEEP_HOUR 35
#define MENU_SLEEP_DAY 36


enum class Fonts : uint8_t {
  Adafruit5x7     = 0,
  Arial14         = 1,
  Arial_bold_14   = 2,
  Corsiva_12      = 3,
  Verdana12       = 4
};

#define MENU_TIMEOUT 15000 // in milliseconds

extern Fonts currentFont;

extern MD_Menu::userNavAction_t menuNavAction;
extern MD_Menu mainMenu;

extern MD_Menu::value_t menuValueBuffer;  // Global variable to pass around menu callback values

MD_Menu::value_t *menuSetFont(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMinimumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMaximumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMinimumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMaximumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSaveConfig(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuCenterServos(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuLaserBrightness(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuShowNextRun(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuRestartSleepTimer(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetRunDuration(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetRunInterval(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);

void initMenu();
void displaySetFont(Fonts font);
void displayShowCurrentText();
void displayToast(const __FlashStringHelper* messageInProgmem, unsigned short duration, bool fontSize2x = false);
void displayToast(const char* message, unsigned short duration, bool fontSize2x = false);
bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg = nullptr);
MD_Menu::userNavAction_t navigateMenu();

