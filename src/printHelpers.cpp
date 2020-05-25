#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "config.h"

void printTemperatureAndHumidity(float temperature, float humidity) {
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.print(F("°C\t"));
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.println(F("%"));
}

void printConfig(configDataStruct configData) {
  Serial.println("--------- Configuration -----------");
  Serial.print("MQTT Server: "); Serial.println(configData.mqttServer);
  Serial.print("MQTT Port: "); Serial.println(configData.mqttPort);
  Serial.println("-----------------------------------");
}

void printTemperatureSensorDetails(DHT_Unified dht) {
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
}

void printHumiditySensorDetails(DHT_Unified dht) {
  sensor_t sensor;
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
}