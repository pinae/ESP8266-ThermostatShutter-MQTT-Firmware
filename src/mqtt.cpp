#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>
#include "mqtt.h"
#include "config.h"
#include "controller.h"

#ifndef MQTT_GLOBALS
#define MQTT_GLOBALS

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
char* clientId;
MqttMessageList* activeMessages = NULL;

#endif

void connectToMqtt() {
    mqttClient.connect();
}

void publishToMqtt(char* topic, char* payload) {
    MqttMessage* msg = (MqttMessage*) malloc(sizeof(MqttMessage));
    msg->id = mqttClient.publish(topic, 1, false, payload);
    if (msg->id == 0) {
        Serial.printf("Publishing of MQTT message to topic %s failed!\n", topic);
        free(msg);
        return;
    }
    Serial.printf("MQTT: published.\ttopic: %s\n", topic); 
    Serial.printf("payload:\t%.35s ... %.35s\tID: %d\n", payload, 
                        payload + strlen(payload) - 35, msg->id);
    msg->topic = topic;
    msg->payload = payload;
    MqttMessageList* newListItem = (MqttMessageList*) malloc(sizeof(MqttMessageList));
    newListItem->msg = msg;
    newListItem->next = activeMessages;
    activeMessages = newListItem;
}

void onMqttConnect(bool sessionPresent) {
    char* setTemperatureTopic = (char*) malloc(41);
    strcpy(setTemperatureTopic, "ETSmqtt-00:00:00:00:00:00/setTemperature");
    *(setTemperatureTopic + 41) = '\0';
    /*char setTemperatureTopic[41] = {'E', 'T', 'S', 'm', 'q', 't', 't', '-', 
        '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0',
        '/', 's', 'e', 't', 'T', 'e', 'm', 'p', 'e', 'r', 'a', 't', 'u', 'r', 'e', 0};
    strncpy(&setTemperatureTopic[0], getDeviceName(), 25);*/
    strncpy(setTemperatureTopic, getDeviceName(), 25);
    uint16_t subscriptionPacketId = mqttClient.subscribe(setTemperatureTopic, 2);
    Serial.print("Subscribed to MQTT-topic "); Serial.print(setTemperatureTopic);
    Serial.print("\tID: "); Serial.println(subscriptionPacketId);
    free(setTemperatureTopic);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
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
    char* plStr = (char*) malloc(len+1);
    strncpy(&plStr[0], payload, len);
    *(plStr + len) = '\0';
    Serial.printf("MQTT: %s (QoS: %d, len: %d, index: %d, total: %d) - Payload: %s\n", 
                  topic, properties.qos, len, index, total, plStr);
    Serial.println("-----------------");
    free(plStr);
}

void onMqttPublish(uint16_t packetId) {
    Serial.print("onMqttPublish()\tID:"); Serial.println(packetId);
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
    mqttClient = AsyncMqttClient();
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(getMqttServer(), getMqttPort());
    // mqttClient.setCredentials(configData.mqttUser, configData.mqttPassword);
    mqttClient.setClientId(getDeviceName());
    connectToMqtt();
}