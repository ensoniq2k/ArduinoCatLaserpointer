#include "menu.h"

MD_Menu::value_t menuValueBuffer;

MD_Menu::value_t *menuRestartSleepTimer(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      return nullptr; // nullptr means no confirmation required
      break;

    case MD_Menu::REQ_SET:
      restartSleepTimer();
      displayToast(F("Timer restarted!"), 1000, true);
      break;    
  }
}

MD_Menu::value_t *menuSetRunDuration(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
  case MD_Menu::REQ_GET:
    if(id == MENU_RUN_SEC) {
      menuValueBuffer.value = RUNTIME_SECONDS % 60;
    }
    if(id == MENU_RUN_MIN) {
      menuValueBuffer.value = RUNTIME_SECONDS / 60;
    }
    break;

  case MD_Menu::REQ_SET:
    if(id == MENU_RUN_SEC) {
      RUNTIME_SECONDS = (RUNTIME_SECONDS / 60 * 60) + menuValueBuffer.value;    
    }
    if(id == MENU_RUN_MIN) {
      RUNTIME_SECONDS = (RUNTIME_SECONDS % 60) + menuValueBuffer.value * 60;
    }

    break;   
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetRunInterval(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  uint16_t remainTime = SLEEPTIME_MINUTES;
  uint8_t day, hour, min;
  
  day = remainTime / MINUTES_PER_DAY; 
  remainTime = remainTime % MINUTES_PER_DAY;

  hour = remainTime / MINUTES_PER_HOUR;
  remainTime = remainTime % MINUTES_PER_HOUR;

  min = remainTime;

  switch(reqType) {
  case MD_Menu::REQ_GET:
      if(id == MENU_SLEEP_MIN) {
        menuValueBuffer.value = min;
      }
      if(id == MENU_SLEEP_HOUR) {
        menuValueBuffer.value = hour;
      }
      if(id == MENU_SLEEP_DAY) {
        menuValueBuffer.value = day;
      }

    break;

  case MD_Menu::REQ_SET:
      if(id == MENU_SLEEP_MIN) {
        SLEEPTIME_MINUTES = menuValueBuffer.value + (hour * MINUTES_PER_HOUR) + (day * MINUTES_PER_DAY);
      }
      if(id == MENU_SLEEP_HOUR) {
        SLEEPTIME_MINUTES = min + (menuValueBuffer.value * MINUTES_PER_HOUR) + (day * MINUTES_PER_DAY);
      }
      if(id == MENU_SLEEP_DAY) {
        SLEEPTIME_MINUTES = min + (hour * MINUTES_PER_HOUR) + (menuValueBuffer.value * MINUTES_PER_DAY);
      }
      
      // Sleep time must be longer than runtime to prevent unexpected behaviour
      if(SLEEPTIME_MINUTES < RUNTIME_SECONDS / SECONDS_PER_MINUTE + 1) {
        // Just to be safe we set it twice as long as the runtime
        SLEEPTIME_MINUTES = (RUNTIME_SECONDS / SECONDS_PER_MINUTE) * 2;
      }

      restartSleepTimer();

    break;   
  } 

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetFont(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
    switch(reqType) {
    case MD_Menu::REQ_GET:
      menuValueBuffer.value = static_cast<int32_t>(currentFont);
      break;

    case MD_Menu::REQ_SET:
      currentFont = static_cast<Fonts>(menuValueBuffer.value);    
      displaySetFont(currentFont);
      break;    
    
    case MD_Menu::REQ_UPD:
      displaySetFont(static_cast<Fonts>(menuValueBuffer.value));
      displayShowCurrentText();
      break;  
      
    case MD_Menu::REQ_ESC:
      displaySetFont(currentFont);
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
      break;   

    case MD_Menu::REQ_UPD:
      analogWrite(LASER_PIN, menuValueBuffer.value);
      break;

    case MD_Menu::REQ_ESC:
      analogWrite(LASER_PIN, LOW);
      break;   
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMinimumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = X_MIN;
      startLaser();
      xAxis.write(X_MIN);
      yAxis.write(MIDPOINT(Y_MIN, Y_MAX));
      break;

    case MD_Menu::REQ_SET:
      X_MIN = menuValueBuffer.value;    
      stopLaser();
      break;    
    
    case MD_Menu::REQ_UPD:
      xAxis.write(menuValueBuffer.value);
      break;  

    case MD_Menu::REQ_ESC:
      stopLaser();
      break;    
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMaximumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = X_MAX;
      startLaser();
      xAxis.write(X_MAX);
      yAxis.write(MIDPOINT(Y_MIN, Y_MAX));
      break;

    case MD_Menu::REQ_SET:
      X_MAX = menuValueBuffer.value;    
      stopLaser();
      break;    
    
    case MD_Menu::REQ_UPD:
      xAxis.write(menuValueBuffer.value);
      break;

    case MD_Menu::REQ_ESC:
      stopLaser();
      break;   
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMinimumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = Y_MIN;
      startLaser();
      xAxis.write(MIDPOINT(X_MIN, X_MAX));
      yAxis.write(Y_MIN);
      break;

    case MD_Menu::REQ_SET:
      Y_MIN = menuValueBuffer.value;    
      stopLaser();
      break;    
    
    case MD_Menu::REQ_UPD:
      yAxis.write(menuValueBuffer.value);
      break;   

    case MD_Menu::REQ_ESC:
      stopLaser();
      break;
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMaximumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
    switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = Y_MAX;
      startLaser();
      xAxis.write(MIDPOINT(X_MIN, X_MAX));
      yAxis.write(Y_MAX);
      break;

    case MD_Menu::REQ_SET:
      Y_MAX = menuValueBuffer.value;    
      stopLaser();
      break;    
    
    case MD_Menu::REQ_UPD:
      yAxis.write(menuValueBuffer.value);
      break;  
      
    case MD_Menu::REQ_ESC:
      stopLaser();
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

MD_Menu::value_t *menuSaveConfig(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      return nullptr; // nullptr means no confirmation required
      break;

    case MD_Menu::REQ_SET:
      writeSettingsToEeprom();
      displayToast(F("Saved!"), 1000, true);
      break;    
  }
}

MD_Menu::userNavAction_t navigateMenu()
{
  MD_Menu::userNavAction_t nav = menuNavAction;
  menuNavAction = MD_Menu::NAV_NULL;
  return nav;
}

