#include <ESP8266WiFi.h>
#include "switches.h"

boolean switch1State[3] = {false, false, false};
boolean switch2State[3] = {false, false, false};
unsigned int switchTime;
SwitchStateTimeRange setupSequence[4];

void resetSetupSequence() {
    setupSequence[0].found = false;
    setupSequence[1].found = false;
    setupSequence[2].found = false;
    setupSequence[3].found = false;
}

void initSetupSequence() {
    setupSequence[0].start = 0;
    setupSequence[0].end = 500;
    setupSequence[0].state1 = false;
    setupSequence[0].state2 = true;
    setupSequence[1].start = 500;
    setupSequence[1].end = 1000;
    setupSequence[1].state1 = true;
    setupSequence[1].state2 = false;
    setupSequence[2].start = 1000;
    setupSequence[2].end = 1500;
    setupSequence[2].state1 = false;
    setupSequence[2].state2 = true;
    setupSequence[3].start = 1500;
    setupSequence[3].end = 2000;
    setupSequence[3].state1 = true;
    setupSequence[3].state2 = false;
    resetSetupSequence();
}

void initSwitches() {
    pinMode(SWITCH_1_PIN, INPUT);
    pinMode(SWITCH_2_PIN, INPUT);
    switchTime = 0;
    initSetupSequence();
}

void switchLoop(unsigned int delta_t) {
    switchTime += delta_t;
    switch1State[1] = switch1State[0];
    switch1State[0] = (digitalRead(SWITCH_1_PIN) == HIGH);
    if (switch1State[1] == switch1State[0]) switch1State[2] = switch1State[1];
    switch2State[1] = switch2State[0];
    switch2State[0] = (digitalRead(SWITCH_2_PIN) == HIGH);
    if (switch2State[1] == switch2State[0]) switch2State[2] = switch2State[1];

    bool sequenceFound = true;
    for (unsigned int i = 0; i < 4; i++) {
        if ((setupSequence[i].start <= switchTime) && 
            (setupSequence[i].end > switchTime)) {
            if ((switch1up() == setupSequence[i].state1) && 
                (switch2up() == setupSequence[i].state2)) {
                setupSequence[i].found = true;
            }
        }
        if ((i >= 1) && (setupSequence[i-1].end < switchTime) && !setupSequence[i-1].found) {
            resetSetupSequence();
            switchTime = 0;
        }
        sequenceFound = sequenceFound && setupSequence[i].found;
    }
    if (sequenceFound) {
        //foo
    }

    Serial.printf("Switch states: %d %d\n", switch1up(), switch2up());
}

bool switch1up() {
    if (switch1State[0] == switch1State[1]) {
        return switch1State[0];
    } else {
        return switch1State[2];
    }
}

bool switch2up() {
    if (switch2State[0] == switch2State[1]) {
        return switch2State[0];
    } else {
        return switch2State[2];
    }
}