#include "ArduinoCatLaserpointer.h"

MD_Menu::userNavAction_t menuNavAction = MD_Menu::NAV_NULL;

OneButton ButtonLeft = OneButton(BUTTON_LEFT, true, true);
OneButton ButtonRight = OneButton(BUTTON_RIGHT, true, true);
OneButton ButtonEnter = OneButton(BUTTON_ENTER, true, true);
OneButton ButtonEscape = OneButton(BUTTON_ESCAPE, true, true);

uint8_t LASER_BRIGHTNESS = 100;

uint8_t SIDE_MIN = 60;
uint8_t SIDE_MAX = 120;
uint8_t FRONT_MIN = 60;
uint8_t FRONT_MAX = 120;

uint16_t RUNTIME_SECONDS = 180;
uint16_t SLEEPTIME_MINUTES = 180;

// Minimum and maximum wait time in milliseconds between each servo movement step
uint8_t MIN_STEP_DELAY = 20;  
uint8_t MAX_STEP_DELAY = 50;

// Minimum and maximum time the laser can be turned off to confuse the cat
bool laserBlankingEnabled = true;
const uint8_t MIN_LASER_OFF_TICKS = 5;    
const uint8_t MAX_LASER_OFF_TICKS = 50;

bool wakeUpTimerActive = false;

bool sideInvertDirection = false;
bool frontInvertDirection = false;

uint8_t sideTurnaround = SIDE_MAX;
uint8_t frontTurnaround = FRONT_MAX;

uint8_t sidePos = MIDPOINT(SIDE_MIN, SIDE_MAX);
uint8_t frontPos = MIDPOINT(FRONT_MIN, FRONT_MAX);

uint8_t laserOffDuration = 0;
uint8_t laserOffTicks = 0;

uint8_t consecuitiveSmallMovementsSide = 0;
uint8_t consecuitiveSmallMovementsFront = 0;

movementTypeEnum movementType;

Servo sideAxis;
Servo frontAxis;

AsyncTimer timer;

unsigned short laserWakeUpTimerId = 0;
unsigned short laserRuntimeUpTimerId = 0;
unsigned short laserMoveGovernorId = 0;
unsigned short laserMoveSideId = 0;
unsigned short laserMoveFrontId = 0;
auto laserWakeUpLambda = [] { startRun(); };
auto laserRuntimeUpLambda = [] { endRun(); };
auto laserMoveGovernorLambda = [] { if(!mainMenu.isInMenu()) laserMove(); };
auto laserMoveSideLambda = [&]() { moveAxis(sideInvertDirection, sidePos, sideTurnaround, SIDE_MIN, SIDE_MAX, consecuitiveSmallMovementsSide, sideAxis); };
auto laserMoveFrontLambda = [&]() { moveAxis(frontInvertDirection, frontPos, frontTurnaround, FRONT_MIN, FRONT_MAX, consecuitiveSmallMovementsFront, frontAxis); };


void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  pinMode(BUTTON_ESCAPE, INPUT_PULLUP);

  pinMode(X_SERVO_MOSFET_PIN, OUTPUT);
  pinMode(Y_SERVO_MOSFET_PIN, OUTPUT);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(X_SERVO_MOSFET_PIN, LOW);
  digitalWrite(Y_SERVO_MOSFET_PIN, LOW);
  analogWrite(LASER_PIN, LOW);

  ButtonLeft.attachClick([] { menuNavAction = MD_Menu::NAV_DEC; });
  ButtonRight.attachClick([] { menuNavAction = MD_Menu::NAV_INC; });
  ButtonEnter.attachClick([] { menuNavAction = MD_Menu::NAV_SEL; });
  ButtonEscape.attachClick([] { menuNavAction = MD_Menu::NAV_ESC; });

  restoreSettingsFromEeprom();

  updateSettings();
  restartSleepTimer();
  initMenu(); 
}

void loop() {
  tickButtons();

  // Handle all timer events
  timer.handle();

  checkUserInput();

  // Check menu conditions only at the end so that we can use 
  // menuNavAction for starting and stopping manually
  if (!mainMenu.isInMenu())
  {
    if (navigateMenu() == MD_Menu::NAV_SEL) {
      endRun();
      mainMenu.runMenu(true);
    }
  }

  mainMenu.runMenu();   // Must run in loop in order to work
}

void tickButtons() {
  ButtonLeft.tick();
  ButtonRight.tick();
  ButtonEnter.tick();
  ButtonEscape.tick();
}

void checkUserInput() {

  // Start if button pressed
  if (menuNavAction == MD_Menu::NAV_ESC && laserMoveGovernorId == 0 && !mainMenu.isInMenu()) {
    menuNavAction = MD_Menu::NAV_NULL;
    startRun();
  }

  // Stop if button pressed
  if (menuNavAction == MD_Menu::NAV_ESC && laserMoveGovernorId > 0 && !mainMenu.isInMenu()) {
    menuNavAction = MD_Menu::NAV_NULL;
    endRun();
  }
}

