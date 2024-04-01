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
void Device_Report_value(const char *service_id, const char **properties, int *values, int size);
/*举个例子，上报温度、湿度和光照强度的值,值是固定的
    const char* properties[] = {"temperature", "humidity", "light_intensity"};
    int values[] = {25, 60, 1000}; // 这里的值是示例，你需要根据实际情况进行修改
    Device_Report_value("esp32", properties, values, 3);
    delay(1000);
*/
/* 举个例子，上报温度、湿度和光照强度的值，并且值不固定，需要调用相应的函数来获取
    const char *properties[] = {"temperature", "humidity", "light_intensity"};
    int values[] = {readTemperature(), readHumidity(), readLightIntensity()}; // 调用相应的函数来获取温度、湿度和光照强度的值
    Device_Report_value("esp32", properties, values, 3);
    delay(1000);
*/
void subscribeCommandTopic();
void sendMQTTMessage(const char *topic, const char *message);
void execute_LedCtr_Command(const JsonObject &paras);