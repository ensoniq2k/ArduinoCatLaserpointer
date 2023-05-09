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
#define LASER_PIN 3

#define MIDPOINT(min, max) (min + max) / 2
#define SECONDS_TO_MILLIS(sec) (static_cast<unsigned long>(sec) * 1000UL)
#define MINUTES_TO_MILLIS(min) (static_cast<unsigned long>(min) * 60000UL)
#define MINUTES_PER_DAY 1440
#define MINUTES_PER_HOUR 60
#define SECONDS_PER_MINUTE 60
#define MILLIS_TO_SECONDS(ms) (ms / 1000)
#define MILLIS_TO_MINUTES(ms) (ms / 60000)

extern OneButton ButtonLeft;
extern OneButton ButtonRight;
extern OneButton ButtonEnter;
extern OneButton ButtonEscape;

extern Servo xAxis;
extern Servo yAxis;

extern uint8_t LASER_BRIGHTNESS;

// X/Y Boundaries for the laser to stay inside while moving
extern uint8_t X_MIN;
extern uint8_t X_MAX;
extern uint8_t Y_MIN;
extern uint8_t Y_MAX;

extern uint16_t RUNTIME_SECONDS;    // Time for each "round" until device goes to sleep
extern uint16_t SLEEPTIME_MINUTES;  // Time for how long the device sleeps until it reactivates itself

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
void updateSettings();
void restartSleepTimer();