#include "mqtt.h"
#include "bsp_servo.h"
#include "infrared.h"
#include "flex.h"
#include "mpu6050.h"

extern PubSubClient client;

void setup()
{
    // RXPIN 9  TXPIN 10
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 16, 17);
    initMQTTClient();
    MPU6050_Init();
}

void loop()
{
    // MPU6050_Data();
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    const char *properties[] = {"Angle:", "AccelX:", "AccelY:", "AccelZ:", "GyroX:", "GyroY:", "GyroZ:"};
    int values[] = {getBendAngle(32), a.acceleration.x, a.acceleration.y, a.acceleration.z, g.gyro.x, g.gyro.y, g.gyro.z};

    reportDeviceValues("esp32", properties, values, sizeof(values) / sizeof(values[0]));
    delay(1000);
}