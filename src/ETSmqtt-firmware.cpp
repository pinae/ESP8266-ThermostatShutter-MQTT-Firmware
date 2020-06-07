#include <limits.h>
#include <stdlib.h>
#include "printHelpers.h"
#include "config.h"
#include "sensor.h"
#include "controller.h"
#include "mqtt.h"
#include "switches.h"

unsigned long uptime;
unsigned long lastCycleUptime;
unsigned int delta_t;
unsigned int printTemperatureTimer;

unsigned int getDelta_t() {
  uptime = millis();
  unsigned long delta_t = uptime - lastCycleUptime;
  if (delta_t > UINT_MAX) {
    delta_t = ULONG_MAX - lastCycleUptime + uptime;
  }
  lastCycleUptime = uptime;
  return delta_t;
}

void setup() {
  Serial.begin(115200);
  uptime = millis();
  lastCycleUptime = uptime;
  printTemperatureTimer = 0;
  initSwitches();
  //pinMode(14, OUTPUT);
  pinMode(2, OUTPUT);
  while(!Serial) { }  // Wait for Serial to start
  setUpConfig();
  setUpSensors();
  setUpController();
  mqttSetup();
  setSetpoint(25.0);
  Serial.println("Setup complete.");
}

void loop() {
  delta_t = getDelta_t();
  //Serial.print("Loop iteration: "); Serial.print(delta_t); Serial.println(".");
  switchLoop(delta_t);
  float t = getTemperature(delta_t);
  float h = getHumidity(delta_t);
  printTemperatureTimer += delta_t;
  if (printTemperatureTimer > 2000) {
    printTemperatureAndHumidity(t, h);
    printTemperatureTimer = 0;
  }
  //printTemperatureDataArr();
  sendLoggedData(delta_t);
  //loopController(delta_t);
  //delay(1000);
}
