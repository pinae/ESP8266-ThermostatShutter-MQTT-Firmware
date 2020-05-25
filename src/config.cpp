#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include "config.h"
#include "printHelpers.h"

#ifndef CONFIG_GLOBALS
#define CONFIG_GLOBALS

#define EEPROM_ADDR 0

bool shouldSaveConfig = false;
configDataStruct configData;
char deviceName[25] = {'E', 'T', 'S', 'm', 'q', 't', 't', '-', 
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#endif

void initDeviceName() {
    byte mac[6]; WiFi.macAddress(mac);
    char macStr[18];
    sprintf(&macStr[0], "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    strncpy(&deviceName[8], macStr, 18);
    Serial.print("MAC:\t"); Serial.println(macStr);
    Serial.print("Device name:\t"); Serial.println(deviceName);
}

void setUpConfig() {
    WiFiManager wifiManager;
    initDeviceName();
    EEPROM.begin(CONFIG_LEN);
    EEPROM.get(EEPROM_ADDR, configData);
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    WiFiManagerParameter custom_mqtt_server("mqttServer", "MQTT Server", configData.mqttServer, MQTT_SERVER_STRLEN);
    wifiManager.addParameter(&custom_mqtt_server);
    char mqttPortStr[10];
    sprintf(mqttPortStr, "%lu", configData.mqttPort);
    WiFiManagerParameter custom_mqtt_port("mqttPort", "MQTT Port", mqttPortStr, 10);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.autoConnect(getDeviceName(), "loving_ct");
    strncpy(configData.mqttServer, custom_mqtt_server.getValue(), MQTT_SERVER_STRLEN);
    char** conversionErrorPos = 0;
    unsigned long convertedPort = strtoul(&mqttPortStr[0], conversionErrorPos, 10);
    if (conversionErrorPos == 0) {
        configData.mqttPort = convertedPort;
    } else {
        Serial.print("Error converting MQTT Port number: "); Serial.println(mqttPortStr);
    }
    EEPROM.put(EEPROM_ADDR, configData);
    EEPROM.commit();

    printConfig(configData);
}

void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

char* getDeviceName() {
    return &deviceName[0];
}

char* getMqttServer() {
    return &configData.mqttServer[0];
}

unsigned long getMqttPort() {
    return configData.mqttPort;
}