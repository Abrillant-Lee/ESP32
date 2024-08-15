#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <DHT.h>
#include <stdlib.h>
#include <stdio.h>

// 定义DHT11引脚和类型
#define DHTPIN 2
#define DHTTYPE DHT11

#define react_tem 23   //知冷知热温度阈值

// DHT对象声明
extern DHT dht;

void react_on();

static float tem = 0;
static float hum = 0;

float gettemp();
float gethum();

// 读取并打印温湿度数据
// void readAndPrintDHTData();

#endif // DHT_SENSOR_H