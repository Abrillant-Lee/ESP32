#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led
{
public:
    Led(int pin);
    void on();
    void off();
    void blink(int time);

private:
    int pin; // 添加这一行来声明pin成员变量
};

#endif // LED_H