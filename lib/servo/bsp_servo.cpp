#include "bsp_servo.h"

Servo myservo; // 创建舵机对象
int pos = 0;   // 舵机位置变量
int servoPin;  // 舵机连接的引脚

void servo_setup(int pin)
{
  servoPin = pin;
  myservo.attach(servoPin); // 将舵机对象附加到对应的引脚
}

void servo_turn_on()
{
  for (pos = 0; pos <= 180; pos += 1)
  {                     // 从0度到180度
    myservo.write(pos); // 告诉舵机去到pos位置
    delay(5);           // 等待15ms让舵机达到位置
  }
}

void servo_turn_off()
{
  for (pos = 180; pos >= 0; pos -= 1)
  {                     // 从180度到0度
    myservo.write(pos); // 告诉舵机去到pos位置
    delay(5);           // 等待15ms让舵机达到位置
  }
}