#pragma once

#include "Servo.h"
#include "OneButton.h"
#include <AsyncTimer.h>
#include <EEPROM.h>

#include "menu.h"
#include "eepromStore.h"


#define BUTTON_LEFT 5
#define BUTTON_RIGHT 6
#define BUTTON_ENTER 3
#define BUTTON_ESCAPE 4

#define X_SERVO_MOSFET_PIN 7
#define Y_SERVO_MOSFET_PIN 8
#define X_SERVO_PIN 9
#define Y_SERVO_PIN 10
#define LASER_PIN 11

#define MIDPOINT(min, max) (min + max) / 2
#define SECONDS_TO_MILLIS(sec) (static_cast<unsigned long>(sec) * 1000UL)
#define MINUTES_TO_MILLIS(min) (static_cast<unsigned long>(min) * 60000UL)
#define MILLIS_TO_SECONDS(ms) (ms / 1000)
#define MILLIS_TO_MINUTES(ms) (ms / 60000)
#define MINUTES_PER_DAY    1440
#define MINUTES_PER_HOUR   60
#define SECONDS_PER_DAY    86400
#define SECONDS_PER_HOUR   3600
#define SECONDS_PER_MINUTE 60

#define X_COMP_FACTOR 10
// Experimental trapezoid compensation
#define X_COMP(x, y) x //x + static_cast<int>(static_cast<double>(X_COMP_FACTOR / 100.0 * y))
#define Y_COMP(x, y) y

// How many steps a move consists of. 
// At boundaries the direction is inverted and movement continued
#define MIN_MOVE_DISTANCE 10
#define MAX_MOVE_DISTANCE 100

#define MIN_AREA_SIZE MIN_MOVE_DISTANCE * 2

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

extern uint8_t MIN_STEP_DELAY;  
extern uint8_t MAX_STEP_DELAY;

extern uint16_t RUNTIME_SECONDS;    // Time for each "round" until device goes to sleep
extern uint16_t SLEEPTIME_MINUTES;  // Time for how long the device sleeps until it reactivates itself

extern bool wakeUpTimerActive;
extern bool laserBlankingEnabled;

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
void sweep(int AFrom, int ATo, int AHorizontalDirection, bool ABack);
void moveWithSimulatedShaking(int AFrom, int ATo, int AHorizontalDirection);
void sleep();
void randomMoves();
void moveAxis(int& interval, int& pos, int& tunraround, int axisMin, int axisMax);
void triggerLaser();
void laserMove();
void laserShowSquareBoundaries();
void writeSettingsToEeprom();
void updateSettings();
void restartSleepTimer();
uint32_t getNextRunSeconds();