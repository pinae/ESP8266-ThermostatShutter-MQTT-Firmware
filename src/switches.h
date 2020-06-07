#define SWITCH_1_PIN 9
#define SWITCH_2_PIN 10

#ifndef SWITCH_SIGNAL_TYPES
#define SWITCH_SIGNAL_TYPES

struct SwitchStateTimeRange {
    unsigned int start;
    unsigned int end;
    bool state1;
    bool state2;
    bool found;
};

#endif

void initSwitches();
void switchLoop(unsigned int delta_t);
bool switch1up();
bool switch2up();