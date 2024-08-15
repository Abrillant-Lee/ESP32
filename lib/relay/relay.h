#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

class Relay {
public:
    Relay(uint8_t pin);
    void begin();
    void on();
    void off();
    bool isOn();

private:
    uint8_t _pin;
    bool _state;
};

#endif // RELAY_H