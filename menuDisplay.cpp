#include "menu.h"

SSD1306AsciiWire display;

char textLine1[25];
char textLine2[25];

void showSplashScreen() {
  const char* splash = "GATOINO";
                          // G  A   T   O   I   N   O
  const uint8_t spacing[] = {0, 20, 38, 56, 78, 88, 108};
  display.set2X();
  display.setRow(2);
  for(uint8_t i = 0; i < 7; i++) {
    display.setCol(spacing[i]);
    display.print(splash[i]);
    delay(250);
  }
  delay(2500);
  display.clear();
  display.set1X();
}

bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg)
{
  bool success = true;

  switch (action)
  {
  case MD_Menu::DISP_INIT:
    Wire.begin();
    Wire.setClock(400000L);

    display.begin(&Adafruit128x64, SCREEN_ADDRESS);
    display.setFont(Arial_bold_14);
    display.clear();
    display.displayRemap(false); // Rotation
    showSplashScreen();
    break;

  case MD_Menu::DISP_CLEAR:
    display.clear();
    break;

  case MD_Menu::DISP_L0:
    strcpy(textLine1, msg); 
    display.setRow(0);
    display.setCol(0);
    display.print(textLine1);
    display.clearToEOL();
    break;

  case MD_Menu::DISP_L1:
    strcpy(textLine2, msg);
    display.setRow(4);
    display.setCol(0);
    display.print(textLine2);
    display.clearToEOL();  
    break;
  }
}

void displayShowCurrentText() {
  displayMenu(MD_Menu::DISP_CLEAR);
  displayMenu(MD_Menu::DISP_L0, textLine1);
  displayMenu(MD_Menu::DISP_L1, textLine2);
}

void waitForButton(unsigned short duration) {
  uint32_t waitUntil = millis() + duration;

  while(millis() < waitUntil)
  {
    tickButtons();
    if(menuNavAction != MD_Menu::NAV_NULL) {
      menuNavAction = MD_Menu::NAV_NULL;
      break;
    }
  }
}

void displayToast(const __FlashStringHelper* messageInProgmem, unsigned short duration, bool fontSize2x = false) {
  char message[strlen_P((const char*)messageInProgmem) + 1]; // Allocate a char array on the stack
  strcpy_P(message, (const char*)messageInProgmem); // Copy the characters from the __FlashStringHelper object into the char array

  if(fontSize2x) display.set2X();
  display.setRow(3);
  display.setCol(0);
  display.print(message);

  waitForButton(duration);
  
  display.set1X();
}

void displayToast(const char* message, unsigned short duration, bool fontSize2x = false) {
  if(fontSize2x) display.set2X();
  display.setRow(3);
  display.setCol(0);
  display.print(message);
  
  waitForButton(duration);

  display.set1X();
}