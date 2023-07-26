#pragma once

#include <Arduino.h>
#include <MD_Menu.h>

#include "menuTranslation.h"
#include "ArduinoCatLaserpointer.h"

#define SCREEN_ADDRESS 0x3C  /// See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

#define MENU_NEXT_RUN 10
#define MENU_RESTART_SLEEP 11
#define MENU_CENTER_SERVOS 14

#define MENU_TIMER_SETTINGS 20
#define MENU_TIMER_ON_OFF 21
#define MENU_TIMER_RUN 22
#define MENU_TIMER_SLEEP 23

#define MENU_LASER_SETTINGS 30
#define MENU_LASER_BRIGHTNESS 31
#define MENU_LASER_SIDE_LEFT 32
#define MENU_LASER_SIDE_RIGHT 33
#define MENU_LASER_FRONT_NEAR 34
#define MENU_LASER_FRONT_FAR 35
#define MENU_LASER_SPEED_MIN 36
#define MENU_LASER_SPEED_MAX 37
#define MENU_LASER_BLANKING_ON_OFF 38
#define MENU_LASER_SHOW_BOUNDARIES 39


#define LASER_SIDE_HARD_MIN 20
#define LASER_SIDE_HARD_MAX 170
#define LASER_FRONT_HARD_MIN 75
#define LASER_FRONT_HARD_MAX 175
#define MENU_LASER_LIMIT_STEP_FACTOR 5
#define MENU_SIDE_MAX (LASER_SIDE_HARD_MAX - LASER_SIDE_HARD_MIN) / MENU_LASER_LIMIT_STEP_FACTOR
#define MENU_FRONT_MAX (LASER_FRONT_HARD_MAX -LASER_FRONT_HARD_MIN) / MENU_LASER_LIMIT_STEP_FACTOR
#define MENU_TO_SIDE_VALUE(val) val * MENU_LASER_LIMIT_STEP_FACTOR + LASER_SIDE_HARD_MIN
#define MENU_TO_FRONT_VALUE(val) LASER_FRONT_HARD_MAX - (val * MENU_LASER_LIMIT_STEP_FACTOR)
#define MENU_FROM_SIDE_VALUE(val) (val - LASER_SIDE_HARD_MIN) / MENU_LASER_LIMIT_STEP_FACTOR
#define MENU_FROM_FRONT_VALUE(val) (LASER_FRONT_HARD_MAX - val) / MENU_LASER_LIMIT_STEP_FACTOR
#define MENU_SPEED_MAX 15
#define MENU_MIN_SPEED_VALUE 10 // Delay in ms between each step
#define MENU_SPEED_STEP_FACTOR 10 // Additional ms of delay for each -1 in speed setting
#define MENU_TO_SPEED_VALUE(val) ((MENU_SPEED_MAX - val) * MENU_SPEED_STEP_FACTOR) + MENU_MIN_SPEED_VALUE
#define MENU_FROM_SPEED_VALUE(val) MENU_SPEED_MAX - ((val - MENU_MIN_SPEED_VALUE) / MENU_SPEED_STEP_FACTOR)

#define MENU_TIMEOUT 60000 // in milliseconds

extern MD_Menu::userNavAction_t menuNavAction;
extern MD_Menu mainMenu;

extern MD_Menu::value_t menuValueBuffer;  // Global variable to pass around menu callback values

MD_Menu::value_t *menuSetFont(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMinimumSide(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMaximumSide(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMinimumFront(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetMaximumFront(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetSpeedMin(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetSpeedMax(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSaveConfig(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuCenterServos(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuLaserBrightness(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuShowNextRun(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuRestartSleepTimer(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuLaserShowBoundaries(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetLaserBlankout(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetTimerOnOff(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetRunDuration(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);
MD_Menu::value_t *menuSetRunInterval(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType);

void initMenu();
void displayShowCurrentText();
void displayToast(const __FlashStringHelper* messageInProgmem, unsigned short duration, bool fontSize2x = false);
void displayToast(const char* message, unsigned short duration, bool fontSize2x = false);
bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg = nullptr);
MD_Menu::userNavAction_t navigateMenu();

