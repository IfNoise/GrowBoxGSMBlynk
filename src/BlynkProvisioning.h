/**************************************************************
 * This is a DEMO. You can use it only for development and testing.
 *
 * If you would like to add these features to your product,
 * please contact Blynk for Business:
 *
 *                  http://www.blynk.io/
 *
 **************************************************************/
#include <Arduino.h>
#include "Settings.h"
#include "BlynkState.h"
#include <BlynkSimpleTinyGSM.h>
#include "ResetButton.h"
#include "ConfigMode.h"
#include "Indicator.h"
#include "OTA.h"
#include "functions.h"

inline
void BlynkState::set(State m) {
  if (state != m) {
    DEBUG_PRINT(String(StateStr[state]) + " => " + StateStr[m]);
    state = m;
  }
}
void timersInit();
class Provisioning {

public:
  void begin()
  {
    DEBUG_PRINT("");
    DEBUG_PRINT("Hardware v" + String(BOARD_HARDWARE_VERSION));
    DEBUG_PRINT("Firmware v" + String(BOARD_FIRMWARE_VERSION));

    indicator_init();
    button_init();
    preferenceInit();
    timersInit();
    tcall_init();
    sensorsInit();
    gpioInit();
    setSyncInterval(10 * 60);
    if (configStore.flagConfig) {
      BlynkState::set(MODE_CONNECTING_NET);
    } else {
      BlynkState::set(MODE_WAIT_CONFIG);
    }
  }

  void run() {
    switch (BlynkState::get()) {
    case MODE_WAIT_CONFIG:       
    case MODE_CONFIGURING:            enterConfigMode();      break;
    case MODE_CONNECTING_NET:         enterConnectNet();      break;
    case MODE_WAIT_CONNECTION_NET:    waitConnectionNet();    break;
    case MODE_CONNECTING_GPRS:        enterConnectGPRS();     break;
    case MODE_WAIT_CONNECTION_GPRS:   waitConnectionGprs();   break;
    case MODE_CONNECTING_CLOUD:       enterConnectCloud();    break;
    case MODE_WAIT_CONNECTION_CLOUD:  waitConnectionCloud();  break;
    case MODE_WAIT_DISCONNECT:        waitDisconnect();       break;
    case MODE_RUNNING:                blynkRun();             break;
    //case MODE_OTA_UPGRADE:            enterOTA();             break;
    case MODE_SWITCH_TO_STA:          enterSwitchToSTA();     break;
    case MODE_RESET_CONFIG:           enterResetConfig();     break;

    }
  }

};

Provisioning BlynkProvisioning;
void ProvisioningRun()
{
    BlynkProvisioning.run();
}
void timersInit()
{
    timer.setInterval(5000L, sendSensor);
    timer.setInterval(900L, getSensors);
    timer.setInterval(970L, clockDisplay);
    timer.setInterval(1000L, checkTimers);
    timer.setInterval(950L, checkTermos);
    timer.setInterval(1200L, ProvisioningRun);
    timer.setInterval(600000L, syncRTC);
}