#include "mqtt.h"

/*MQTT连接配置*/
/*-----------------------------------------------------*/
const char *ssid = "nan";                                                            // 接入wifi的名字
const char *password = "20021113";                                                   // 接入wifi的密码
const char *mqttServer = "92e10cb0f7.st1.iotda-device.cn-north-4.myhuaweicloud.com"; // 在华为云IoT的 总览->接入信息->MQTT（1883）后面的网址，这个是华为云的地址
const int mqttPort = 1883;
// 以下3个参数可以由HMACSHA256算法生成，为硬件通过MQTT协议接入华为云IoT平台的鉴权依据
const char *clientId = "6609494b71d845632a033b20_0331_0_0_2024033111";
const char *mqttUser = "6609494b71d845632a033b20_0331";
const char *mqttPassword = "8c03c225082feea540f65591dfbbad4cdeed779cca02ac072e3547098358fca7";

WiFiClient espClient; // ESP32WiFi模型定义
PubSubClient client(espClient);

// 华为云IoT的产品->查看->Topic管理->设备上报属性数据的 $oc/devices/{你的设备ID}/sys/properties/report
const char *topic_properties_report = "$oc/devices/{6609494b71d845632a033b20_0331}/sys/properties/report";

/*-----------------------------------------------------*/

extern Servo myservo; // 创建舵机对象
extern int pos; // 舵机位置变量

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

    client.setKeepAlive(60); // 设置心跳时间

    // 设置回调函数
    client.setCallback(callback);

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
    // 接受平台下发内容的初始化
}

/*
 * 作用：  上报到MQTT服务器任务
 * 参数：  (int)可以继续增加
 * 返回值：无
 * 命名说明：Capacity：容量
 */
void Device_Report_value(int capacity)
{
    // 创建一个大小为1024的动态JSON文档
    DynamicJsonDocument doc(1024);

    // 创建一个JSON对象root
    JsonObject root = doc.to<JsonObject>();
    // 在root对象中创建一个名为"services"的JSON数组
    JsonArray services = root.createNestedArray("services");
    // 在services数组中创建一个新的JSON对象
    JsonObject service_1 = services.createNestedObject();
    // 在service_1对象中创建一个名为"properties"的JSON对象
    JsonObject properties_1_1 = service_1.createNestedObject("properties");
    service_1["service_id"] = "esp32"; // 这里的填的是 产品->查看->服务列表的服务名字
    properties_1_1["test"] = capacity; // 这里是服务里面的属性

    // 创建一个大小为100的字符数组，用于存储序列化后的JSON字符串
    char JSONmessageBuffer[100];
    // 将doc文档序列化为JSON字符串，存储在JSONmessageBuffer中
    serializeJson(doc, JSONmessageBuffer);
    // 在串口打印"Sending message to MQTT topic.."和序列化后的JSON字符串
    Serial.println("Sending message to MQTT topic..");
    Serial.println(JSONmessageBuffer);

    // 将JSONmessageBuffer发布到MQTT服务器的topic_properties_report主题
    // 如果发布成功，打印"Success sending message"，否则打印"Error sending message"
    if (client.publish(topic_properties_report, JSONmessageBuffer) == true)
    {
        Serial.println("Success sending message");
    }
    else
    {
        Serial.println("Error sending message");
    }
    // 由于本函数是放在loop中不断循环执行，所以添加client.loop()进行保持硬件的活跃度
    // 避免由于长时间未向服务器发送消息而被服务器踢下线
    client.loop();
    Serial.println("-------------");
}

// 定义一个回调函数，这个函数会在ESP32从MQTT服务器接收到消息时被调用
// PubSubClient库会自动调用callback函数，并传入消息的主题、内容和长度作为参数。
void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);

    Serial.print("Message:");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }

    Serial.println();

    // 将payload转换为字符串
    String payloadStr;
    for (int i = 0; i < length; i++)
    {
        payloadStr += (char)payload[i];
    }

    // 创建一个JsonDocument对象，用于存储解析的JSON数据
    DynamicJsonDocument doc(200);

    // 解析JSON数据
    DeserializationError error = deserializeJson(doc, payloadStr);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // 检查"command_name"字段是否为"led_ctr"
    const char *command_name = doc["command_name"];
    if (strcmp(command_name, "led_ctr") == 0)
    {
        Led led(48);
        servo_setup(47);

        JsonObject paras = doc["paras"].as<JsonObject>(); // 获取"paras"字段
        int value = paras["value"];                       // 从"paras"字段中获取"value"字段，将其解析为一个整数
        if (value)
        {
            led.on();
            myservo.write(180); // 告诉舵机去到pos位置
        }

        else
        {
            led.off();
            myservo.write(10); // 0的话舵机内部会有问题
        }
    }
}