#include "menu.h"

MD_Menu::value_t menuValueBuffer;

MD_Menu::value_t *menuRestartSleepTimer(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      return nullptr; // nullptr means no confirmation required
      break;

    case MD_Menu::REQ_SET:
      restartSleepTimer();
      displayToast(F("Timer restarted!"), 1000, false);
      break;    
  }
}

MD_Menu::value_t *menuSetRunDuration(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
  case MD_Menu::REQ_GET:
    if(id == MENU_TIMER_RUN_SEC) {
      menuValueBuffer.value = RUNTIME_SECONDS % 60;
    }
    if(id == MENU_TIMER_RUN_MIN) {
      menuValueBuffer.value = RUNTIME_SECONDS / 60;
    }
    break;

  case MD_Menu::REQ_SET:
    if(id == MENU_TIMER_RUN_SEC) {
      RUNTIME_SECONDS = (RUNTIME_SECONDS / 60 * 60) + menuValueBuffer.value;    
    }
    if(id == MENU_TIMER_RUN_MIN) {
      RUNTIME_SECONDS = (RUNTIME_SECONDS % 60) + menuValueBuffer.value * 60;
    }

    writeSettingsToEeprom();
    break;   
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetRunInterval(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  uint16_t remainMin = SLEEPTIME_MINUTES;
  uint8_t day, hour;
  
  day = remainMin / MINUTES_PER_DAY; 
  remainMin = remainMin % MINUTES_PER_DAY;

  hour = remainMin / MINUTES_PER_HOUR;
  remainMin = remainMin % MINUTES_PER_HOUR;

  switch(reqType) {
  case MD_Menu::REQ_GET:
    if(id == MENU_TIMER_SLEEP_MIN) {
      menuValueBuffer.value = remainMin;
    }
    if(id == MENU_TIMER_SLEEP_HOUR) {
      menuValueBuffer.value = hour;
    }
    if(id == MENU_TIMER_SLEEP_DAY) {
      menuValueBuffer.value = day;
    }
    break;

  case MD_Menu::REQ_SET:
    if(id == MENU_TIMER_SLEEP_MIN) {
      SLEEPTIME_MINUTES = menuValueBuffer.value + (hour * MINUTES_PER_HOUR) + (day * MINUTES_PER_DAY);
    }
    if(id == MENU_TIMER_SLEEP_HOUR) {
      SLEEPTIME_MINUTES = remainMin + (menuValueBuffer.value * MINUTES_PER_HOUR) + (day * MINUTES_PER_DAY);
    }
    if(id == MENU_TIMER_SLEEP_DAY) {
      SLEEPTIME_MINUTES = remainMin + (hour * MINUTES_PER_HOUR) + (menuValueBuffer.value * MINUTES_PER_DAY);
    }
    
    // Sleep time must be longer than runtime to prevent unexpected behaviour
    if(SLEEPTIME_MINUTES > 0 && SLEEPTIME_MINUTES < RUNTIME_SECONDS / SECONDS_PER_MINUTE + 1) {
      // Just to be safe we set it twice as long as the runtime
      SLEEPTIME_MINUTES = (RUNTIME_SECONDS / SECONDS_PER_MINUTE) * 2;
    }

    writeSettingsToEeprom();
    restartSleepTimer();
    break;   
  } 

  return &menuValueBuffer;
}

MD_Menu::value_t *menuLaserBrightness(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      menuValueBuffer.value = LASER_BRIGHTNESS / 10;
      analogWrite(LASER_PIN, LASER_BRIGHTNESS);
      break;

    case MD_Menu::REQ_SET:
      LASER_BRIGHTNESS = menuValueBuffer.value * 10;
      analogWrite(LASER_PIN, LOW);
      writeSettingsToEeprom();
      break;   

    case MD_Menu::REQ_UPD:
      analogWrite(LASER_PIN, menuValueBuffer.value * 10);
      break;

    case MD_Menu::REQ_ESC:
      analogWrite(LASER_PIN, LOW);
      break;   
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMinimumSide(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = MENU_FROM_SIDE_VALUE(X_MIN);
      startLaser();
      xAxis.write(X_MIN);
      yAxis.write(MIDPOINT(Y_MIN, Y_MAX));
      break;

    case MD_Menu::REQ_SET:
      X_MIN = MENU_TO_SIDE_VALUE(menuValueBuffer.value);    
      stopLaser();
      writeSettingsToEeprom();
      break;    
    
    case MD_Menu::REQ_UPD:
      xAxis.write(MENU_TO_SIDE_VALUE(menuValueBuffer.value));
      break;  

    case MD_Menu::REQ_ESC:
      stopLaser();
      break;    
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMaximumSide(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = MENU_FROM_SIDE_VALUE(X_MAX);
      startLaser();
      xAxis.write(X_MAX);
      yAxis.write(MIDPOINT(Y_MIN, Y_MAX));
      break;

    case MD_Menu::REQ_SET:
      X_MAX = MENU_TO_SIDE_VALUE(menuValueBuffer.value);    
      stopLaser();
      writeSettingsToEeprom();
      break;    
    
    case MD_Menu::REQ_UPD:
      xAxis.write(MENU_TO_SIDE_VALUE(menuValueBuffer.value));
      break;

    case MD_Menu::REQ_ESC:
      stopLaser();
      break;   
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMinimumFront(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = MENU_FROM_FRONT_VALUE(Y_MIN);
      startLaser();
      xAxis.write(MIDPOINT(X_MIN, X_MAX));
      yAxis.write(Y_MIN);
      break;

    case MD_Menu::REQ_SET:
      Y_MIN = MENU_TO_FRONT_VALUE(menuValueBuffer.value);    
      stopLaser();
      writeSettingsToEeprom();
      break;    
    
    case MD_Menu::REQ_UPD:
      yAxis.write(MENU_TO_FRONT_VALUE(menuValueBuffer.value));
      break;   

    case MD_Menu::REQ_ESC:
      stopLaser();
      break;
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMaximumFront(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
    switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = MENU_FROM_FRONT_VALUE(Y_MAX);
      startLaser();
      xAxis.write(MIDPOINT(X_MIN, X_MAX));
      yAxis.write(Y_MAX);
      break;

    case MD_Menu::REQ_SET:
      Y_MAX = MENU_TO_FRONT_VALUE(menuValueBuffer.value);    
      stopLaser();
      writeSettingsToEeprom();
      break;    
    
    case MD_Menu::REQ_UPD:
      yAxis.write(MENU_TO_FRONT_VALUE(menuValueBuffer.value));
      break;  
      
    case MD_Menu::REQ_ESC:
      stopLaser();
      break;
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetSpeedMin(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
    switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = MIN_STEP_DELAY / 5;
      break;

    case MD_Menu::REQ_SET:
      MIN_STEP_DELAY = menuValueBuffer.value * 5;
      writeSettingsToEeprom();
      break;    

    case MD_Menu::REQ_UPD:
      //TODO: Check if lower than MAX_STEP_DELAY
      break;  
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetSpeedMax(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
    switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = MAX_STEP_DELAY / 5;
      break;

    case MD_Menu::REQ_SET:
      MAX_STEP_DELAY = menuValueBuffer.value * 5;
      writeSettingsToEeprom();
      break;  

    case MD_Menu::REQ_UPD:
      //TODO: Check if higher than MIN_STEP_DELAY
      break;    
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetTimerOnOff(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
    switch(reqType) {
    case MD_Menu::REQ_GET:
      menuValueBuffer.value = wakeUpTimerActive;
      break;

    case MD_Menu::REQ_SET:
      if(wakeUpTimerActive != menuValueBuffer.value) {
        wakeUpTimerActive = menuValueBuffer.value;
        restartSleepTimer();
        writeSettingsToEeprom();
      }
      break;  
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuCenterServos(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      return nullptr; // nullptr means no confirmation required
      break;

    case MD_Menu::REQ_SET:
      endRun();
      displayToast(F("Centering\nServos"), 1000, true);
      startLaser();
      xAxis.write(90);
      yAxis.write(90);
      delay(1500);
      stopLaser();
      break;    
  }
}

MD_Menu::value_t *menuLaserShowBoundaries(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      return nullptr; // nullptr means no confirmation required
      break;

    case MD_Menu::REQ_SET:
      laserShowSquareBoundaries();
      break;    
  }
}

MD_Menu::value_t *menuShowNextRun(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      return nullptr; // nullptr means no confirmation required
      break;

    case MD_Menu::REQ_SET:
      if(!wakeUpTimerActive || SLEEPTIME_MINUTES < 1) {
        displayToast(F("No timer set"), 1500, false); 
        break;
      }

      uint16_t remainSeconds = getNextRunSeconds();
      uint8_t day, hour, min;
      
      day = remainSeconds / SECONDS_PER_DAY; 
      remainSeconds = remainSeconds % SECONDS_PER_DAY;

      hour = remainSeconds / SECONDS_PER_HOUR;
      remainSeconds = remainSeconds % SECONDS_PER_HOUR;

      min = remainSeconds / SECONDS_PER_MINUTE;
      remainSeconds = remainSeconds % SECONDS_PER_MINUTE;

      char timeMessage[30];
      sprintf(timeMessage, "Next run in\n%02u Days %02u:%02u:%02u", day, hour, min, remainSeconds);
      displayToast(timeMessage, 1500, false);
    break;
  }
}

MD_Menu::userNavAction_t navigateMenu()
{
  MD_Menu::userNavAction_t nav = menuNavAction;
  menuNavAction = MD_Menu::NAV_NULL;
  return nav;
}

