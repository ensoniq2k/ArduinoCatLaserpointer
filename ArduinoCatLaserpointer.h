#pragma once

#include "Servo.h"
#include "OneButton.h"
#include <AsyncTimer.h>
#include <EEPROM.h>

#include "menu.h"
#include "eepromStore.h"

#define BUTTON_LEFT 7
#define BUTTON_RIGHT 8
#define BUTTON_ENTER 9
#define BUTTON_ESCAPE 10

#define X_SERVO_PIN 5
#define Y_SERVO_PIN 6
#define LASER_PIN 4

extern OneButton ButtonLeft;
extern OneButton ButtonRight;
extern OneButton ButtonEnter;
extern OneButton ButtonEscape;

extern Servo xAxis;
extern Servo yAxis;

extern uint8_t X_MIN;
extern uint8_t X_MAX;
extern uint8_t Y_MIN;
extern uint8_t Y_MAX;

enum movementTypeEnum { 
  mtNone, 
  mtDiagonal,
  mtHorizontal,
  mtVertical,
  mtCircle
};

void startLaser();
void stopLaser();
void startRun();
void endRun();
void laufen();
void sweep(int AFrom, int ATo, int AHorizontalDirection, bool ABack);
void moveWithSimulatedShaking(int AFrom, int ATo, int AHorizontalDirection);
void sleep();
void randomMoves();
void moveAxis(int& interval, int& pos, int& tunraround, int axisMin, int axisMax);
void triggerLaser();
void laserMove();
void writeSettingsToEeprom();