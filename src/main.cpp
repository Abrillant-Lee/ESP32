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
    Device_Report_value(i);
    i = i + 1;
    delay(2000);
}
