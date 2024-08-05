#include "menu.h"

MD_Menu::value_t menuValueBuffer;

MD_Menu::value_t *menuRestartSleepTimer(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      return nullptr; // nullptr means no confirmation required
      break;

    case MD_Menu::REQ_SET:
      restartSleepTimer();
      displayToast(F(MENUSTR_TIMER_RESTARTED), 10000, false);
      break;    
  }
}

MD_Menu::value_t *menuSetRunDuration(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
  case MD_Menu::REQ_GET:
    menuValueBuffer.value = RUNTIME_SECONDS;
    break;

  case MD_Menu::REQ_SET:
    RUNTIME_SECONDS = menuValueBuffer.value;    

    writeSettingsToEeprom();
    break;   
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetRunInterval(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {

  switch(reqType) {
    case MD_Menu::REQ_GET:
      menuValueBuffer.value = static_cast<uint32_t>(SLEEPTIME_MINUTES) * SECONDS_PER_MINUTE;
      break;

    case MD_Menu::REQ_SET:
      SLEEPTIME_MINUTES = static_cast<uint16_t>(menuValueBuffer.value / SECONDS_PER_MINUTE);

      // Sleep time must be longer than runtime to prevent unexpected behaviour
      if(SLEEPTIME_MINUTES > 0 && SLEEPTIME_MINUTES < RUNTIME_SECONDS / SECONDS_PER_MINUTE + 1) {
        // Just to be safe we set it twice as long as the runtime
        SLEEPTIME_MINUTES = (RUNTIME_SECONDS / SECONDS_PER_MINUTE) + 1;
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
      menuValueBuffer.value = sqrt(LASER_BRIGHTNESS);
      analogWrite(LASER_PIN, LASER_BRIGHTNESS);
      break;

    case MD_Menu::REQ_SET:
      LASER_BRIGHTNESS = pow(menuValueBuffer.value, 2);
      analogWrite(LASER_PIN, LOW);
      writeSettingsToEeprom();
      break;   

    case MD_Menu::REQ_UPD:
      analogWrite(LASER_PIN, pow(menuValueBuffer.value, 2));
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
      menuValueBuffer.value = MENU_FROM_SIDE_VALUE(SIDE_MIN);
      startLaser();
      sideAxis.write(SIDE_MIN);
      frontAxis.write(MIDPOINT(FRONT_MIN, FRONT_MAX));
      break;

    case MD_Menu::REQ_SET:
      SIDE_MIN = MENU_TO_SIDE_VALUE(menuValueBuffer.value);    
      stopLaser();
      writeSettingsToEeprom();
      break;    
    
    case MD_Menu::REQ_UPD:
      if(MENU_TO_SIDE_VALUE(menuValueBuffer.value) > SIDE_MAX - MIN_AREA_SIZE) {
        menuValueBuffer.value = menuValueBuffer.value -1;
      }
      sideAxis.write(MENU_TO_SIDE_VALUE(menuValueBuffer.value));
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
      menuValueBuffer.value = MENU_FROM_SIDE_VALUE(SIDE_MAX);
      startLaser();
      sideAxis.write(SIDE_MAX);
      frontAxis.write(MIDPOINT(FRONT_MIN, FRONT_MAX));
      break;

    case MD_Menu::REQ_SET:
      SIDE_MAX = MENU_TO_SIDE_VALUE(menuValueBuffer.value);    
      stopLaser();
      writeSettingsToEeprom();
      break;    
    
    case MD_Menu::REQ_UPD:
      if(MENU_TO_SIDE_VALUE(menuValueBuffer.value) < SIDE_MIN + MIN_AREA_SIZE) {
        menuValueBuffer.value = menuValueBuffer.value +1;
      }
      sideAxis.write(MENU_TO_SIDE_VALUE(menuValueBuffer.value));
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
      menuValueBuffer.value = MENU_FROM_FRONT_VALUE(FRONT_MIN);
      startLaser();
      sideAxis.write(MIDPOINT(SIDE_MIN, SIDE_MAX));
      frontAxis.write(FRONT_MIN);
      break;

    case MD_Menu::REQ_SET:
      FRONT_MIN = MENU_TO_FRONT_VALUE(menuValueBuffer.value);    
      stopLaser();
      writeSettingsToEeprom();
      break;    
    
    case MD_Menu::REQ_UPD:
      if(MENU_TO_FRONT_VALUE(menuValueBuffer.value) > FRONT_MAX - MIN_AREA_SIZE) {
        menuValueBuffer.value = menuValueBuffer.value +1;
      }
      frontAxis.write(MENU_TO_FRONT_VALUE(menuValueBuffer.value));
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
      menuValueBuffer.value = MENU_FROM_FRONT_VALUE(FRONT_MAX);
      startLaser();
      sideAxis.write(MIDPOINT(SIDE_MIN, SIDE_MAX));
      frontAxis.write(FRONT_MAX);
      break;

    case MD_Menu::REQ_SET:
      FRONT_MAX = MENU_TO_FRONT_VALUE(menuValueBuffer.value);    
      stopLaser();
      writeSettingsToEeprom();
      break;    
    
    case MD_Menu::REQ_UPD:
      if(MENU_TO_FRONT_VALUE(menuValueBuffer.value) < FRONT_MIN + MIN_AREA_SIZE) {
        menuValueBuffer.value = menuValueBuffer.value -1;
      }
      frontAxis.write(MENU_TO_FRONT_VALUE(menuValueBuffer.value));
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
      menuValueBuffer.value = MENU_FROM_SPEED_VALUE(MAX_STEP_DELAY);
      break;

    case MD_Menu::REQ_SET:
      MAX_STEP_DELAY = MENU_TO_SPEED_VALUE(menuValueBuffer.value);
      writeSettingsToEeprom();
      break;    

    case MD_Menu::REQ_UPD:
      if(menuValueBuffer.value > MENU_FROM_SPEED_VALUE(MIN_STEP_DELAY)) {
        menuValueBuffer.value = menuValueBuffer.value -1;
      }
      break;  
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetSpeedMax(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
    switch(reqType) {
    case MD_Menu::REQ_GET:
      endRun();
      menuValueBuffer.value = MENU_FROM_SPEED_VALUE(MIN_STEP_DELAY);
      break;

    case MD_Menu::REQ_SET:
      MIN_STEP_DELAY = MENU_TO_SPEED_VALUE(menuValueBuffer.value);
      writeSettingsToEeprom();
      break;  

    case MD_Menu::REQ_UPD:
      if(menuValueBuffer.value < MENU_FROM_SPEED_VALUE(MAX_STEP_DELAY)) {
        menuValueBuffer.value = menuValueBuffer.value +1;
      }
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

MD_Menu::value_t *menuSetLaserBlankout(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
    switch(reqType) {
    case MD_Menu::REQ_GET:
      menuValueBuffer.value = laserBlankingEnabled;
      break;

    case MD_Menu::REQ_SET:
      laserBlankingEnabled = menuValueBuffer.value;
      writeSettingsToEeprom();
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
      displayToast(F(MENUSTR_CENTERING_SERVOS), 0, false);
      startLaser();
      sideAxis.write(90);
      frontAxis.write(90);
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
        displayToast(F(MENUSTR_NO_TIMER_SET), 10000, false); 
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
      sprintf(timeMessage, MENUSTR_NEXT_RUN_IN, day, hour, min, remainSeconds);
      displayToast(timeMessage, 10000, false);
    break;
  }
}

MD_Menu::userNavAction_t navigateMenu()
{
  MD_Menu::userNavAction_t nav = menuNavAction;
  menuNavAction = MD_Menu::NAV_NULL;
  return nav;
}

