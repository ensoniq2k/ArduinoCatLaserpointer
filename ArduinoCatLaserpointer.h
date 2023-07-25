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

// How many steps a move consists of. 
// At boundaries the direction is inverted and movement continued
#define MIN_MOVE_DISTANCE 25
#define MAX_MOVE_DISTANCE 150

#define MIN_MOVE_MILLIS 500
#define MAX_MOVE_MILLIS 5000

#define MIN_AREA_SIZE MIN_MOVE_DISTANCE * 2

#define MAX_CONSECUITIVE_SMALL_MOVES 5
#define SMALL_MOVE_FACTOR 0.75

extern OneButton ButtonLeft;
extern OneButton ButtonRight;
extern OneButton ButtonEnter;
extern OneButton ButtonEscape;

extern Servo sideAxis;
extern Servo frontAxis;

extern uint8_t LASER_BRIGHTNESS;

// X/Y Boundaries for the laser to stay inside while moving
extern uint8_t SIDE_MIN;
extern uint8_t SIDE_MAX;
extern uint8_t FRONT_MIN;
extern uint8_t FRONT_MAX;

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
  mtVertical
};

void startLaser();
void stopLaser();
void startRun();
void endRun();
void sweep(int AFrom, int ATo, int AHorizontalDirection, bool ABack);
void moveWithSimulatedShaking(int AFrom, int ATo, int AHorizontalDirection);
void sleep();
void randomMoves();
void moveAxis(bool& invertDirection, uint8_t& pos, uint8_t& turnaround, uint8_t axisMin, uint8_t axisMax, uint8_t& consecuitiveSmallMovements, Servo servoAxis);
void triggerLaser();
void laserMove();
void laserShowSquareBoundaries();
void writeSettingsToEeprom();
void updateSettings();
void restartSleepTimer();
uint32_t getNextRunSeconds();