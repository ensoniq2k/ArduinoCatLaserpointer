#include "ArduinoCatLaserpointer.h"

MD_Menu::userNavAction_t menuNavAction = MD_Menu::NAV_NULL;

OneButton ButtonLeft = OneButton(BUTTON_LEFT, true, true);
OneButton ButtonRight = OneButton(BUTTON_RIGHT, true, true);
OneButton ButtonEnter = OneButton(BUTTON_ENTER, true, true);
OneButton ButtonEscape = OneButton(BUTTON_ESCAPE, true, true);

uint8_t LASER_BRIGHTNESS = 100;

uint8_t X_MIN = 60;
uint8_t X_MAX = 120;
uint8_t Y_MIN = 60;
uint8_t Y_MAX = 120;

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

bool xInvertDirection = false;
bool yInvertDirection = false;

uint8_t xTurnaround = X_MAX;
uint8_t yTurnaround = Y_MAX;

uint8_t xPos = MIDPOINT(X_MIN, X_MAX);
uint8_t yPos = MIDPOINT(Y_MIN, Y_MAX);

uint8_t laserOffDuration = 0;
uint8_t laserOffTicks = 0;

uint8_t  movementTypeTicks = 0;
uint8_t  runningTicks = 0;

movementTypeEnum movementType;

Servo xAxis;
Servo yAxis;

AsyncTimer timer;

unsigned short laserMoveTimerId = 0;
unsigned short laserWakeUpTimerId = 0;
unsigned short laserRuntimeUpTimerId = 0;
auto laserWakeUpLambda = [] { startRun(); };
auto laserRuntimeUpLambda = [] { endRun(); };
auto laserMoveLambda = [] { if(!mainMenu.isInMenu()) laserMove(); };

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
  ButtonLeft.tick();
  ButtonRight.tick();
  ButtonEnter.tick();
  ButtonEscape.tick();

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

void checkUserInput() {

  // Start if button pressed
  if (menuNavAction == MD_Menu::NAV_ESC && laserMoveTimerId == 0 && !mainMenu.isInMenu()) {
    menuNavAction = MD_Menu::NAV_NULL;
    startRun();
  }

  // Stop if button pressed
  if (menuNavAction == MD_Menu::NAV_ESC && laserMoveTimerId > 0 && !mainMenu.isInMenu()) {
    menuNavAction = MD_Menu::NAV_NULL;
    endRun();
  }
}

void laserMove() {
  if (runningTicks > movementTypeTicks) {
    chooseNewRandomMovementPattern();
  }
  else
    runningTicks++;

  randomMoves();

  unsigned short randomDelay = random(MIN_STEP_DELAY, MAX_STEP_DELAY);
  Serial.println(randomDelay);
  laserMoveTimerId = timer.setTimeout(laserMoveLambda, randomDelay);
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
  timer.cancel(laserMoveTimerId);
  laserMoveTimerId = 0;
  timer.cancel(laserRuntimeUpTimerId);
  laserRuntimeUpTimerId = 0;

  stopLaser();
}

void startLaser() {
  analogWrite(LASER_PIN, LASER_BRIGHTNESS);
  digitalWrite(X_SERVO_MOSFET_PIN, HIGH);
  digitalWrite(Y_SERVO_MOSFET_PIN, HIGH);
  xAxis.attach(X_SERVO_PIN);
  yAxis.attach(Y_SERVO_PIN);
}

void stopLaser() {
  analogWrite(LASER_PIN, LOW);
  digitalWrite(X_SERVO_MOSFET_PIN, LOW);
  digitalWrite(Y_SERVO_MOSFET_PIN, LOW);
  xAxis.detach();
  yAxis.detach();
}

void randomMoves() {
  if (movementType == mtDiagonal) {
    moveAxis(xInvertDirection, xPos, xTurnaround, X_MIN, X_MAX);
    moveAxis(yInvertDirection, yPos, yTurnaround, Y_MIN, Y_MAX);
  } 
  else if (movementType == mtHorizontal) {
    moveAxis(xInvertDirection, xPos, xTurnaround, X_MIN, X_MAX);
  }
  else if (movementType == mtVertical) {
    moveAxis(yInvertDirection, yPos, yTurnaround, Y_MIN, Y_MAX);
  }

  triggerLaser();

  xAxis.write(X_COMP(xPos, yPos));
  yAxis.write(Y_COMP(xPos, yPos));
}

