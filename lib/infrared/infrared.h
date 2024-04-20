#ifndef _INFRARED_H
#define _INFRARED_H

/*

想要实现的功能：
内码1：打开空调
内码2：关闭空调
内码3：热风
内码4：冷风
内码5：升高温度
内码6：降低温度
注意：空调的升温不是一个固定的值，而是根据当前温度来调整的，所以升温和降温的内码是不同的

华为云平台下发命令：

Air_Conditioner_On，红外学习模块出发内码发送1 打开空调
Air_Conditioner_Off，红外学习模块出发内码发送2 关闭空调

Air_Conditioner_Hot，红外学习模块出发内码发送3 制热模式
Air_Conditioner_Cold，红外学习模块出发内码发送4 制冷模式

# Air_Conditioner_Raise，红外学习模块出发内码发送5 升高温度
Air_Conditioner_27C，红外学习模块出发内码发送5 调整温度到27度

# Air_Conditioner_Rduce，红外学习模块出发内码发送6 降低温度
Air_Conditioner_26C，红外学习模块出发内码发送6 调整温度到26度

Air_Conditioner_Study7，红外学习模块出发内码7学习 自由模式
Air_Conditioner_Send7，红外学习模块出发内码7发送 自由模式


*/
#include <Arduino.h>

// 红外内码学习
byte IrStudy_data(byte *data, byte group);

// 红外内码发送
byte IrSend_data(byte *data, byte group);

//求校验和
byte getSum(byte *data, byte len);


/*
控制示例：
byte data[32], len;
void setup()
{
    Serial.begin(115200);
    // RXPIN 16  TXPIN 17
    Serial1.begin(115200, SERIAL_8N1, 16, 17);
}

void loop()
{
    len = IrSend_data(data, 0);
    Serial1.write(data, len);
    // 输出buf
    Serial.print("buf: ");
    for (int i = 0; i < len; i++)
    {
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }

    delay(2000); // 等待2秒
}
*/

#endif // !_INFRARED_H
