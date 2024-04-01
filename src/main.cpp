#include "mqtt.h"
#include "bsp_servo.h"

int i;
void setup()
{
    Serial.begin(115200);
    MQTT_Init();
}

void loop()
{
    const char *properties[] = {"temperature", "humidity", "light_intensity"};
    int values[] = {25, 60, 1000}; // 这里的值是示例，你需要根据实际情况进行修改
    Device_Report_value("esp32", properties, values, 3);
    delay(2000);
}
