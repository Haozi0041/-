#include <STC12C5A60S2.H>

#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int

void IIC_SendStart(void);
void IIC_SendStop(void);
uchar IIC_GetAck(void);
void IIC_SendAck(uchar ack);
void IIC_SendOneByte(uchar dat);
uchar IIC_RecvOneByte(void);
void AT24C02_WriteOneByte(uint addr,uchar dat);
uchar AT24C02_ReadOneByte(uint addr);
void Delay_Write();