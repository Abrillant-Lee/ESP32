// mpu6050.h
#ifndef _MPU6050_H_
#define _MPU6050_H_

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

extern Adafruit_MPU6050 mpu;

void MPU6050_Init();
void MPU6050_Data();

#endif // _MPU6050_H_
