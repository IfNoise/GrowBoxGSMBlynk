#include <Arduino.h>
#define USE_CUSTOM_BOARD          // See "Custom board configuration" in Settings.h

#define APP_DEBUG        // Comment this out to disable debug prints

#define BLYNK_PRINT Serial

#include "BlynkProvisioning.h"

void setup() {
  delay(500);
  Serial.begin(115200);
  Hrtc.begin();
  BlynkProvisioning.begin();

}

void loop() {
  timer.run();

}
