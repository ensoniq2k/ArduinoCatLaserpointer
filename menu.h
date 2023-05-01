#pragma once

#include <Arduino.h>
#include <MD_Menu.h>

#include "ArduinoCatLaserpointer.h"

#define adafruit 0           /// Flexible use of Adafruit library vs. plain ASCII display
#define SCREEN_ADDRESS 0x3C  /// See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#if adafruit
  #include <Adafruit_SSD1306.h>

  #define SCREEN_WIDTH 128     // OLED display width, in pixels
  #define SCREEN_HEIGHT 32     // OLED display height, in pixels
  #define SCREEN_RESET -1      // Reset pin # (or -1 if sharing Arduino reset pin)
  #define RST_STATE 1          //1-> Reset=close switch, 0->Reset=open switch
#else
  #include <SSD1306Ascii.h>
  #include <SSD1306AsciiWire.h>
#endif

enum class Fonts : uint8_t {
  Adafruit5x7 = 0,
  Arial14 = 1,
  Arial_bold_14 = 2,
  Callibri14 = 3,
  Corsiva_12 = 4,
  TimesNewRoman13 = 5,
  Verdana12 = 6
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

void initMenu();
void displaySetFont(Fonts font);
void displayShowCurrentText();
void displayToast(const __FlashStringHelper* messageInProgmem, unsigned short duration, bool fontSize2x = false);
bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg = nullptr);
MD_Menu::userNavAction_t navigateMenu();

