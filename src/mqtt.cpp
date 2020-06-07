#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>
#include "mqtt.h"
#include "config.h"
#include "controller.h"
#include "sensor.h"

#ifndef MQTT_GLOBALS
#define MQTT_GLOBALS

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
char* clientId;
MqttMessageList* activeMessages = NULL;
unsigned int sendTimer;
bool logSent[8];
bool mqttConnected = false;

#endif

void connectToMqtt() {
    mqttClient.connect();
}

void publishToMqtt(char* topic, char* payload) {
    MqttMessage* msg = (MqttMessage*) calloc(1, sizeof(MqttMessage));
    msg->id = mqttClient.publish(topic, 1, false, payload);
    if (msg->id == 0) {
        Serial.printf("Publishing of MQTT message to topic %s failed!\n", topic);
        free(msg);
        return;
    }
    /*Serial.printf("MQTT: published.\ttopic: %s\n", topic); 
    Serial.printf("payload:\t%.35s ... %.35s\tID: %d\n", payload, 
                        payload + strlen(payload) - 35, msg->id);*/
    msg->topic = topic;
    msg->payload = payload;
    MqttMessageList* newListItem = (MqttMessageList*) malloc(sizeof(MqttMessageList));
    newListItem->msg = msg;
    newListItem->next = activeMessages;
    activeMessages = newListItem;
}

void sendLoggedData(unsigned int delta_t) {
    sendTimer += delta_t;
    if (!mqttConnected) return;
    unsigned short i;
    unsigned short offset;
    for (i = 0; i < 8; i++) {
        if ((sendTimer > (i * 128)) && (!logSent[i])) {
            offset = i * 32;
            char* jsonStr = temperatureDataToJson(offset);
            //Serial.println(jsonStr);
            //Serial.printf("offset: %u\tstrlen: %u\n", offset, strlen(jsonStr));
            char* topic = (char*) calloc(45, sizeof(char));
            strncpy(topic, getDeviceName(), 25);
            strncpy(topic+25, "/temperatureLog/", 16);
            char* offsetStr = (char*) calloc(4, sizeof(char));
            sprintf(offsetStr, "%3.3u", offset);
            strncpy(topic+41, offsetStr, 4);
            *(topic+45) = 0;
            publishToMqtt(topic, jsonStr);
            free(jsonStr);
            free(topic);
            free(offsetStr);
            logSent[i] = true;
        }
    }
    if (sendTimer > 2000) {
        sendTimer = 0;
        for (i = 0; i < 8; i++) {
            logSent[i] = false;
        }
    }
}

void onMqttConnect(bool sessionPresent) {
    char* setTemperatureTopic = (char*) calloc(41, sizeof(char));
    strcpy(setTemperatureTopic, "ETSmqtt-00:00:00:00:00:00/setTemperature");
    *(setTemperatureTopic + 41) = 0;
    strncpy(setTemperatureTopic, getDeviceName(), 25);
    uint16_t subscriptionPacketId = mqttClient.subscribe(setTemperatureTopic, 2);
    Serial.print("Subscribed to MQTT-topic "); Serial.print(setTemperatureTopic);
    Serial.print("\tID: "); Serial.println(subscriptionPacketId);
    free(setTemperatureTopic);
    mqttConnected = true;
    Serial.println("MQTT connected.");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    mqttConnected = false;
    Serial.println("MQTT Disconnect!");
    if (WiFi.isConnected()) {
        mqttReconnectTimer.once(5, connectToMqtt);
    }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
    Serial.print("Subscription to "); Serial.print(packetId); Serial.print(" with QoS ");
    Serial.print(qos); Serial.println(" acknowledged.");
}

void onMqttUnsubscribe(uint16_t packetId) {

}

void onMqttMessage(char* topic, char* payload, 
                   AsyncMqttClientMessageProperties properties, 
                   size_t len, size_t index, size_t total) {
    char* plStr = (char*) calloc(len+1, sizeof(char));
    strncpy(&plStr[0], payload, len);
    *(plStr + len) = 0;
    Serial.printf("MQTT: %s (QoS: %d, len: %d, index: %d, total: %d) - Payload: %s\n", 
                  topic, properties.qos, len, index, total, plStr);
    Serial.println("-----------------");
    free(plStr);
}

void onMqttPublish(uint16_t packetId) {
    //Serial.print("onMqttPublish()\tID:"); Serial.println(packetId);
    MqttMessageList* last = NULL;
    MqttMessageList* item = activeMessages;
    while (item != NULL) {
        if (item->msg->id == packetId) {
            if (last == NULL) {
                activeMessages = item->next;
            } else {
                last->next = item->next;
            }
            MqttMessageList* itemForDeletion = item;
            free(itemForDeletion->msg);
            item = item->next;
            free(itemForDeletion);
        } else {
            last = item;
            item = item->next;
        }
    }
}

void mqttSetup() {
    mqttConnected = false;
    sendTimer = 0;
    for (unsigned int i = 0; i < 8; i++) {
        logSent[i] = false;
    }
    mqttClient = AsyncMqttClient();
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(getMqttServer(), getMqttPort());
    if (strlen(getMqttUsername()) > 0 && strlen(getMqttPassword())) {
        Serial.printf("MQTT: Using credentials: %s PW: %s\n", getMqttUsername(), getMqttPassword());
        mqttClient.setCredentials(getMqttUsername(), getMqttPassword());
    }
    mqttClient.setClientId(getDeviceName());
    connectToMqtt();
}