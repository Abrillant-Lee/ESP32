#include "infrared.h"

#define K1 2
#define K1_MASK 0x01
#define K2 3
#define K2_MASK 0x02

byte trg = 0, cont = 0;
// byte buf[32], len;

// 求校验和
byte getSum(byte *data, byte len)
{
  byte i, sum = 0;
  for (i = 0; i < len; i++)
  {
    sum += data[i];
  }
  return sum;
}

// 红外内码学习
byte IrStudy_data(byte *data, byte group)
{
  byte *offset = data, cs;
  // 帧头
  *offset++ = 0x68;
  // 帧长度
  *offset++ = 0x08;
  *offset++ = 0x00;
  // 模块地址
  *offset++ = 0xff;
  // 功能码
  *offset++ = 0x10;
  // 内码索引号，代表第几组
  *offset++ = group;
  cs = getSum(&data[3], offset - data - 3);
  *offset++ = cs;
  *offset++ = 0x16;
  return offset - data;
}

// 红外内码发送
byte IrSend_data(byte *data, byte group)
{
  byte *offset = data, cs;
  // 帧头
  *offset++ = 0x68;
  // 帧长度
  *offset++ = 0x08;
  *offset++ = 0x00;
  // 模块地址
  *offset++ = 0xff;
  // 功能码
  *offset++ = 0x12;
  // 内码索引号，代表第几组
  *offset++ = group;
  // 计算校验和
  cs = getSum(&data[3], offset - data - 3);
  // 设置校验和和帧尾
  *offset++ = cs;
  *offset++ = 0x16;
  // 返回发送的字节数。
  return offset - data;
}
