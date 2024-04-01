#include "mqtt.h"

/*MQTT连接配置*/
/*-----------------------------------------------------*/
const char *ssid = "nan";                                                            // 接入wifi的名字
const char *password = "20021113";                                                   // 接入wifi的密码
const char *mqttServer = "92e10cb0f7.st1.iotda-device.cn-north-4.myhuaweicloud.com"; // 在华为云IoT的 总览->接入信息->MQTT（1883）后面的网址
const int mqttPort = 1883;
// 以下3个参数可以由HMACSHA256算法生成，为硬件通过MQTT协议接入华为云IoT平台的鉴权依据
const char *clientId = "6609494b71d845632a033b20_0331_0_0_2024033111";
const char *mqttUser = "6609494b71d845632a033b20_0331";
const char *mqttPassword = "8c03c225082feea540f65591dfbbad4cdeed779cca02ac072e3547098358fca7";

// 华为云IoT的产品->查看->Topic管理->设备上报属性数据的 $oc/devices/{你的设备ID}/sys/properties/report
const char *topic_properties_report = "$oc/devices/6609494b71d845632a033b20_0331/sys/properties/report";

// 订阅命令下发主题
const char *topic_commands_request = "$oc/devices/6609493b71d845632a033b20_0331/sys/    commands/#";
/*-----------------------------------------------------*/

WiFiClient espClient; // ESP32WiFi模型定义MQTT_Init
PubSubClient client(espClient);

// JSON文档的大小
const int JSON_DOC_SIZE = 1024;
// JSON字符串的缓冲区大小
const int JSON_BUFFER_SIZE = 200;

void initMQTTClient()
{
    // WiFi网络连接部分
    WiFi.begin(ssid, password); // 开启ESP32的WiFi
    while (WiFi.status() != WL_CONNECTED)
    { // ESP尝试连接到WiFi网络
        delay(3000);
        Serial.println("正在连接wifi.......");
    }
    Serial.println("连接WiFi成功！");

    // MQTT服务器连接部分
    client.setServer(mqttServer, mqttPort); // 设置连接到MQTT服务器的参数

    client.setKeepAlive(60); // 设置心跳时间

    while (!client.connected())
    { // 尝试与MQTT服务器建立连接
        Serial.println("正在通过MQTT连接华为云IoTDA平台...");

        if (client.connect(clientId, mqttUser, mqttPassword))
        {

            Serial.println("连接华为云IoTDA平台成功!");
        }
        else
        {

            Serial.print("连接失败");
            Serial.print(client.state());
            delay(6000);
        }
    }
    // client.setmqttMessageCallback(mqttMessageCallback); // 可以接受任何平台下发的内容

    // 订阅命令下发主题
    subscribeToCommandTopic();
}

// 发送MQTT消息的函数
// 参数：topic - MQTT主题
//       message - 要发送的消息
void sendMQTTMessage(const char *topic, const char *message)
{
    // 发布消息到指定的MQTT主题
    if (client.publish(topic, message) == true)
    {
        // 如果发布成功，打印成功消息
        Serial.println("消息发送成功");
    }
    else
    {
        // 如果发布失败，打印错误消息
        Serial.println("消息发送失败");
    }
    // 调用client.loop()以保持连接活跃
    client.loop();
    Serial.println("-------------");
}

// 创建一个JSON对象并发送到MQTT服务器的函数
// 参数：service_id - 服务ID
//       properties - 属性名称数组
//       values - 属性值数组
//       size - 数组的大小
void reportDeviceValues(const char *service_id, const char **properties, int *values, int size)
{
    DynamicJsonDocument doc(JSON_DOC_SIZE);

    JsonObject root = doc.to<JsonObject>();
    JsonArray services = root.createNestedArray("services");
    JsonObject service_1 = services.createNestedObject();
    JsonObject properties_1_1 = service_1.createNestedObject("properties");
    service_1["service_id"] = service_id;

    // 循环设置每个属性和其对应的值
    for (int i = 0; i < size; i++)
    {
        properties_1_1[properties[i]] = values[i];
    }

    char JSONmessageBuffer[JSON_BUFFER_SIZE];
    serializeJson(doc, JSONmessageBuffer);
    Serial.println("正在发送消息到MQTT主题..");
    Serial.println(JSONmessageBuffer);

    sendMQTTMessage(topic_properties_report, JSONmessageBuffer);
}

void subscribeToCommandTopic()
{
    if (client.subscribe(topic_commands_request))
    {
        // 如果订阅成功，打印成功消息
        Serial.println("成功订阅命令请求主题");
    }
    else
    {
        // 如果订阅失败，打印错误消息
        Serial.println("订阅命令请求主题失败");
    }

    // 设置回调函数，当接收到命令请求时，会调用这个函数
    client.setmqttMessageCallback(mqttMessageCallback);
}

// 定义一个函数，用于打印消息
void printMessage(const char *message, const char *value)
{
    Serial.print(message); // 打印消息
    Serial.println(value); // 打印值
}

// 定义一个函数，用于发布响应
bool publishResponse(const char *topic, const char *response)
{
    if (client.publish(topic, response))
    {                                                   // 如果发布成功
        printMessage("成功发布响应到命令响应主题", ""); // 打印成功消息
        return true;                                    // 返回true
    }
    else
    {                                                   // 如果发布失败
        printMessage("发布响应到命令响应主题失败", ""); // 打印失败消息
        return false;                                   // 返回false
    }
}

// 定义一个回调函数，这个函数会在ESP32从MQTT服务器接收到消息时被调用
// PubSubClient库会自动调用mqttMessageCallback函数，并传入消息的主题、内容和长度作为参数。
void mqttMessageCallback(char *topic, byte *payload, unsigned int length)
{
    printMessage("主题中收到的消息: ", topic); // 打印主题中收到的消息

    char *request_id_start = strstr(topic, "request_id="); // 在主题中查找"request_id="
    if (request_id_start == NULL)
    {                                          // 如果没有找到
        printMessage("请求ID未找到: ", topic); // 打印错误消息
        return;                                // 返回
    }

    char *request_id = request_id_start + 11; // 获取request_id
    printMessage("请求ID: ", request_id);     // 打印请求ID

    printMessage("平台下发命令:", ""); // 打印平台下发的命令
    for (int i = 0; i < length; i++)
    {                                   // 遍历命令的每一个字符
        Serial.print((char)payload[i]); // 打印字符
    }
    Serial.println(); // 打印换行符

    // 将payload转换为字符串
    String payloadStr = String((char *)payload);

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
        JsonObject paras = doc["paras"].as<JsonObject>(); // 获取"paras"字段
        executeLedControlCommand(paras);                      // 执行led_ctr命令
    }

    // 创建响应消息
    doc["result_code"] = 0;
    doc["result_desc"] = "OK";
    char response[200];
    serializeJson(doc, response);

    char topic_commands_response[200]; // 定义一个数组，用于存储命令应答主题
    // 使用snprintf函数，将格式化的字符串写入topic_commands_response
    snprintf(topic_commands_response, sizeof(topic_commands_response), "$oc/devices/6609494b71d845632a033b20_0331/sys/commands/response/request_id=%s", request_id);
    serializeJson(doc, response);                       // 将doc序列化为JSON字符串，并存储到response中
    publishResponse(topic_commands_response, response); // 发布响应
}

// 定义一个函数，用于执行led_ctr命令
void executeLedControlCommand(const JsonObject &paras)
{
    Led led(48);
    servo_setup(47);

    int value = paras["value"]; // 从"paras"字段中获取"value"字段，将其解析为一个整数
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