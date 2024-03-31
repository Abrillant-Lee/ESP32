#ifndef SERVO_H
#define SERVO_H

#include <ESP32Servo.h>
#include <Arduino.h>

void servo_setup(int pin);
void servo_turn_on();
void servo_turn_off();

#endif // SERVO_H