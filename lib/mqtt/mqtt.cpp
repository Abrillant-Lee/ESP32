/**
 * @file mqtt.cpp
 * @brief 这个文件包含了MQTT客户端的实现。
 *
 * 这个文件包含了以下内容：
 * - WiFi网络的SSID和密码，用于连接WiFi网络。
 * - MQTT服务器的地址和端口，用于连接MQTT服务器。
 * - MQTT客户端的ID、用户名和密码，用于鉴权。
 * - MQTT的主题，用于发布和订阅消息。
 * - WiFiClient和PubSubClient对象的定义，用于实现MQTT客户端的功能。
 * - JSON文档和缓冲区的大小，用于处理JSON格式的消息。
 * - 初始化MQTT客户端的函数，用于连接WiFi网络和MQTT服务器。
 *
 * @author 李浩楠
 * @date 2024-4-1
 */

#include "mqtt.h"

#include <DHT.h>

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
const char *topic_commands_request = "$oc/devices/6609493b71d845632a033b20_0331/sys/commands/#";
/*-----------------------------------------------------*/

WiFiClient espClient; // ESP32WiFi模型定义MQTT_Init
PubSubClient client(espClient);
int dht11_ctr = 0; // 全局变量，用于控制DHT11的数据采集

// JSON文档的大小
const int JSON_DOC_SIZE = 1024;
// JSON字符串的缓冲区大小
const int JSON_BUFFER_SIZE = 200;

/**
 * @brief 初始化MQTT客户端，连接WiFi网络和MQTT服务器。
 *
 * 此函数将ESP32连接到指定的WiFi网络，并尝试与MQTT服务器建立连接。它设置连接的必要参数，如服务器地址、端口、客户端ID、用户名和密码。它还设置连接的保持活动时间。
 * 如果与MQTT服务器的连接失败，函数将在6秒的延迟后重试。
 * 成功连接到MQTT服务器后，函数将订阅命令主题。
 * @author Abrillant-Lee
 * @date 2024-4-1
 */
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

/**
 * @brief 向指定的主题发送MQTT消息。
 *
 * 此函数使用MQTT客户端将消息发布到指定的MQTT主题。
 * 如果消息成功发布，将在串行监视器中打印成功消息。
 * 如果消息发布失败，将在串行监视器中打印错误消息。
 * 在发布消息后，调用`client.loop()`以保持连接活动。
 *
 * @param topic 要发布消息的MQTT主题。
 * @param message 要发布的消息。
 * @date 2024-4-1
 * @author Abrillant-Lee
 */
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

/**
 * @brief 将设备值报告给指定的MQTT服务。
 *
 * 该函数创建一个包含设备值的JSON文档，并将其发送到MQTT主题。
 * 设备值由properties和values数组指定。
 *
 * @param service_id MQTT服务的ID。
 * @param properties 一个字符串数组，表示属性名称。
 * @param values 一个整数数组，表示属性值。
 * @param size properties和values数组的大小。
 */
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

/**
 * @brief 订阅命令请求主题并设置回调函数。
 *
 * 该函数用于订阅命令请求主题，并设置回调函数以处理接收到的命令请求。
 * 如果订阅成功，将打印成功消息；如果订阅失败，将打印错误消息。
 */
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
    client.setCallback(mqttMessageCallback);
}

/**
 * @brief 打印消息和值
 *
 * @param message 消息字符串
 * @param value 值字符串
 *
 * @date 2024-4-1
 * @author Abrillant-Lee
 */
void printMessage(const char *message, const char *value)
{
    Serial.print(message); // 打印消息
    Serial.println(value); // 打印值
}

/**
 * Publishes a response to the specified topic.
 * 发布响应到指定的主题。
 *
 * @param topic The topic to publish the response to.
 *              要发布响应的主题。
 * @param response The response to be published.
 *                 要发布的响应。
 * @return true if the response was published successfully, false otherwise.
 *         如果成功发布响应，则返回true；否则返回false。
 */
