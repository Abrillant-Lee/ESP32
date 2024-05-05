#include "flex.h"

// 获取弯曲角度的函数
// int getBendAngle(int sensorPin)
// {
//     // 从指定的引脚读取弯曲传感器的模拟值
//     int flexSensorReading = analogRead(sensorPin);
//     // 在串口监视器上打印出模拟值
//     // Serial.println(flexSensorReading);

//     // 使用map函数将模拟值映射到0到100的范围
//     // 当模拟值在512到614之间时，映射后的值在0到100之间
//     // 如果模拟值小于512或大于614，映射后的值可能小于0或大于100
//     // int flex0to100 = map(flexSensorReading, 35, 153, 0, 90);
//     int flex0to100 = map(flexSensorReading, 153, 35, 90, 0);

//     // 在串口监视器上打印出映射后的值
//     Serial.println(90- flex0to100);

//     // 延迟250毫秒
//     delay(250);

//     // 返回映射后的值
//     return flex0to100;
// }
// 获取弯曲角度的函数
int getBendAngle(int sensorPin)
{
    const int numReadings = 10;
    static int readings[numReadings]; // 存储最近的读数
    static int readIndex = 0;         // 下一个读数的索引
    static int total = 0;             // 读数的总和
    static int averageReading = 0;    // 平均读数

    // 从指定的引脚读取弯曲传感器的模拟值
    int flexSensorReading = analogRead(sensorPin);

    // 从总和中减去最旧的读数
    total -= readings[readIndex];
    // 读取新的读数
    readings[readIndex] = flexSensorReading;
    // 将新的读数添加到总和中
    total += readings[readIndex];
    // 移动到下一个位置
    readIndex = (readIndex + 1) % numReadings;
    // 计算平均值
    averageReading = total / numReadings;

    // 使用map函数将模拟值映射到0到100的范围
    int flex0to100 = map(averageReading, 153, 35, 90, 0);


    int angle = 90 - flex0to100;

    // 返回映射后的值
    return angle;
}
