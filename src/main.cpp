#include "mqtt.h"
#include "bsp_servo.h"

int i;
void setup()
{
    Serial.begin(115200);
    initMQTTClient();
}

void loop()
{
    const char *properties[] = {"temperature", "humidity", "light_intensity"};
    int values[] = {25, 60, 1000}; // 这里的值是示例，你需要根据实际情况进行修改
    reportDeviceValues("esp32", properties, values, 3);
    delay(2000);
}
