#include "menu.h"

MD_Menu::value_t menuValueBuffer;

MD_Menu::value_t *menuSetMinimumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType){
  switch(reqType) {
    case MD_Menu::REQ_GET:
      stopLaser();
      menuValueBuffer.value = X_MIN;
      xAxis.write(X_MIN);
      xAxis.attach(X_SERVO_PIN);
      break;

    case MD_Menu::REQ_SET:
      X_MIN = menuValueBuffer.value;    
      xAxis.detach();
      break;    
    
    case MD_Menu::REQ_UPD:
      xAxis.write(menuValueBuffer.value);
      break;  

    case MD_Menu::REQ_ESC:
      xAxis.detach();
      break;    
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMaximumX(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      stopLaser();
      menuValueBuffer.value = X_MAX;
      xAxis.write(X_MAX);
      xAxis.attach(X_SERVO_PIN);
      break;

    case MD_Menu::REQ_SET:
      X_MAX = menuValueBuffer.value;    
      xAxis.detach();
      break;    
    
    case MD_Menu::REQ_UPD:
      xAxis.write(menuValueBuffer.value);
      break;

    case MD_Menu::REQ_ESC:
      xAxis.detach();
      break;   
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMinimumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType) {
  switch(reqType) {
    case MD_Menu::REQ_GET:
      stopLaser();
      menuValueBuffer.value = Y_MIN;
      yAxis.write(Y_MIN);
      yAxis.attach(Y_SERVO_PIN);
      break;

    case MD_Menu::REQ_SET:
      Y_MIN = menuValueBuffer.value;    
      yAxis.detach();
      break;    
    
    case MD_Menu::REQ_UPD:
      yAxis.write(menuValueBuffer.value);
      break;   

    case MD_Menu::REQ_ESC:
      yAxis.detach();
      break;
  }

  return &menuValueBuffer;
}

MD_Menu::value_t *menuSetMaximumY(MD_Menu::mnuId_t id, MD_Menu::requestType_t reqType){
    switch(reqType) {
    case MD_Menu::REQ_GET:
      stopLaser();
      menuValueBuffer.value = Y_MAX;
      yAxis.write(Y_MAX);
      yAxis.attach(Y_SERVO_PIN);
      break;

    case MD_Menu::REQ_SET:
      Y_MAX = menuValueBuffer.value;    
      yAxis.detach();
      break;    
    
    case MD_Menu::REQ_UPD:
      yAxis.write(menuValueBuffer.value);
      break;  
      
    case MD_Menu::REQ_ESC:
      yAxis.detach();
      break;
  }

  return &menuValueBuffer;
}

MD_Menu::userNavAction_t navigateMenu()
{
  MD_Menu::userNavAction_t nav = menuNavAction;
  menuNavAction = MD_Menu::NAV_NULL;
  return nav;
}

