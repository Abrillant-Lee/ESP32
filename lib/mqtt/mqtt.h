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


// 初始化MQTT客户端
void initMQTTClient();

// 订阅命令主题
void subscribeToCommandTopic();

// 当收到MQTT消息时的回调函数，参数包括主题、负载和负载长度
void mqttMessageCallback(char *topic, byte *payload, unsigned int length);

// 发送MQTT消息，参数包括主题和消息内容
void sendMQTTMessage(const char *topic, const char *message);

// 执行LedCtr命令，参数是一个包含命令参数的Json对象
void executeLedControlCommand(const JsonObject &paras);

// 上报设备的属性值，参数包括服务ID，属性名数组，属性值数组和数组大小
void reportDeviceValues(const char *service_id, const char **properties, int *values, int size);
/*举个例子，上报温度、湿度和光照强度的值,值是固定的
    const char* properties[] = {"temperature", "humidity", "light_intensity"};
    int values[] = {25, 60, 1000}; // 这里的值是示例，你需要根据实际情况进行修改
    reportDeviceValues("esp32", properties, values, 3);
    delay(1000);
*/
/* 举个例子，上报温度、湿度和光照强度的值，并且值不固定，需要调用相应的函数来获取
    const char *properties[] = {"temperature", "humidity", "light_intensity"};
    int values[] = {readTemperature(), readHumidity(), readLightIntensity()}; // 调用相应的函数来获取温度、湿度和光照强度的值
    reportDeviceValues("esp32", properties, values, 3);
    delay(1000);
*/