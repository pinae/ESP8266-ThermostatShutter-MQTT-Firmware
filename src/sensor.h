#ifndef SENSOR_DATA_TYPES
#define SENSOR_DATA_TYPES

struct SensorData {
    unsigned long time;
    float value;
};

#endif

void setUpSensors();
float getTemperature(unsigned int delta_t);
float getTemperature();
SensorData* getTemperatureLog();
float getHumidity(unsigned int delta_t);
float getHumidity();
SensorData* getAllHumidity();
void printTemperatureDataArr();
char* temperatureDataToJson(unsigned short offset);