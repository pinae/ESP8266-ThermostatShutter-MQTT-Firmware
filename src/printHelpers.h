#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "config.h"

void printTemperatureAndHumidity(float temperature, float humidity);
void printConfig(configDataStruct configData);
void printTemperatureSensorDetails(DHT_Unified dht);
void printHumiditySensorDetails(DHT_Unified dht);