void startLaser() {
  sideAxis.attach(X_SERVO_PIN);
  frontAxis.attach(Y_SERVO_PIN);
  digitalWrite(X_SERVO_MOSFET_PIN, HIGH);
  delay(125); // Short delay to make sure we don't have a voltage drop crashing the cpu
  digitalWrite(Y_SERVO_MOSFET_PIN, HIGH);
  delay(125); // Short delay to make sure we don't have a voltage drop crashing the cpu
  analogWrite(LASER_PIN, LASER_BRIGHTNESS);
}

void stopLaser() {
  analogWrite(LASER_PIN, LOW);
  digitalWrite(X_SERVO_MOSFET_PIN, LOW);
  digitalWrite(Y_SERVO_MOSFET_PIN, LOW);
  sideAxis.detach();
  frontAxis.detach();
}

void startRun() {
  randomSeed(analogRead(A0));

  if(RUNTIME_SECONDS < 1) {
    return;
  }

  timer.reset(laserWakeUpTimerId);
  startLaser();

  laserRuntimeUpTimerId = timer.setTimeout(laserRuntimeUpLambda, SECONDS_TO_MILLIS(RUNTIME_SECONDS));
  laserMove();
}

void endRun() {
  timer.cancel(laserMoveSideId);
  laserMoveSideId = 0;
  timer.cancel(laserMoveFrontId);
  laserMoveFrontId = 0;
  timer.cancel(laserMoveGovernorId);
  laserMoveGovernorId = 0;
  timer.cancel(laserRuntimeUpTimerId);
  laserRuntimeUpTimerId = 0;

  stopLaser();
}

void laserMove() {
  timer.cancel(laserMoveSideId);
  laserMoveSideId = 0;
  timer.cancel(laserMoveFrontId);
  laserMoveFrontId = 0;

  laserMoveGovernorId = timer.setTimeout(laserMoveGovernorLambda, random(MIN_MOVE_MILLIS, MAX_MOVE_MILLIS));
  chooseNewRandomMovementPattern();
  randomMoves();
}

void randomMoves() {

  if (movementType == mtDiagonal) {
    laserMoveSideId = timer.setInterval(laserMoveSideLambda, random(MIN_STEP_DELAY, MAX_STEP_DELAY));
    laserMoveFrontId = timer.setInterval(laserMoveFrontLambda, random(MIN_STEP_DELAY, MAX_STEP_DELAY));
  } 
  else if (movementType == mtHorizontal) {
    laserMoveSideId = timer.setInterval(laserMoveSideLambda, random(MIN_STEP_DELAY, MAX_STEP_DELAY));
  }
  else if (movementType == mtVertical) {
    laserMoveFrontId = timer.setInterval(laserMoveFrontLambda, random(MIN_STEP_DELAY, MAX_STEP_DELAY));
  }
}

void chooseNewRandomMovementPattern() {
    int movementTypeDiceRoll = random(0, 1000); 
  
    if (movementTypeDiceRoll < 600)   
      movementType = mtDiagonal;
    else if (movementTypeDiceRoll < 750) 
      movementType = mtHorizontal;
    else if (movementTypeDiceRoll < 900)
      movementType = mtVertical;
    else
      movementType = mtNone;
}

// Moves the laser in straight lines
void moveAxis(bool& invertDirection, uint8_t& pos, uint8_t& turnaround, uint8_t axisMin, uint8_t axisMax, uint8_t& consecuitiveSmallMovements, Servo servoAxis) {
  
  if (invertDirection) {
    if (pos > turnaround && pos > axisMin) {
      pos--;
    } else {
      invertDirection = false;

      if(consecuitiveSmallMovements >= MAX_CONSECUITIVE_SMALL_MOVES) {
        turnaround = axisMax;
      }
      else
      {
        turnaround = random(pos + MIN_MOVE_DISTANCE, axisMax);
      }

      // Once in a while we force a large move
      if(turnaround < axisMax * SMALL_MOVE_FACTOR) {
        consecuitiveSmallMovements++;
      }
      else
      {
        consecuitiveSmallMovements = 0;
      }
    }
  }
  else {
    if (pos < turnaround && pos < axisMax) {
      pos++;
    } else {
      invertDirection = true;

      if(consecuitiveSmallMovements >= MAX_CONSECUITIVE_SMALL_MOVES) {
        turnaround = axisMin;
      }
      else
      {
        turnaround = random(axisMin, pos - MIN_MOVE_DISTANCE);
      }

      // Once in a while we force a large move
      if(turnaround * SMALL_MOVE_FACTOR > axisMin) {
        consecuitiveSmallMovements++;
      }
      else
      {
        consecuitiveSmallMovements = 0;
      }
    }
  }

  servoAxis.write(pos);
}

// Shuts off the laser for brief moments to confuse the cat
void triggerLaser() {
  if (laserBlankingEnabled && laserOffTicks == 0) {
    if (random(1, 1250) == 1) {
      analogWrite(LASER_PIN, LOW);
      laserOffDuration = random(MIN_LASER_OFF_TICKS, MAX_LASER_OFF_TICKS); 
      laserOffTicks = 1;
    }
  } else {
    if (laserOffTicks <= laserOffDuration)
      laserOffTicks++;
    else {
      laserOffTicks = 0;
      analogWrite(LASER_PIN, LASER_BRIGHTNESS);
    }
  }
}

