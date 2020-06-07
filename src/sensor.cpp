#include <stdlib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <math.h>
#include "sensor.h"
#include "printHelpers.h"

#ifndef SENSOR_INIT
#define SENSOR_INIT

#define DHTTYPE DHT22  // DHT 22: AM2302
#define DHTPin 4 

uint32_t DHT22MinDelay;
uint8_t temperatureDataPos;
unsigned int temperatureTimer;
SensorData temperatureDataArr[256];
//char temperatureDataChunk[256 * sizeof(SensorData)];
//SensorData* temperatureData = (SensorData*) &temperatureDataArr[0];
//SensorData* temperatureData = (SensorData*) malloc(256 * sizeof(SensorData));
//SensorData* temperatureData;
uint8_t humidityDataPos;
unsigned int humidityTimer;
SensorData humidityDataArr[256];
//char humidityDataChunk[256 * sizeof(SensorData)];
//SensorData* humidityData = (SensorData*) &humidityDataArr[0];
//SensorData* humidityData = (SensorData*) malloc(256 * sizeof(SensorData));
//SensorData* humidityData;

DHT_Unified dht(DHTPin, DHTTYPE);

#endif

/*void printTemperatureData() {
  Serial.println("-------------------------\nPrinting temperatureData[]:");
  SensorData* t;
  for (unsigned int i = 0; i < 256; i++) {
    t = temperatureData + i * sizeof(SensorData);
    Serial.printf("i: %u\t", i);
    Serial.printf("time: %.10lu", t->time);
    Serial.printf("\ttemperature: %2.2f°C", t->value);
    if (i == temperatureDataPos) Serial.print(" <-- temperatureDataPos");
    Serial.println("");
  }
  Serial.println("-------------------------");
}*/

void printTemperatureDataArr() {
  Serial.println("-------------------------\nPrinting temperatureData[]:");
  //SensorData* t;
  for (unsigned int i = 0; i < 256; i++) {
    Serial.printf("i: %u\t", i);
    Serial.printf("time: %.10lu", temperatureDataArr[i].time);
    Serial.printf("\ttemperature: %2.2f°C", temperatureDataArr[i].value);
    if (i == temperatureDataPos) Serial.print(" <-- temperatureDataPos");
    Serial.println("");
  }
  Serial.println("-------------------------");
}

void setUpSensors() {
  //Serial.printf("sizeof(SendorData)=%u, malloc(%u)\n", sizeof(SensorData), 256 * sizeof(SensorData));
  //temperatureData = (SensorData*) malloc(256 * sizeof(SensorData));
  //humidityData = (SensorData*) malloc(256 * sizeof(SensorData));
  //Serial.printf("temperatureData(%lu): %lu - %f\n", temperatureData, (unsigned long) temperatureData->time, (float) temperatureData->value);
  /*SensorData* t;
  SensorData* h;*/
  pinMode(DHTPin, INPUT);
  for (unsigned int i = 0; i < 256; i++) {
    /*t = temperatureData + i * sizeof(SensorData);
    t->value = NAN;
    t->time = (unsigned long) 0;
    h = humidityData + i * sizeof(SensorData);
    h->value = NAN;
    h->time = 0;*/
    temperatureDataArr[i].time = 0;
    temperatureDataArr[i].value = NAN;
    humidityDataArr[i].time = 0;
    humidityDataArr[i].value = 0;
  }
  //Serial.printf("temperatureData(%lu): %lu - %f\n", temperatureData, (unsigned long) temperatureData->time, (float) temperatureData->value);
  temperatureDataPos = 255;
  humidityDataPos = 255;
  temperatureTimer = 0;
  humidityTimer = 0;
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  DHT22MinDelay = sensor.min_delay / 1000;
  printTemperatureSensorDetails(dht);
  printHumiditySensorDetails(dht);
  getTemperature(DHT22MinDelay+1);
  getHumidity(DHT22MinDelay+1);
}

