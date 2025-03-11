#include "menu.h"

#ifdef ALTERNATE_DISPLAY_LIB

#include <Tiny4kOLED.h>

const DCfont *FONT1x = FONT6X8;
const DCfont *FONT2x = FONT8X16;

char textLine1[25];
char textLine2[25];

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

void showSplashScreen() {
  const char* splash = "GATOINO";
                          // G  A   T   O   I   N   O
  const uint8_t spacing[] = {0, 20, 38, 56, 78, 98, 118};

  for(uint8_t i = 0; i < 7; i++) {
    oled.setCursor(spacing[i] +2, 1);
    oled.print(splash[i]);
    delay(150);
  }
  waitForButton(2500);
}

bool displayMenu(MD_Menu::userDisplayAction_t action, char *msg)
{
  bool success = true;

  switch (action)
  {
  case MD_Menu::DISP_INIT:
    oled.begin();
    oled.setFont(FONT2x);
    oled.clear();
    oled.on();
    showSplashScreen();
    oled.clear();
    break;

  case MD_Menu::DISP_CLEAR:
    oled.clear();
    break;

  case MD_Menu::DISP_L0:
    strcpy(textLine1, msg); 
    
    oled.switchRenderFrame();
    oled.clear();
    oled.setCursor(0, 0);
    oled.print(textLine1);
    oled.setCursor(0, 2);
    oled.print(textLine2);
    oled.switchDisplayFrame();
    break;

  case MD_Menu::DISP_L1:
    strcpy(textLine2, msg);

    oled.switchRenderFrame();
    oled.clear();
    oled.setCursor(0, 0);
    oled.print(textLine1);
    oled.setCursor(0, 2);
    oled.print(textLine2);
    oled.switchDisplayFrame();
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

  // Tiny4k doesn't have a 2x size method
  if(fontSize2x) ; 
  
  oled.setCursor(0, 2);
  oled.print(message);

  waitForButton(duration);
  
  //display.set1X();
}

void displayToast(const char* message, unsigned short duration, bool fontSize2x = false) {
  
  // Tiny4k doesn't have a 2x size method
  if(fontSize2x) ;

  oled.setCursor(0, 2);
  oled.print(message);
  
  waitForButton(duration);

  //display.set1X();
}

#endif