void chooseNewRandomMovementPattern() {
    runningTicks = 0;
    int movementTypeDiceRoll = random(0, 1000); 
  
    if (movementTypeDiceRoll < 600)   
      movementType = mtDiagonal;
    else if (movementTypeDiceRoll < 750) 
      movementType = mtHorizontal;
    else if (movementTypeDiceRoll < 900)
      movementType = mtVertical;
    else
      movementType = mtNone;

    // How long the distance of ticks the movement should last
    movementTypeTicks = random(MIN_MOVE_DISTANCE, MAX_MOVE_DISTANCE);
}

// Moves the laser in straight lines
void moveAxis(bool& invertDirection, uint8_t& pos, uint8_t& turnaround, uint8_t axisMin, uint8_t axisMax) {
  // Serial.print("Invert: ");       Serial.print(invertDirection);
  // Serial.print(" Pos: ");          Serial.print(pos);
  // Serial.print(" Turnaround: ");   Serial.print(turnaround);
  // Serial.print(" Axis min: "); Serial.print(axisMin);
  // Serial.print(" Axis max: "); Serial.println(axisMax);

  if (invertDirection) {
    if (pos > turnaround && pos > axisMin) {
      pos--;
    } else {
      invertDirection = false;
      turnaround = random(pos + MIN_MOVE_DISTANCE, axisMax);
    }
  }
  else {
    if (pos < turnaround && pos < axisMax) {
      pos++;
    } else {
      invertDirection = true;
      turnaround = random(axisMin, pos - MIN_MOVE_DISTANCE);
    }
  }
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

  uint8_t x = X_MIN;
  uint8_t y = Y_MIN;

  xAxis.write(X_COMP(x, y));
  yAxis.write(Y_COMP(x, y));
  uint8_t delayMs = 50;

  for(x = X_MIN; x < X_MAX; x++) {
    xAxis.write(X_COMP(x, y));
    yAxis.write(Y_COMP(x, y));
    delay(delayMs);
  }

  for(y = Y_MIN; y < Y_MAX; y++) {
    xAxis.write(X_COMP(x, y));
    yAxis.write(Y_COMP(x, y));
    delay(delayMs);
  }

  for(x = X_MAX; x > X_MIN; x--) {
    xAxis.write(X_COMP(x, y));
    yAxis.write(Y_COMP(x, y));
    delay(delayMs);
  }

  for(y = Y_MAX; y > Y_MIN; y--) {
    xAxis.write(X_COMP(x, y));
    yAxis.write(Y_COMP(x, y));
    delay(delayMs);
  }

  stopLaser();
}

void writeSettingsToEeprom() {
  EEPROM.begin();

    EEPROM.put(ADRESS_X_MIN, X_MIN);
    EEPROM.put(ADRESS_X_MAX, X_MAX);
    EEPROM.put(ADRESS_Y_MIN, Y_MIN);
    EEPROM.put(ADRESS_Y_MAX, Y_MAX);
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
  if(readUInt8 != EMPTY_EEPROM_1BYTE) X_MIN = readUInt8;

  EEPROM.get(ADRESS_X_MAX, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) X_MAX = readUInt8;

  EEPROM.get(ADRESS_Y_MIN, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) Y_MIN = readUInt8;

  EEPROM.get(ADRESS_Y_MAX, readUInt8);
  if(readUInt8 != EMPTY_EEPROM_1BYTE) Y_MAX = readUInt8;

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
  xPos = MIDPOINT(X_MIN, X_MAX);
  yPos = MIDPOINT(Y_MIN, Y_MAX);

  xTurnaround = X_MAX;
  yTurnaround = Y_MAX;

  xInvertDirection = false;
  yInvertDirection = false;
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
