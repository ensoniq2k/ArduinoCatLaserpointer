#pragma once

#define ENGLISH 0
#define GERMAN 1

#define LANGUAGE ENGLISH

#if LANGUAGE == ENGLISH

  #define MENUSTR_MAIN_MENU              "Main Menu"
  #define MENUSTR_TIMER_SETTINGS         "Timer Settings"
  #define MENUSTR_LASER_SETTINGS         "Laser settings"

  #define MENUSTR_NEXT_RUN               "Next run?"
  #define MENUSTR_RESTART_SLEEP          "Restart sleep"
  #define MENUSTR_CENTER_SERVOS          "Center Servos"

  #define MENUSTR_BRIGHTNESS             "Brightness"
  #define MENUSTR_SIDE_LEFT              "Limit left"
  #define MENUSTR_SIDE_RIGHT             "Limit right"
  #define MENUSTR_FRONT_FAR              "Limit far"
  #define MENUSTR_FRONT_NEAR             "Limit near"
  #define MENUSTR_SPEED_MIN              "Speed min"
  #define MENUSTR_SPEED_MAX              "Speed max"
  #define MENUSTR_LASER_BLANKING         "Blanking"
  #define MENUSTR_LASER_SHOW_BOUNDARIES  "Show boundary"
  
  #define MENUSTR_TIMER_ON_OFF           "Timer On/Off"
  #define MENUSTR_RUN_SECONDS            "Run seconds"
  #define MENUSTR_RUN_MINUTES            "Run minutes"
  #define MENUSTR_SLEEP_MINUTES          "Sleep minutes"
  #define MENUSTR_SLEEP_HOURS            "Sleep hours"
  #define MENUSTR_SLEEP_DAYS             "Sleep days"

  #define MENUSTR_TIMER_ON               "Timer on?"

#elif LANGUAGE == GERMAN

  #define MENUSTR_MAIN_MENU              "Main Menu"
  #define MENUSTR_LASER_SETTINGS         "Laser settings"
  #define MENUSTR_TIMER_SETTINGS         "Timer Settings"

  #define MENUSTR_CENTER_SERVOS          "Center Servos"
  #define MENUSTR_LASER_BRIGHTNESS       "Laser Brightness"
  #define MENUSTR_SIDE_LEFT              "Side left"
  #define MENUSTR_SIDE_RIGHT             "Side right"
  #define MENUSTR_FRONT_FAR              "Front far"
  #define MENUSTR_FRONT_NEAR             "Front near"
  #define MENUSTR_LASER_SHOW_BOUNDARIES  "Show boundaries"
  
  #define MENUSTR_NEXT_RUN               "Next run?"
  #define MENUSTR_RESTART_SLEEP          "Restart sleep"
  #define MENUSTR_RUN_SECONDS            "Run seconds"
  #define MENUSTR_RUN_MINUTES            "Run minutes"
  #define MENUSTR_SLEEP_MINUTES          "Sleep minutes"
  #define MENUSTR_SLEEP_HOURS            "Sleep hours"
  #define MENUSTR_SLEEP_DAYS             "Sleep days"

#else
    #error "Unsupported menu language selected"
#endif