#include "menu.h"

#if adafruit

  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);

  #define _SIZE 1    // OLED  scaling
  #define MENU_ROTATION 2
  #define LINE_HEIGHT (8 * _SIZE)  // Height of text line
  #define LINE1_X  0      // Line 1 coordinates for start of text
  #define LINE1_Y  0
  #define LINE2_X  0      // Line 2 coordinates for start of text
  #define LINE2_Y  (LINE_HEIGHT + 2)

  bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg)
  {
    bool success = true;

    switch (action)
    {
    case MD_Menu::DISP_INIT:
      // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
      success = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

      display.clearDisplay();
      display.setFontTextSize(_SIZE);
      display.setFontTextColor(SSD1306_WHITE);
      display.setFontRotation(MENU_ROTATION);
      display.display();
      break;

    case MD_Menu::DISP_CLEAR:
      display.clearDisplay();
      display.display();
      break;

    case MD_Menu::DISP_L0:
      // clear the line, put cursor in position and print the message
      display.setFontRotation(MENU_ROTATION);
      display.setFontTextSize(_SIZE);
      display.fillRect(LINE1_X, LINE1_Y, SCREEN_WIDTH-LINE1_X, LINE_HEIGHT, SSD1306_BLACK);
      display.setFontCursor(LINE1_X, LINE1_Y);
      display.print(msg);
      display.display();
      break;

    case MD_Menu::DISP_L1:
      // clear the line, put cursor in position and print the message
      display.setFontRotation(MENU_ROTATION);
      display.setFontTextSize(_SIZE);
      display.fillRect(LINE2_X, LINE2_Y, SCREEN_WIDTH - LINE2_X, LINE_HEIGHT, SSD1306_BLACK);
      display.setFontCursor(LINE2_X, LINE2_Y);
      display.print(msg);
      display.display();
      break;
    }

  return success;
}
#else
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
#endif