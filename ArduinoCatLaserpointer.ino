#include "ArduinoCatLaserpointer.h"

MD_Menu::userNavAction_t menuNavAction = MD_Menu::NAV_NULL;

OneButton ButtonLeft = OneButton(BUTTON_LEFT, true, true);
OneButton ButtonRight = OneButton(BUTTON_RIGHT, true, true);
OneButton ButtonEnter = OneButton(BUTTON_ENTER, true, true);
OneButton ButtonEscape = OneButton(BUTTON_ESCAPE, true, true);

uint8_t X_MIN = 50;
uint8_t X_MAX = 130;
uint8_t Y_MIN = 50;
uint8_t Y_MAX = 130;
#define MIN_DISTANCE 15

// Minimum and maximum wait time in milliseconds between each servo movement step
const unsigned short MIN_DELAY = 20;  
const unsigned short MAX_DELAY = 50;

const uint32_t RUNTIME = 180000;      // Time for each "round" until device goes to sleep
const uint32_t SLEEPTIME = 10800000;  // Time for how long the device sleeps until it reactivates itself

// Minimum and maximum time the laser can be turned off to confuse the cat
const uint8_t MIN_LASER_OFF_TICKS = 5;    
const uint8_t MAX_LASER_OFF_TICKS = 50;

// 
const uint8_t MIN_AXIS_MOVE_DECISSION_TICKS = 20;
const uint8_t MAX_AXIS_MOVE_DECISSION_TICKS = 100;

short xInterval = 1;
short yInterval = 1;

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
auto laserRuntimeUpLambda = [] { endRun(); laserRuntimeUpTimerId = 0; };
auto laserMoveLambda = [] { if(!mainMenu.isInMenu()) laserMove(); };

void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  pinMode(BUTTON_ESCAPE, INPUT_PULLUP);

  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);

  ButtonLeft.attachClick([] { menuNavAction = MD_Menu::NAV_DEC; });
  ButtonRight.attachClick([] { menuNavAction = MD_Menu::NAV_INC; });
  ButtonEnter.attachClick([] { menuNavAction = MD_Menu::NAV_SEL; });
  ButtonEscape.attachClick([] { menuNavAction = MD_Menu::NAV_ESC; });

  restoreSettingsFromEeprom();
  initMenu();

  laserWakeUpTimerId = timer.setInterval(laserWakeUpLambda, SLEEPTIME);
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
    if (navigateMenu() == MD_Menu::NAV_SEL)
      mainMenu.runMenu(true);
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

  // 
  unsigned short randomDelay = random(MIN_DELAY, MAX_DELAY);
  laserMoveTimerId = timer.setTimeout(laserMoveLambda, randomDelay);
}

void startRun() {
  timer.reset(laserWakeUpTimerId);

  startLaser();
  
  laserMove();
  laserRuntimeUpTimerId = timer.setTimeout(laserRuntimeUpLambda, RUNTIME);
}

void endRun() {
  timer.cancel(laserMoveTimerId);
  laserMoveTimerId = 0;

  stopLaser();
}

void startLaser() {
  digitalWrite(LASER_PIN, HIGH);
  xAxis.attach(X_SERVO_PIN);
  yAxis.attach(Y_SERVO_PIN);
}

void stopLaser() {
  digitalWrite(LASER_PIN, LOW);
  xAxis.detach();
  yAxis.detach();
}

void randomMoves() {
  if (movementType == mtDiagonal) {
    moveAxis(xInterval, xPos, xTurnaround, X_MIN, X_MAX);
    moveAxis(yInterval, yPos, yTurnaround, Y_MIN, Y_MAX);
  } 
  else if (movementType == mtHorizontal) {
    moveAxis(xInterval, xPos, xTurnaround, X_MIN, X_MAX);
  }
  else if (movementType == mtVertical) {
    moveAxis(yInterval, yPos, yTurnaround, Y_MIN, Y_MAX);
  }

  triggerLaser();

  xAxis.write(xPos);
  yAxis.write(yPos);
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
    movementTypeTicks = random(MIN_AXIS_MOVE_DECISSION_TICKS, MAX_AXIS_MOVE_DECISSION_TICKS);
}

// Moves the laser in straight lines
void moveAxis(short& interval, uint8_t& pos, uint8_t& tunraround, uint8_t axisMin, uint8_t axisMax) {
  if (interval == 1) {
    if (pos < tunraround) {
      pos++;
    } else {
      interval = -1;
      tunraround = random(axisMin, yPos - MIN_DISTANCE);
    }
  } else {
    if (pos > tunraround) {
      pos--;
    } else {
      interval = 1;
      tunraround = random(pos + MIN_DISTANCE, axisMax);
    }
  }
}

// Moves the laser in circles
void moveInCircle() {
  double angle = 2 * PI / 90;
  unsigned short x, y;
  unsigned short radius = 20;


  for (int i = 0; i <= 90; i++) {
    x = cos(i * angle) * radius + xPos;
    y = sin(i * angle) * radius + yPos;

    xAxis.write(x);
    yAxis.write(y);
    delay(20);
  }
}


// Shuts off the laser for brief moments to confuse the cat
void triggerLaser() {
  if (laserOffTicks == 0) {
    if (random(1, 1250) == 1) {
      digitalWrite(LASER_PIN, LOW);
      laserOffDuration = random(MIN_LASER_OFF_TICKS, MAX_LASER_OFF_TICKS);
      laserOffTicks = 1;
    }
  } else {
    if (laserOffTicks <= laserOffDuration)
      laserOffTicks++;
    else {
      laserOffTicks = 0;
      digitalWrite(LASER_PIN, HIGH);
    }
  }
}

void writeSettingsToEeprom() {
  EEPROM.begin();

    EEPROM.update(ADRESS_X_MIN, X_MIN);
    EEPROM.update(ADRESS_X_MAX, X_MAX);
    EEPROM.update(ADRESS_Y_MIN, Y_MIN);
    EEPROM.update(ADRESS_Y_MAX, Y_MAX);
    EEPROM.update(ADRESS_FONT_TYPE, static_cast<uint8_t>(currentFont));

  EEPROM.end();
}


void restoreSettingsFromEeprom() {
  // If a value is not yet stored they're max value so we ignore it
  EEPROM.begin();
  uint8_t readByte;

  readByte = EEPROM.read(ADRESS_X_MIN);
  if(readByte != 255) X_MIN = readByte;

  readByte = EEPROM.read(ADRESS_X_MAX);
  if(readByte != 255) X_MAX = readByte;

  readByte = EEPROM.read(ADRESS_Y_MIN);
  if(readByte != 255) Y_MIN = readByte;

  readByte = EEPROM.read(ADRESS_Y_MAX);
  if(readByte != 255) Y_MAX = readByte;

  readByte = EEPROM.read(ADRESS_FONT_TYPE);
  if(readByte != 255) currentFont = static_cast<Fonts>(readByte);

  EEPROM.end();

  updateDefaultValues();
}

// Recalculation after reading from EEPROM
void updateDefaultValues() {
  xPos = MIDPOINT(X_MIN, X_MAX);
  yPos = MIDPOINT(Y_MIN, Y_MAX);

  xTurnaround = X_MAX;
  yTurnaround = Y_MAX;
}