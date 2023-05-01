#include "menu.h"

SSD1306AsciiWire display;

Fonts currentFont = Fonts::Adafruit5x7;

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

    display.begin(&Adafruit128x32, SCREEN_ADDRESS);
    displaySetFont(currentFont);
    display.clear();
    display.displayRemap(true); // Rotation
    break;

  case MD_Menu::DISP_CLEAR:
    display.clear();
    break;

  case MD_Menu::DISP_L0:
    strcpy(textLine1, msg); 
    display.clear();
    display.println(textLine1);
    display.println(textLine2);
    break;

  case MD_Menu::DISP_L1:      
    strcpy(textLine2, msg); 
    display.clear();
    display.println(textLine1);
    display.println(textLine2);
    break;
  }
}

void displaySetFont(Fonts font) {
  switch(font) {
    case Fonts::Adafruit5x7:
      display.setFont(Adafruit5x7);
      break;
    case Fonts::Arial14:
      display.setFont(Arial14);
      break;
    case Fonts::Arial_bold_14:
      display.setFont(Arial_bold_14);
      break;
    case Fonts::Callibri14:
      display.setFont(Callibri14);
      break;
    case Fonts::Corsiva_12:
      display.setFont(Corsiva_12);
      break;
    case Fonts::TimesNewRoman13:
      display.setFont(TimesNewRoman13);
      break;
    case Fonts::Verdana12:
      display.setFont(Verdana12);
      break;
    default:
      display.setFont(Adafruit5x7);
      break;
  }
}

void displayShowCurrentText() {
  display.clear();
  display.println(textLine1);
  display.println(textLine2);
}

void displayToast(const __FlashStringHelper* messageInProgmem, unsigned short duration, bool fontSize2x = false) {
  char message[strlen_P((const char*)messageInProgmem) + 1]; // Allocate a char array on the stack
  strcpy_P(message, (const char*)messageInProgmem); // Copy the characters from the __FlashStringHelper object into the char array

  display.clear();
  if(fontSize2x) display.set2X();
  display.println(message);
  delay(duration);
  display.set1X();
}