#include <limits.h>
#include <stdlib.h>
#include "printHelpers.h"
#include "config.h"
#include "sensor.h"
#include "controller.h"
#include "mqtt.h"

unsigned long uptime;
unsigned long lastCycleUptime;
unsigned int delta_t;

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
  //pinMode(14, OUTPUT);
  pinMode(2, OUTPUT);
  while(!Serial) { }  // Wait for Serial to start
  setUpConfig();
  //setUpSensors();
  //setUpController();
  mqttSetup();
  //setSetpoint(25.0);
  Serial.println("Setup complete.");
}

void loop() {
  delta_t = getDelta_t();
  Serial.print("Loop iteration: "); Serial.print(delta_t); Serial.println(".");
  /*float t = getTemperature(delta_t);
  float h = getHumidity(delta_t);
  printTemperatureAndHumidity(t, h);*/
  //printTemperatureDataArr();
  /*for (unsigned short offset = 0; offset < UINT8_MAX; offset += 32) {
    char* jsonStr = temperatureDataToJson(offset);
    //Serial.println(jsonStr);
    //Serial.printf("offset: %u\tstrlen: %u\n", offset, strlen(jsonStr));
    char topic[45] = {'E', 'T', 'S', 'm', 'q', 't', 't', '-', 
          '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0',
          '/', 't', 'e', 'm', 'p', 'e', 'r', 'a', 't', 'u', 'r', 'e', 'L', 'o', 'g',
          '/', '0', '0', '0', 0};
    strncpy(&topic[0], getDeviceName(), 25);
    char offsetStr[4] = {'0', '0', '0', 0};
    sprintf(&offsetStr[0], "%3.3u", offset);
    strncpy(&topic[41], &offsetStr[0], 3);
    publishToMqtt(topic, jsonStr);
    free(jsonStr);
    delay(125);
  }*/
  //loopController(delta_t);
  delay(1000);
}
