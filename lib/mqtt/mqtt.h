#include <Arduino.h>
#include <Wire.h>         //Arduino的库文件
#include <WiFi.h>         //ESP32的库文件
#include <PubSubClient.h> //在“管理库”搜索“PubSubClient”，下载库文件
#include <ArduinoJson.h>

#include "led.h"
#include "bsp_servo.h"

extern const char *ssid;
extern const char *password;
extern const char *mqttServer;
extern const int mqttPort;
extern const char *clientId;
extern const char *mqttUser;
extern const char *mqttPassword;
extern const char *topic_properties_report;
extern const char *topic_commands_request;
extern Servo myservo; // 创建舵机对象
extern int pos;       // 舵机位置变量

void callback(char *topic, byte *payload, unsigned int length);
void MQTT_Init();
void Device_Report_value(int capacity);
void subscribeCommandTopic();