float getTemperature(unsigned int delta_t) {
  //Serial.println("temp read 0");
  temperatureTimer += delta_t;
  //Serial.printf("tDataPos: %u\n", temperatureDataPos);
  //SensorData* t = temperatureData + temperatureDataPos * sizeof(SensorData);
  //Serial.printf("(%10lu,%f)\n", t->time, t->value);
  //Serial.println("temp read 1");
  if (temperatureTimer > DHT22MinDelay) {
    temperatureTimer = 0;
    sensors_event_t event;
    //Serial.println("temp read 2");
    dht.temperature().getEvent(&event);
    //Serial.println("temp read 3");
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    } else {
      //Serial.println("temp read 4");
      //unsigned long newTime = t->time + delta_t;
      unsigned long newTime = temperatureDataArr[temperatureDataPos].time + delta_t;
      temperatureDataPos = (temperatureDataPos + 1) % 256;
      /*t = temperatureData + temperatureDataPos * sizeof(SensorData);
      t->value = event.temperature;
      t->time = newTime;*/
      temperatureDataArr[temperatureDataPos].time = newTime;
      temperatureDataArr[temperatureDataPos].value = event.temperature;
    }
  }
  //Serial.println("temp read finished.");
  //return t->value;
  return temperatureDataArr[temperatureDataPos].value;
}

float getTemperature() {
  return getTemperature(0);
}

SensorData* getTemperatureLog() {
  //return temperatureData;
  return &temperatureDataArr[0];
}

float getHumidity(unsigned int delta_t) {
  humidityTimer += delta_t;
  //SensorData* h = humidityData + humidityDataPos * sizeof(SensorData);
  if (humidityTimer > DHT22MinDelay) {
    humidityTimer = 0;
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    } else {
      //unsigned long newTime = (*h).time + delta_t;
      unsigned long newTime = humidityDataArr[humidityDataPos].time + delta_t;
      humidityDataPos = (humidityDataPos + 1) % 256;
      /*h = humidityData + humidityDataPos * sizeof(SensorData);
      (*h).value = event.relative_humidity;
      (*h).time = newTime;*/
      humidityDataArr[humidityDataPos].value = newTime;
      humidityDataArr[humidityDataPos].value = event.relative_humidity;
    }
  }
  //return (*h).value;
  return humidityDataArr[humidityDataPos].value;
}

float getHumidity() {
  return getHumidity(0);
}

SensorData* getAllHumidity() {
  //return humidityData;
  return &humidityDataArr[0];
}

char* temperatureDataToJson(unsigned short offset) {
  int i;
  unsigned short j;
  unsigned short CHUNK_SIZE = 16;
  char* json = (char*) calloc(3 + (CHUNK_SIZE-1) + CHUNK_SIZE * 34, sizeof(char));
  for (i = 0; i < (2 + (CHUNK_SIZE-1) + CHUNK_SIZE * 34); i++) {
    strncpy(json+i, " ", 1);
  }
  *(json+(2 + (CHUNK_SIZE-1) + CHUNK_SIZE * 34)) = 0;
  char timeStr[10];
  char valueStr[6];
  SensorData* d;
  char* p = json;
  p = strncpy(p, "[", 1);
  p += 1;
  for (i = 0; i < CHUNK_SIZE; i++) {
    /*d = (SensorData*) (temperatureData + 
                       ((256+temperatureDataPos-CHUNK_SIZE-offset+i)%(256)) 
                       * sizeof(SensorData));*/
    d = &temperatureDataArr[(256+temperatureDataPos-CHUNK_SIZE-offset+i)%(256)];
    p = strncpy(p, "{\"time\":", 8);
    p += 8;
    sprintf(&timeStr[0], "%10lu", (*d).time);
    for (j = 0; j < 10; j++) 
      if (timeStr[j] == 0) {
        strncpy(&timeStr[j], "          ", 10-j);
        break;
      }
    p = strncpy(p, &timeStr[0], 10);
    p += 10;
    p = strncpy(p, ",\"value\":", 9);
    p += 9;
    strncpy(&valueStr[0], "      ", 6);
    if (isnan((*d).value) || ((*d).value < -40.0) || ((*d).value > 125.0)) {
      strncpy(&valueStr[0], "\"\"    ", 6);
    } else {
      sprintf(&valueStr[0], "%2.2f", (*d).value);
      for (j = 0; j < 6; j++) 
        if (valueStr[j] == 0) {
          strncpy(&valueStr[j], "      ", 6-j);
          break;
        }
    }
    p = strncpy(p, &valueStr[0], 6);
    p += 6;
    p = strncpy(p, "}", 1);
    p += 1;
    if (i < (CHUNK_SIZE - 1)) {
      p = strncpy(p, ",", 1);
      p += 1;
    }
  }
  p = strncpy(p, "]", 1);
  return json;
}