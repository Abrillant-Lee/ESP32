#include "mqtt.h"
#include "bsp_servo.h"
#include "infrared.h"
#include "flex.h"
#include "dht11_owner.h"

extern PubSubClient client;

void setup()
{
    // RXPIN 9  TXPIN 10k
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 9, 10);
    initMQTTClient();
    dht.begin();
}

void loop()
{ // 开始采集数据
    const char *properties[] = {"must"};
    int values[] = {1}; // 调用相应的函数来获取温度和湿度的值
    reportDeviceValues("智控终端", properties, values, 1);
    delay(3000);
}
