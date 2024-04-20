#include "mqtt.h"
#include "bsp_servo.h"
#include "infrared.h"

extern PubSubClient client;

void setup()
{
    // RXPIN 9  TXPIN 10
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 16, 17);
    initMQTTClient();
}

void loop()
{
    const char *properties[] = {"one", "two", "three"};
    int values[] = {1, 2, 3}; // 这里的值是示例，你需要根据实际情况进行修改
    reportDeviceValues("esp32", properties, values, 3);
    delay(2000);
}
