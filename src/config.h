#define MQTT_SERVER_STRLEN 64
#define MQTT_PORT_BYTES_LEN 4
#define CONFIG_LEN 68

#ifndef CONFIG_DATA_STRUCT
#define CONFIG_DATA_STRUCT

struct configDataStruct { 
  char mqttServer[MQTT_SERVER_STRLEN] = "";
  unsigned long mqttPort = 1883;
};

#endif

void setUpConfig();
void saveConfigCallback();
char* getDeviceName();
char* getMqttServer();
unsigned long getMqttPort();