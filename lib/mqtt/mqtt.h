/**
 * @file mqtt.h
 * @brief 这个头文件包含了MQTT客户端的声明和一些全局变量的声明。
 *
 * 这个文件包含了以下内容：
 * - 引入了一些必要的库文件，包括Arduino、Wire、WiFi、PubSubClient和ArduinoJson库。
 * - 引入了自定义的led和bsp_servo头文件。
 * - 声明了一些全局变量，包括WiFi的SSID和密码、MQTT服务器的地址和端口、MQTT客户端的ID、用户名和密码、MQTT的主题、舵机对象和舵机位置变量。
 * - 声明了一些函数，包括初始化MQTT客户端的函数、订阅命令主题的函数、处理MQTT消息的回调函数、发送MQTT消息的函数、执行LedCtr命令的函数和上报设备属性值的函数。
 *
 * @author 李浩楠
 * @date 2024-4-1、
 * @version 1.0:控制舵机开关命令下发
 * @version 2.0(2024-4-21):新增红外控制空调开关、制冷制热、升降温度命令下功能
 */

#include <Arduino.h>
#include <Wire.h>         //Arduino的库文件
#include <WiFi.h>         //ESP32的库文件
#include <PubSubClient.h> //在“管理库”搜索“PubSubClient”，下载库文件
#include <ArduinoJson.h>

#include "led.h"
#include "bsp_servo.h"
#include "infrared.h"

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

// 执行Dht11Ctr命令，参数是一个包含命令参数的Json对象
void executeDht11ControlCommand(const JsonObject &paras);

void executeAirConditionerControlCommand(const JsonObject &paras); // 执行led_ctr命令

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