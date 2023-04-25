#include <Servo.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "OneButton.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define RST_STATE 1 //1-> Reset=close switch, 0->Reset=open switch
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_LEFT 7
#define BUTTON_RIGHT 8
#define BUTTON_ENTER 9
#define BUTTON_ESCAPE 10

void laufen();
void sweep(int AFrom, int ATo, int AHorizontalDirection, bool ABack);
void moveWithSimulatedShaking (int AFrom, int ATo, int AHorizontalDirection);
void sleep();
void randomMoves ();
void moveAxis(int& interval, int& pos, int& tunraround, int axisMin, int axisMax);
void triggerLaser();

void ShowOnDisplay(String text);
void ShowLeftOnDisplay();
void ShowRightOnDisplay();
void ShowEnterOnDisplay();
void ShowEscapeOnDisplay();


OneButton ButtonLeft = OneButton(
  BUTTON_LEFT,    // Input pin for the button
  true,           // Button is active LOW
  true            // Enable internal pull-up resistor
);

OneButton ButtonRight = OneButton(
  BUTTON_RIGHT,  // Input pin for the button
  true,           // Button is active LOW
  true            // Enable internal pull-up resistor
);

OneButton ButtonEnter = OneButton(
  BUTTON_ENTER,      // Input pin for the button
  true,           // Button is active LOW
  true            // Enable internal pull-up resistor
);

OneButton ButtonEscape = OneButton(
  BUTTON_ESCAPE,      // Input pin for the button
  true,           // Button is active LOW
  true            // Enable internal pull-up resistor
);

const int X_MIN = 35;
const int X_MAX = 130;
const int Y_MIN = 90;
const int Y_MAX = 135;
const int MIN_DISTANCE = 15;

const int MIN_DELAY = 20; // Sets the minimum wait time in milliseconds between  each movement step
const int MAX_DELAY = 50; // Sets the maximum wait time in milliseconds between  each movement step

const uint32_t RUNTIME   = 180000;    // Time for each "round" until device goes to sleep
const uint32_t SLEEPTIME = 10800000;  // Time for how long the device sleeps until it reactivates itself

const int MIN_LASER_OFF_TICKS = 5;
const int MAX_LASER_OFF_TICKS = 100;

const int MIN_AXIS_MOVE_DECISSION_TICKS = 20;
const int MAX_AXIS_MOVE_DECISSION_TICKS = 100;

const int X_SERVO_PIN = 5;
const int Y_SERVO_PIN = 6;
const int LASER_PIN   = 4;
const int BUTTON_PIN  = 2;

enum axisToMoveEnum {none, xAndY, xOnly, yOnly};

int xInterval = 1;
int yInterval = 1;

int xTurnaround = X_MAX;
int yTurnaround = Y_MAX;

int randomDelay = 5;
int xPos = 90;
int yPos = 90;

int laserOffDuration = 0;
int laserOffTicks = 0;

axisToMoveEnum axisToMove;

uint32_t stopTime;
uint32_t startTime;

bool isRunning = false;

Servo xAxis;
Servo yAxis;

void setup() {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);    //Start display
  delay(50);                                              //Delay to make sure display is ready
  display.setTextColor(SSD1306_WHITE);                    //Set text color
  display.setRotation(2);                                         

  ShowOnDisplay("Running...");

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  pinMode(BUTTON_ESCAPE, INPUT_PULLUP);

  pinMode(LASER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  ButtonLeft.attachClick(ShowLeftOnDisplay);
  ButtonRight.attachClick(ShowRightOnDisplay);
  ButtonEnter.attachClick(ShowEnterOnDisplay);
  ButtonEscape.attachClick(ShowEscapeOnDisplay);

  stopTime = millis();
  isRunning = true;
}

void loop() {
  int axisToMoveDiceRoll;
  int axisToMoveTicks = 0;
  int runningTicks = 0;
  
  ButtonLeft.tick();
  ButtonRight.tick();
  ButtonEnter.tick();
  ButtonEscape.tick();

  if(Serial.available()){
    display.clearDisplay();
    display.setCursor(0 , 0);
    display.setTextSize(1);
    String input = Serial.readStringUntil('\n');

    display.println(input);

    display.display();
  }

  /*
  while (digitalRead(BUTTON_PIN) == HIGH) {
    isRunning = true;
    digitalWrite(LASER_PIN, HIGH);
  }

  if (millis() - stopTime > SLEEPTIME) {
    isRunning = true;
  }

  if (isRunning) {
    xAxis.attach(X_SERVO_PIN);
    yAxis.attach(Y_SERVO_PIN);
    digitalWrite(LASER_PIN, HIGH);
    
    startTime = millis();
  }

  while (isRunning) {
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

    // Stop if button pressed
    while (digitalRead(BUTTON_PIN) == HIGH) {
      isRunning = false;
      stopTime = millis();
    }

    // Stop if time is over
    if (!(millis() - startTime < RUNTIME)) {
      isRunning = false; 
      stopTime = millis();
    }
  }
    
  digitalWrite(LASER_PIN, LOW);
  xAxis.detach();
  yAxis.detach();
  //delay(SLEEPTIME);
  
  delay(100);
  */
}

void sleep (uint32_t sleepMs) {
  uint32_t startMs = millis();
  while ((millis() - startMs) < sleepMs)
  {
    delay(10);
  }
}

void randomMoves () {
  if (axisToMove == xAndY) {
    moveAxis(xInterval, xPos, xTurnaround, X_MIN, X_MAX);
    moveAxis(yInterval, yPos, yTurnaround, Y_MIN, Y_MAX); 
  }
  else if (axisToMove == xOnly) 
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
    }
    else {
      interval = -1;
      tunraround = random(axisMin, yPos - MIN_DISTANCE);
    }
  }
  else {
    if (pos > tunraround) {
      pos--;
    }
    else {
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
  }
  else {
    if (laserOffTicks <= laserOffDuration)
      laserOffTicks++;
    else {
      laserOffTicks = 0;
      digitalWrite(LASER_PIN, HIGH);  
    }
  }
}



void ShowOnDisplay(String text) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(text);
  display.display();
}

void ShowLeftOnDisplay() {
  ShowOnDisplay("Left"); 
}

void ShowRightOnDisplay() {
  ShowOnDisplay("Right");
}

void ShowEnterOnDisplay() {ShowOnDisplay("Enter");}
void ShowEscapeOnDisplay() {ShowOnDisplay("Escape");}









void moveWithSimulatedShaking (int APos, int AHorizontalDirection) {
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
  }
  else {
    for (int LPos = AFrom; LPos >= ATo; LPos--)
      moveWithSimulatedShaking(LPos, AHorizontalDirection);
  }
}

// Vom Flur ins Schlafzimmer, dann Richtung Küche und wieder von vorne
void laufen() {
  sweep(75, 120, 80, false);
  sweep(120, 75, 80, true);
  sweep(75, 40, 110, true);
  sweep(40, 75, 110, false);
}

void kreis() {
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
