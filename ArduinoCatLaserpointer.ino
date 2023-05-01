#include "ArduinoCatLaserpointer.h"

MD_Menu::userNavAction_t menuNavAction = MD_Menu::NAV_NULL;

OneButton ButtonLeft = OneButton(BUTTON_LEFT, true, true);
OneButton ButtonRight = OneButton(BUTTON_RIGHT, true, true);
OneButton ButtonEnter = OneButton(BUTTON_ENTER, true, true);
OneButton ButtonEscape = OneButton(BUTTON_ESCAPE, true, true);

byte X_MIN = 35;
byte X_MAX = 130;
byte Y_MIN = 90;
byte Y_MAX = 135;
#define MIN_DISTANCE 15

// Minimum and maximum wait time in milliseconds between each servo movement step
const int MIN_DELAY = 20;  
const int MAX_DELAY = 50;

const uint32_t RUNTIME = 180000;      // Time for each "round" until device goes to sleep
const uint32_t SLEEPTIME = 10800000;  // Time for how long the device sleeps until it reactivates itself

// Minimum and maximum time the laser can be turned off to confuse the cat
const int MIN_LASER_OFF_TICKS = 5;    
const int MAX_LASER_OFF_TICKS = 100;

// 
const int MIN_AXIS_MOVE_DECISSION_TICKS = 20;
const int MAX_AXIS_MOVE_DECISSION_TICKS = 100;

int xInterval = 1;
int yInterval = 1;

int xTurnaround = X_MAX;
int yTurnaround = Y_MAX;

int xPos = 90;
int yPos = 90;
int circleStartAngle = 0;

int laserOffDuration = 0;
int laserOffTicks = 0;

int movementTypeTicks = 0;
int runningTicks = 0;

movementTypeEnum movementType;

bool isRunning = false;

Servo xAxis;
Servo yAxis;

AsyncTimer timer;

unsigned short laserMoveTimerId = 0;
unsigned short laserWakeUpTimerId = 0;
unsigned short laserRuntimeUpTimerId = 0;
auto laserWakeUpLambda = [] { startLaser(); };
auto laserRuntimeUpLambda = [] { stopLaser(); laserRuntimeUpTimerId = 0; };
auto laserMoveLambda = [] { laserMove(); };

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
  if (!MainMenu.isInMenu())
  {
    if (navigateMenu() == MD_Menu::NAV_SEL)
      MainMenu.runMenu(true);
  }

  MainMenu.runMenu();   // Must run in loop in order to work
}

void checkUserInput() {

  // Start if button pressed
  if (menuNavAction == MD_Menu::NAV_ESC && laserMoveTimerId == 0 && !MainMenu.isInMenu()) {
    Serial.println(F("Button Pressed - starting"));
    menuNavAction = MD_Menu::NAV_NULL;
    startLaser();
  }

  // Stop if button pressed
  if (menuNavAction == MD_Menu::NAV_ESC && laserMoveTimerId > 0 && !MainMenu.isInMenu()) {
    Serial.println(F("Button Pressed - stopping"));
    menuNavAction = MD_Menu::NAV_NULL;
    stopLaser();
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

void startLaser() {
  Serial.println(F("Starting Laser"));
  timer.reset(laserWakeUpTimerId);

  digitalWrite(LASER_PIN, HIGH);
  xAxis.attach(X_SERVO_PIN);
  yAxis.attach(Y_SERVO_PIN);
  
  laserMove();
  laserRuntimeUpTimerId = timer.setTimeout(laserRuntimeUpLambda, RUNTIME);
}

void stopLaser() {
  Serial.println(F("Stopping Laser"));

  timer.cancel(laserMoveTimerId);
  laserMoveTimerId = 0;

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
  else if ((movementType == mtCircle)) {
    //moveInEllipse(xPos, yPos);
    //chooseNewRandomMovementPattern();
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

    // if (movementTypeDiceRoll < 500)   
    //   movementType = mtDiagonal;
    // else if (movementTypeDiceRoll < 650) 
    //   movementType = mtHorizontal;
    // else if (movementTypeDiceRoll < 800)
    //   movementType = mtVertical;
    // else if (movementTypeDiceRoll < 900)
    //   movementType = mtCircle;
    // else
    //   movementType = mtNone;

    // How long the distance of ticks the movement should last
    movementTypeTicks = random(MIN_AXIS_MOVE_DECISSION_TICKS, MAX_AXIS_MOVE_DECISSION_TICKS);
}

// Moves the laser in straight lines
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



/* Attempt in letting the laser to a elipse around the center from the current point
void moveInEllipse(unsigned short startX, unsigned short startY) {
  Serial.println("Starting Ellipse at");
  Serial.print("X: ");
  Serial.print(startX);
  Serial.print(" Y: ");
  Serial.println(startY);

  // calculate center of the ellipse
  unsigned short centerX = (X_MAX - X_MIN) / 2 + X_MIN;
  unsigned short centerY = (Y_MAX - Y_MIN) / 2 + Y_MIN;

  Serial.print("Center X: ");
  Serial.println(centerX);
  Serial.print("Center Y: ");
  Serial.println(centerY);

  // calculate maximum radius based on X/Y bounds
  unsigned short maxRadiusX = startX > centerX ? (centerX - startX) *-1 : centerX - startX;
  unsigned short maxRadiusY = startY > centerY ? (centerY - startY) *-1 : centerY - startY;

  Serial.print("Max Radius X: ");
  Serial.println(maxRadiusX);
  Serial.print("Max Radius Y: ");
  Serial.println(maxRadiusY);

  double startAngle = atan2(startY - centerY, startX - centerX);
  if (startAngle < 0) {
    startAngle += 2 * PI;
  }
  
  double angle = 2 * PI / 90;
  unsigned short x, y;

  for (int i = 0; i <= 90; i++) {
    // calculate radius based on current angle
    unsigned short radiusX = (unsigned short) (maxRadiusX * abs(cos(i * angle)));
    unsigned short radiusY = (unsigned short) (maxRadiusY * abs(sin(i * angle)));

    Serial.print("Radius X: ");
    Serial.println(radiusX);
    Serial.print("Radius Y: ");
    Serial.println(radiusY);

    double currentAngle = startAngle + i * angle;
    x = centerX + cos(currentAngle) * radiusX;
    y = centerY + sin(currentAngle) * radiusY;

    xAxis.write(x);
    yAxis.write(y);
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" Y: ");
    Serial.println(y);
    delay(25);
  }
}
*/
