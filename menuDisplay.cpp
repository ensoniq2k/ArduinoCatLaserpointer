#include "menu.h"

#define FONT_SIZE 1    // OLED font scaling
#define MENU_ROTATION 2
#define LINE_HEIGHT (8 * FONT_SIZE)  // Height of text line
#define LINE1_X  0      // Line 1 coordinates for start of text
#define LINE1_Y  0
#define LINE2_X  0      // Line 2 coordinates for start of text
#define LINE2_Y  (LINE_HEIGHT + 2)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);

bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg)
{
  bool success = true;

  switch (action)
  {
  case MD_Menu::DISP_INIT:
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    success = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

    display.clearDisplay();
    display.setTextSize(FONT_SIZE);
    display.setTextColor(SSD1306_WHITE);
    display.setRotation(MENU_ROTATION);
    display.display();
    break;

  case MD_Menu::DISP_CLEAR:
    display.clearDisplay();
    display.display();
    break;

  case MD_Menu::DISP_L0:
    // clear the line, put cursor in position and print the message
    display.setRotation(MENU_ROTATION);
    display.setTextSize(FONT_SIZE);
    display.fillRect(LINE1_X, LINE1_Y, SCREEN_WIDTH-LINE1_X, LINE_HEIGHT, SSD1306_BLACK);
    display.setCursor(LINE1_X, LINE1_Y);
    display.print(msg);
    display.display();
    break;

  case MD_Menu::DISP_L1:
    // clear the line, put cursor in position and print the message
    display.setRotation(MENU_ROTATION);
    display.setTextSize(FONT_SIZE);
    display.fillRect(LINE2_X, LINE2_Y, SCREEN_WIDTH - LINE2_X, LINE_HEIGHT, SSD1306_BLACK);
    display.setCursor(LINE2_X, LINE2_Y);
    display.print(msg);
    display.display();
    break;
  }

  return success;
}