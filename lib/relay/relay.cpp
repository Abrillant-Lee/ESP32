#include "relay.h"

Relay::Relay(uint8_t pin) : _pin(pin), _state(false) {}

void Relay::begin() {
    pinMode(_pin, OUTPUT);
    off(); // 默认关闭继电器
}

void Relay::on() {
    digitalWrite(_pin, HIGH);
    _state = true;
}

void Relay::off() {
    digitalWrite(_pin, LOW);
    _state = false;
}

bool Relay::isOn() {
    return _state;
}