void laserShowSquareBoundaries() {
  startLaser();

  delay(500);

  uint8_t side = SIDE_MIN;
  uint8_t front = FRONT_MIN;

  sideAxis.write(side);
  frontAxis.write(front);
  uint8_t delayMs = 50;

  for(side = SIDE_MIN; side < SIDE_MAX; side++) {
    sideAxis.write(side);
    frontAxis.write(front);
    delay(delayMs);
  }

  for(front = FRONT_MIN; front < FRONT_MAX; front++) {
    sideAxis.write(side);
    frontAxis.write(front);
    delay(delayMs);
  }

  for(side = SIDE_MAX; side > SIDE_MIN; side--) {
    sideAxis.write(side);
    frontAxis.write(front);
    delay(delayMs);
  }

  for(front = FRONT_MAX; front > FRONT_MIN; front--) {
    sideAxis.write(side);
    frontAxis.write(front);
    delay(delayMs);
  }

  delay(500);

  stopLaser();
}

void writeSettingsToEeprom() {
  EEPROM.begin();

    EEPROM.put(ADRESS_X_MIN, SIDE_MIN);
    EEPROM.put(ADRESS_X_MAX, SIDE_MAX);
    EEPROM.put(ADRESS_Y_MIN, FRONT_MIN);
    EEPROM.put(ADRESS_Y_MAX, FRONT_MAX);
    //EEPROM.put(ADRESS_FONT_TYPE, static_cast<uint8_t>(currentFont));
    EEPROM.put(ADRESS_RUNTIME, RUNTIME_SECONDS);
    EEPROM.put(ADRESS_SLEEPTIME, SLEEPTIME_MINUTES);
    EEPROM.put(ADRESS_LASER_BRIGHTNESS, LASER_BRIGHTNESS);
    EEPROM.put(ADRESS_WAKEUP_TIMER_ACTIVE, wakeUpTimerActive);
    EEPROM.put(ADRESS_LASER_BLANKING_ENABLED, laserBlankingEnabled);
    EEPROM.put(ADRESS_SPEED_MIN, MAX_STEP_DELAY);
    EEPROM.put(ADRESS_SPEED_MAX, MIN_STEP_DELAY);

  EEPROM.end();
}


void restoreSettingsFromEeprom() {
  // If a value is not yet stored they're max value so we ignore it
  EEPROM.begin();
  uint8_t readUInt8;
  uint16_t readUInt16;

  EEPROM.get(ADRESS_X_MIN, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) SIDE_MIN = readUInt8;

  EEPROM.get(ADRESS_X_MAX, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) SIDE_MAX = readUInt8;

  EEPROM.get(ADRESS_Y_MIN, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) FRONT_MIN = readUInt8;

  EEPROM.get(ADRESS_Y_MAX, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) FRONT_MAX = readUInt8;

  //EEPROM.get(ADRESS_FONT_TYPE, readUInt8);
  //if(readUInt8 != EMPTY_EEPROM_1BYTE) currentFont = static_cast<Fonts>(readUInt8);

  EEPROM.get(ADRESS_RUNTIME, readUInt16);
  if(readUInt16 != EMPTY_EEPROM_2BYTE) RUNTIME_SECONDS = readUInt16;

  EEPROM.get(ADRESS_SLEEPTIME, readUInt16);
  if(readUInt16 != EMPTY_EEPROM_2BYTE) SLEEPTIME_MINUTES = readUInt16;

  EEPROM.get(ADRESS_LASER_BRIGHTNESS, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) LASER_BRIGHTNESS = readUInt8;

  EEPROM.get(ADRESS_WAKEUP_TIMER_ACTIVE, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) wakeUpTimerActive = readUInt8;

  EEPROM.get(ADRESS_LASER_BLANKING_ENABLED, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) laserBlankingEnabled = readUInt8;

  EEPROM.get(ADRESS_SPEED_MIN, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) MAX_STEP_DELAY = readUInt8;

  EEPROM.get(ADRESS_SPEED_MAX, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) MIN_STEP_DELAY = readUInt8;

  EEPROM.end();
}

// Recalculation after reading from EEPROM or changing settings
void updateSettings() {
  sidePos = MIDPOINT(SIDE_MIN, SIDE_MAX);
  frontPos = MIDPOINT(FRONT_MIN, FRONT_MAX);

  sideTurnaround = SIDE_MAX;
  frontTurnaround = FRONT_MAX;

  sideInvertDirection = false;
  frontInvertDirection = false;
}

void restartSleepTimer() {
  timer.cancel(laserWakeUpTimerId);

  if(wakeUpTimerActive && SLEEPTIME_MINUTES > 0) {
    laserWakeUpTimerId = timer.setInterval(laserWakeUpLambda, MINUTES_TO_MILLIS(SLEEPTIME_MINUTES));
  }
}

uint32_t getNextRunSeconds() {
  if(laserWakeUpTimerId > 0) {
    return MILLIS_TO_SECONDS(timer.getRemaining(laserWakeUpTimerId));
  }
  return 0;
}
