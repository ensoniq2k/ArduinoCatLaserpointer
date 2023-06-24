#include "menu.h"

SSD1306AsciiWire display;

char textLine1[25];
char textLine2[25];

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

void displayToast(const __FlashStringHelper* messageInProgmem, unsigned short duration, bool fontSize2x = false) {
  char message[strlen_P((const char*)messageInProgmem) + 1]; // Allocate a char array on the stack
  strcpy_P(message, (const char*)messageInProgmem); // Copy the characters from the __FlashStringHelper object into the char array

  if(fontSize2x) display.set2X();
  display.setRow(3);
  display.setCol(0);
  display.print(message);
  delay(duration);
  display.set1X();
}

void displayToast(const char* message, unsigned short duration, bool fontSize2x = false) {
  if(fontSize2x) display.set2X();
  display.setRow(3);
  display.setCol(0);
  display.print(message);
  delay(duration);
  display.set1X();
}