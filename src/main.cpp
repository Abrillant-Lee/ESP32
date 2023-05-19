/**
* @file    main.cpp
* @brief   ESP32通过MQTT协议接入华为云IoT平台，上报温度数据
* @author  Abrillant Lee
* @date    2023.5.19
* @version v1.0
*/
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// 定义DS18B20数据口连接ESP32的4号IO上
#define ONE_WIRE_BUS 4
// 初始连接在单总线上的单总线设备
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char *ssid = "STM32";                                                          // ESP32连接的WiFi名称
const char *password = "20050601";                                                   // WiFi密码
const char *mqttServer = "9745350464.st1.iotda-device.cn-north-4.myhuaweicloud.com"; // 华为云MQTT接入地址
const int mqttPort = 1883;
// 以下3个参数可以由HMACSHA256算法生成，为硬件通过MQTT协议接入华为云IoT平台的鉴权依据
const char *clientId = "64643c3ca1e0862b43d0d96c_0000_0_0_2023051809";
const char *mqttUser = "64643c3ca1e0862b43d0d96c_0000";
const char *mqttPassword = "d9fe074670d1b3b2fcb9f51b9391737d2f02f9b3c38b917d1eb7650f52098150";
WiFiClient espClient;                                                              // ESP32WiFi模型定义
PubSubClient client(espClient);                                                    // MQTT客户端定义

#define Iot_link_Body_Format "{\"services\":[{\"service_id\":\"1\",\"properties\":{%s"
const char *Iot_link_MQTT_Topic_Report = "$oc/devices/6446623740773741f9febca1_140524/sys/properties/report"; // 接收到命令后上发的响应topic
char *topic_Commands_Response = "$oc/devices/6446623740773741f9febca1_140524/sys/commands/response/request_id=";

double tem = 0;
long lastMsg = 0;


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
  client.setKeepAlive(60);                // 设置心跳时间
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
  sprintf(properties, "\"temp\":%d}}]}", tem);
  sprintf(jsonBuf, Iot_link_Body_Format, properties);
  client.publish(Iot_link_MQTT_Topic_Report, jsonBuf);
  Serial.println(Iot_link_MQTT_Topic_Report);
  Serial.println(jsonBuf);
  Serial.println("MQTT Publish OK!");
}


void steup()
{
  // 设置串口通信波特率
  Serial.begin(115200);
  // 初始库
  sensors.begin();
}

void loop()
{
  sensors.requestTemperatures(); // 发送命令获取温度值
  double tem = sensors.getTempCByIndex(0);
  delay(500);

  if (!client.connected())
  {
    MQTT_Init();
  }
  else
    client.loop();
  long now = millis();
  if (now - lastMsg > 5000) // 定时上报
  {
    lastMsg = now;
    MQTT_POST();
  }
}
