#include "ArduinoCatLaserpointer.h"

MD_Menu::userNavAction_t menuNavAction = MD_Menu::NAV_NULL;

void laufen();
void sweep(int AFrom, int ATo, int AHorizontalDirection, bool ABack);
void moveWithSimulatedShaking(int AFrom, int ATo, int AHorizontalDirection);
void sleep();
void randomMoves();
void moveAxis(int& interval, int& pos, int& tunraround, int axisMin, int axisMax);
void triggerLaser();

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);

OneButton ButtonLeft = OneButton(BUTTON_LEFT, true, true);
OneButton ButtonRight = OneButton(BUTTON_RIGHT, true, true);
OneButton ButtonEnter = OneButton(BUTTON_ENTER, true, true);
OneButton ButtonEscape = OneButton(BUTTON_ESCAPE, true, true);

byte X_MIN = 35;
byte X_MAX = 130;
byte Y_MIN = 90;
byte Y_MAX = 135;
#define MIN_DISTANCE 15

const int MIN_DELAY = 20;  // Sets the minimum wait time in milliseconds between  each movement step
const int MAX_DELAY = 50;  // Sets the maximum wait time in milliseconds between  each movement step

const uint32_t RUNTIME = 180000;      // Time for each "round" until device goes to sleep
const uint32_t SLEEPTIME = 10800000;  // Time for how long the device sleeps until it reactivates itself

const int MIN_LASER_OFF_TICKS = 5;
const int MAX_LASER_OFF_TICKS = 100;

const int MIN_AXIS_MOVE_DECISSION_TICKS = 20;
const int MAX_AXIS_MOVE_DECISSION_TICKS = 100;

enum axisToMoveEnum { none,
                      xAndY,
                      xOnly,
                      yOnly };

int xInterval = 1;
int yInterval = 1;

int xTurnaround = X_MAX;
int yTurnaround = Y_MAX;

int randomDelay = 5;
int xPos = 90;
int yPos = 90;

int laserOffDuration = 0;
int laserOffTicks = 0;

int axisToMoveDiceRoll;
int axisToMoveTicks = 0;
int runningTicks = 0;

axisToMoveEnum axisToMove;

uint32_t stopTime;
uint32_t startTime;

bool isRunning = false;

Servo xAxis;
Servo yAxis;

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

  displayMenu(MD_Menu::DISP_INIT);
  MainMenu.begin();
  MainMenu.setMenuWrap(true);

  stopTime = millis();
}

void loop() {
  ButtonLeft.tick();
  ButtonRight.tick();
  ButtonEnter.tick();
  ButtonEscape.tick();

  bool formerRunState = isRunning;

  if(formerRunState != checkRunConditions())
  {
    Serial.println(F("Run condition changed"));

    if (isRunning) {
      startRun();
    }
    else {
      stopRun();
    }
  }

  if(isRunning) {
    laserLoop();
  }

  // Check menu conditions only at the end so that we can use 
  // menuNavAction for starting and stopping manually
  if (!MainMenu.isInMenu())
  {
    if (navigateMenu() == MD_Menu::NAV_SEL)
      MainMenu.runMenu(true);
  }

  MainMenu.runMenu();   // Must run in loop in order to work
}

  bool checkRunConditions() {

  // Start if button pressed
  if (!isRunning && !MainMenu.isInMenu() && menuNavAction == MD_Menu::NAV_ESC) {
    Serial.println(F("Button Pressed - starting"));
    menuNavAction = MD_Menu::NAV_NULL;
    isRunning = true;
  }

  // Stop if button pressed
  if (isRunning && !MainMenu.isInMenu() && menuNavAction == MD_Menu::NAV_ESC) {
    Serial.println(F("Button Pressed - stopping"));
    menuNavAction = MD_Menu::NAV_NULL;
    isRunning = false;
  }

  // Stop if time is over
  if (!(millis() - startTime < RUNTIME) && isRunning) {
    Serial.println(F("Time is up - stopping"));
    isRunning = false;
  }

  // Start if sleep time is over
  if (millis() - stopTime > SLEEPTIME && !isRunning) {
      Serial.println(F("Sleep is over - starting"));
    isRunning = true;
  }

  return isRunning;
}

void laserLoop() {
  if (runningTicks > axisToMoveTicks) {
    runningTicks = 0;
    axisToMoveDiceRoll = random(0, 1000); 
  
    if (axisToMoveDiceRoll < 600)   
      axisToMove = xAndY;
    else if (axisToMoveDiceRoll < 750) 
      axisToMove = xOnly;
    else if (axisToMoveDiceRoll < 900)
      axisToMove = yOnly;
    else
      axisToMove = none;
      
    axisToMoveTicks = random(MIN_AXIS_MOVE_DECISSION_TICKS, MAX_AXIS_MOVE_DECISSION_TICKS);
  }
  else
    runningTicks++;

  randomMoves();    

  randomDelay = random(MIN_DELAY, MAX_DELAY);
  sleep(randomDelay);
}

void startRun() {
  digitalWrite(LASER_PIN, HIGH);
  xAxis.attach(X_SERVO_PIN);
  yAxis.attach(Y_SERVO_PIN);
  startTime = millis();
}

void stopRun() {
  digitalWrite(LASER_PIN, LOW);
  xAxis.detach();
  yAxis.detach();
  stopTime = millis();
}

void sleep(uint32_t sleepMs) {
  uint32_t startMs = millis();
  while ((millis() - startMs) < sleepMs) {
    delay(10);
  }
}

void randomMoves() {
  if (axisToMove == xAndY) {
    moveAxis(xInterval, xPos, xTurnaround, X_MIN, X_MAX);
    moveAxis(yInterval, yPos, yTurnaround, Y_MIN, Y_MAX);
  } else if (axisToMove == xOnly)
    moveAxis(xInterval, xPos, xTurnaround, X_MIN, X_MAX);
  else if (axisToMove == yOnly)
    moveAxis(yInterval, yPos, yTurnaround, Y_MIN, Y_MAX);

  triggerLaser();

  xAxis.write(xPos);
  yAxis.write(yPos);
}




void moveAxis(int& interval, int& pos, int& tunraround, int axisMin, int axisMax) {
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

void moveWithSimulatedShaking(int APos, int AHorizontalDirection) {
  yAxis.write(APos);

  if (APos % 10 == 0) randomDelay = random(10, 50);

  xAxis.write(random(AHorizontalDirection - 5, AHorizontalDirection + 5));
  sleep(randomDelay);
}

void sweep(int AFrom, int ATo, int AHorizontalDirection, bool ABack) {
  xAxis.write(AHorizontalDirection);

  if (!ABack) {
    for (int LPos = AFrom; LPos <= ATo; LPos++)
      moveWithSimulatedShaking(LPos, AHorizontalDirection);
  } else {
    for (int LPos = AFrom; LPos >= ATo; LPos--)
      moveWithSimulatedShaking(LPos, AHorizontalDirection);
  }
}

void circle() {
  double winkel = 2 * PI / 90;
  int x, y;
  int radius = 10;


  for (int i = 0; i <= 90; i++) {
    x = cos(i * winkel) * radius + xPos;
    y = sin(i * winkel) * radius + yPos;

    xAxis.write(x);
    yAxis.write(y);
    sleep(2);
  }
}