bool publishResponse(const char *topic, const char *response)
{
    Serial.print("Topic: ");
    Serial.println(topic);
    Serial.print("Response: ");
    Serial.println(response);
    Serial.print("Client state: ");
    Serial.println(client.state());
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

/**
 * @brief MQTT消息接收的回调函数。
 *
 * 当在MQTT主题上接收到消息时，将调用此函数。
 * 它处理接收到的消息，并根据消息内容执行必要的操作。
 *
 * @param topic 接收到消息的主题。
 * @param payload 接收到消息的有效载荷。
 * @param length 有效载荷的长度。
 */
void mqttMessageCallback(char *topic, byte *payload, unsigned int length)
{
    {

        Serial.println("-------------");
        printMessage("接收到华为云平台的命令下发消息: ", topic); // 打印主题中收到的消息

        char *request_id_start = strstr(topic, "request_id="); // 在主题中查找"request_id="
        printMessage("请求ID_start: ", request_id_start);      // 打印请求ID
        if (request_id_start == NULL)
        {                                          // 如果没有找到
            printMessage("请求ID未找到: ", topic); // 打印错误消息
            return;                                // 返回
        }

        const char *request_id = request_id_start + 11; // 获取request_id
        printMessage("请求ID: ", request_id);           // 打印请求ID

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
            executeLedControlCommand(paras);                  // 执行led_ctr命令
        }
        else if (strcmp(command_name, "dht11_ctr") == 0)
        {
            JsonObject paras = doc["paras"].as<JsonObject>(); // 获取"paras"字段
            executeDht11ControlCommand(paras);                // 执行led_ctr命令
        }
        else if (strcmp(command_name, "air_conditioner") == 0)
        {
            JsonObject paras = doc["paras"].as<JsonObject>(); // 获取"paras"字段
            executeAirConditionerControlCommand(paras);       // 执行led_ctr命令
        }

        printMessage("请求ID: ", request_id); // 打印请求ID

        // 创建响应消息
        doc["result_code"] = 0;
        doc["result_desc"] = "OK";
        char response[200];
        serializeJson(doc, response);

        char topic_commands_response[200]; // 定义一个数组，用于存储命令应答主题
        // 使用snprintf函数，将格式化的字符串写入topic_commands_response
        snprintf(topic_commands_response, sizeof(topic_commands_response), "$oc/devices/6609494b71d845632a033b20_0331/sys/commands/response/request_id=%s", request_id);
        publishResponse(topic_commands_response, response); // 发布响应
    }
}

/**
 * Executes a LED control command based on the provided parameters.
 * 根据提供的参数执行 LED 控制命令。
 *
 * @param paras The JsonObject containing the command parameters.
 *              包含命令参数的 JsonObject 对象。
 */
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

// void executeDht11ControlCommand(const JsonObject &paras)
// {
//     int value = paras["value"]; // 从"paras"字段中获取"value"字段，将其解析为一个整数
//     if (value)
//     {
//         dht11_ctr = 1;

//         // 开始采集数据
//         DHT dht(2, DHT11);
//         dht.begin();

//         const char *properties[] = {"temperature", "humidity"};

//         // 当dht11_ctr为1时，持续采集数据并上报
//         while (dht11_ctr)
//         {
//             int values[] = {dht.readHumidity(), dht.readTemperature()}; // 调用相应的函数来获取温度和湿度的值
//             reportDeviceValues("esp32", properties, values, 2);
//             delay(2000);
//         }
//     }
//     else
//     {
//         // 停止采集数据
//         dht11_ctr = 0;
//     }
// }
void executeDht11ControlCommand(const JsonObject &paras)
{
    int value = paras["value"]; // 从"paras"字段中获取"value"字段，将其解析为一个整数
    Led led(48);
    // 开始采集数据
    DHT dht(2, DHT11);
    dht.begin();
    const char *properties[] = {"temperature", "humidity"};
    if (value)
    {

        // 当dht11_ctr为1时，持续采集数据并上报
        int values[] = {dht.readHumidity(), dht.readTemperature()}; // 调用相应的函数来获取温度和湿度的值
        reportDeviceValues("esp32", properties, values, 2);
        led.on();
    }
    else
    {
        led.off();
    }
}

void executeAirConditionerControlCommand(const JsonObject &paras)
{

    const char *ctr = paras["ctr"];
    byte data[32], len;

    if (strcmp(ctr, "ON") == 0)
    {
        len = IrSend_data(data, 0);
        Serial1.write(data, len);
        Serial.println("空调已打开");
    }
    else if (strcmp(ctr, "OFF") == 0)
    {
        len = IrSend_data(data, 1);
        Serial1.write(data, len);
        Serial.println("空调已关闭");
    }
    else if (strcmp(ctr, "HOT") == 0)
    {
        len = IrSend_data(data, 2);
        Serial1.write(data, len);
        Serial.println("空调已设置为制热模式");
    }
    else if (strcmp(ctr, "COLD") == 0)
    {
        len = IrSend_data(data, 3);
        Serial1.write(data, len);
        Serial.println("空调已设置为制冷模式");
    }
    else if (strcmp(ctr, "RAISE") == 0)
    {
        len = IrSend_data(data, 4);
        Serial1.write(data, len);
        Serial.println("升高温度");
    }
    else if (strcmp(ctr, "REDUCE") == 0)
    {
        len = IrSend_data(data, 5);
        Serial1.write(data, len);
        Serial.println("降低温度");
    }
}