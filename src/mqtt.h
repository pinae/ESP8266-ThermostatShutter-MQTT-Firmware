#ifndef MQTT_TYPES
#define MQTT_TYPES

struct MqttMessage {
    uint16_t id;
    char* topic;
    char* payload;
};

struct MqttMessageList {
    MqttMessage* msg;
    MqttMessageList* next;
};

#endif

void mqttSetup();
void publishToMqtt(char* topic, char* payload);