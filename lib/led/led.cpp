#include "led.h"

Led::Led(int pin) {
    pinMode(pin, OUTPUT);
    this->pin = pin;
}

void Led::on() {
    digitalWrite(this->pin, HIGH);
}

void Led::off() {
    digitalWrite(this->pin, LOW);
}

void Led::blink(int time) {
    this->on();
    delay(time);
    this->off();
    delay(time);
}