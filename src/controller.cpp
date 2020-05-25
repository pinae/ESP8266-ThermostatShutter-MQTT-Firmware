#include <Adafruit_Sensor.h>
#include "controller.h"
#include "sensor.h"

#ifndef CONTROLLER_INIT
#define CONTROLLER_INIT

#define ACTUATOR_CYCLE_TIME 1000
#define ACTUATOR_PIN 5

unsigned int actuatorTimer;
struct {
  float integratedError;
  float proportionalFactor;
  float differentialFactor;
  float integralFactor;
  float setpoint;
} ctlParams;
float lastCycleTemperature;

#endif

void setUpController() {
    pinMode(ACTUATOR_PIN, OUTPUT);
    digitalWrite(ACTUATOR_PIN, HIGH);
    actuatorTimer = 0;
    ctlParams.integratedError = 0.0;
    ctlParams.proportionalFactor = 0.1;
    ctlParams.differentialFactor = 1.0;
    ctlParams.integralFactor = 0.001;
    lastCycleTemperature = getTemperature();
}

void setSetpoint(float newSetpoint) {
    ctlParams.setpoint = newSetpoint;
}

float controller(unsigned int delta_t) {
    float error = ctlParams.setpoint - getTemperature();
    float differential = 0.0;
    float fdt = float(delta_t) / 1000.0;
    if (fdt > 0) {
        ctlParams.integratedError = error / fdt;
        differential = (getTemperature() - lastCycleTemperature) / fdt;
    }
    Serial.print(delta_t); Serial.print("\t");
    Serial.print(fdt); Serial.print("\t");
    Serial.print(lastCycleTemperature); Serial.print("\t");
    Serial.print(error); Serial.print("\t");
    Serial.print(differential); Serial.print("\t");
    Serial.print(ctlParams.proportionalFactor * error * 1000); Serial.print("\t");
    Serial.print(ctlParams.differentialFactor * differential / (error + 1)); Serial.print("\t");
    Serial.print(ctlParams.integralFactor * ctlParams.integratedError); Serial.println("\t");
    return ctlParams.proportionalFactor * error + 
           ctlParams.differentialFactor * differential / (error + 1) + 
           ctlParams.integralFactor * ctlParams.integratedError;
}

void setHeaterActuator(unsigned int delta_t, float valve_state) {
    actuatorTimer = actuatorTimer + delta_t;
    if (actuatorTimer > ACTUATOR_CYCLE_TIME) digitalWrite(ACTUATOR_PIN, HIGH);
    actuatorTimer = actuatorTimer % ACTUATOR_CYCLE_TIME;
    if (actuatorTimer > trunc(valve_state * ACTUATOR_CYCLE_TIME)) digitalWrite(ACTUATOR_PIN, LOW);
}

void loopController(unsigned int delta_t) {
    lastCycleTemperature = getTemperature();
    float valve_state = controller(delta_t);
    Serial.print("Valve State: "); Serial.println(valve_state);
    setHeaterActuator(delta_t, 0.3);
}