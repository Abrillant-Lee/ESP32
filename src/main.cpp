/**
* @file    main.cpp
* @brief   本文件为华为云物联网平台ESP32设备接入示例代码，通过MQTT协议接入华为云物联网平台，实现设备上报属性功能
           注: 本示例代码仅供参考，开发者可根据自己的实际需求进行修改
* @author  Abrillant Lee
* @date    2023.5.19
* @version v1.0
*/

#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
/*MQTT连接配置*/
/*----------------需要更改的代码块--------------------*/
const char *ssid = "请填写你所要的wifi名称";                                                         
const char *password = "输入你的wifi密码";                                                  
const char *mqttServer = "输入你的华为云MQTT接入地址"; // 在总览 > 接入信息 > 设备接入查看
const int mqttPort = 1883; // 在总览 > 接入信息 > 设备接入查看
// 以下3个参数可以由HMACSHA256算法生成，网址:https://iot-tool.obs-website.cn-north-4.myhuaweicloud.com/
const char *clientId = "填写生成的ClientId";
const char *mqttUser = "填写生成的Username";
const char *mqttPassword = "填写生成的Password";
// 产品 > 查看 > topic管理 >寻找用途里面的设备上报属性
const char *Iot_link_MQTT_Topic_Report = "$oc/devices/{device_id}/sys/properties/report"; //注意修改自己的设备ID,{}要删除
#define Iot_link_Body_Format "{\"services\":[{\"service_id\":\"这里填写自己的服务id\",\"properties\":{%s" //注意修改自己的服务ID
/*-----------------------------------------------------*/
WiFiClient espClient; // ESP32WiFi模型定义
PubSubClient client(espClient);
double tem;

// 定义DS18B20数据口连接ESP32的4号IO上
#define ONE_WIRE_BUS 4
// 初始连接在单总线上的单总线设备
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void MQTT_Init()
{
  // WiFi网络连接部分
  WiFi.begin(ssid, password); // 开启ESP32的WiFi
  while (WiFi.status() != WL_CONNECTED)
  { // ESP尝试连接到WiFi网络
    delay(3000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  // MQTT服务器连接部分
  client.setServer(mqttServer, mqttPort); // 设置连接到MQTT服务器的参数
  client.setKeepAlive(120); // 设置心跳时间
  while (!client.connected())
  { // 尝试与MQTT服务器建立连接
    Serial.println("Connecting to MQTT...");
    if (client.connect(clientId, mqttUser, mqttPassword))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(6000);
    }
  }
}

void MQTT_POST()
{
  char properties[32]; 
  char jsonBuf[128]; 
  sprintf(properties,"\"Temperature\":%lf}}]}",tem); // 这里注意更改自己的属性名称,我的属性名称为Temperature
  sprintf(jsonBuf, Iot_link_Body_Format, properties); 
  client.publish(Iot_link_MQTT_Topic_Report, jsonBuf); 
  Serial.println(Iot_link_MQTT_Topic_Report); 
  Serial.println(jsonBuf); 
  Serial.println("MQTT Publish OK!"); 
}

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  MQTT_Init();
}

void loop()
{
  sensors.requestTemperatures();
  tem = sensors.getTempCByIndex(0);
  delay(500);
  MQTT_POST();
}