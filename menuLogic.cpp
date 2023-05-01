#include "menu.h"

MD_Menu::value_t menuValueBuffer;

MD_Menu::value_t *menuSetFont(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType){
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

MD_Menu::value_t *menuSetMinimumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType){
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

MD_Menu::value_t *menuSetMaximumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType){
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

MD_Menu::value_t *menuSaveConfig(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType){
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

