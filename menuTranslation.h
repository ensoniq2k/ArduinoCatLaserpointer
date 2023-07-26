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
  #define MENUSTR_FRONT_NEAR             "Limit near"
  #define MENUSTR_FRONT_FAR              "Limit far"
  #define MENUSTR_SPEED_MIN              "Speed min"
  #define MENUSTR_SPEED_MAX              "Speed max"
  #define MENUSTR_LASER_BLANKING         "Blanking"
  #define MENUSTR_LASER_SHOW_BOUNDARIES  "Show boundary"
  
  #define MENUSTR_TIMER_ON_OFF           "Timer On/Off"
  #define MENUSTR_RUN_TIMER              "Run duration"
  #define MENUSTR_SLEEP_TIMER            "Sleep duration"


  #define MENUSTR_TIMER_RESTARTED        "Timer\nrestarted!"
  #define MENUSTR_CENTERING_SERVOS       "Centering\nServos"
  #define MENUSTR_NO_TIMER_SET           "No timer set"
  #define MENUSTR_NEXT_RUN_IN            "Next run in\n%02u Days %02u:%02u:%02u"

#elif LANGUAGE == GERMAN

  #define MENUSTR_MAIN_MENU              "Hauptmenue"
  #define MENUSTR_LASER_SETTINGS         "Laser Einst."
  #define MENUSTR_TIMER_SETTINGS         "Timer Einst."

  #define MENUSTR_NEXT_RUN               "Naechtser Lauf"
  #define MENUSTR_RESTART_SLEEP          "Timer neustart"
  #define MENUSTR_CENTER_SERVOS          "Zentr. Servos"

  #define MENUSTR_BRIGHTNESS             "Helligkeit"
  #define MENUSTR_SIDE_LEFT              "Seite links"
  #define MENUSTR_SIDE_RIGHT             "Seite rechts"
  #define MENUSTR_FRONT_NEAR             "Front nah"
  #define MENUSTR_FRONT_FAR              "Front fern"
  #define MENUSTR_SPEED_MIN              "Geschw. min"
  #define MENUSTR_SPEED_MAX              "Geschw. max"
  #define MENUSTR_LASER_BLANKING         "Aussetzer"
  #define MENUSTR_LASER_SHOW_BOUNDARIES  "Zeige Areal"
  
  #define MENUSTR_TIMER_ON_OFF           "Timer An/Aus"
  #define MENUSTR_RUN_TIMER              "Laufzeit"
  #define MENUSTR_SLEEP_TIMER            "Schlafdauer"

  #define MENUSTR_TIMER_RESTARTED        "Timer neu\ngestartet"
  #define MENUSTR_CENTERING_SERVOS       "Zentriere\nServos"
  #define MENUSTR_NO_TIMER_SET           "Kein Timer aktiv"
  #define MENUSTR_NEXT_RUN_IN            "Naechster lauf in\n%02u Tage %02u:%02u:%02u"

#else
    #error "Unsupported menu language selected"
#endif