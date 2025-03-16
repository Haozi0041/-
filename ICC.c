#include <STC12C5A60S2.H>
#include<ICC.h>
#include <intrins.h>
#include <lcd1602.h>
#define uchar unsigned char
#define uint unsigned int

//数据线
sbit IIC_SDA=P2^1;
//时钟线
sbit IIC_SCL=P2^2;

/*
函数功能: 发送起始信号
当时钟线为高电平的时候，数据线由高电平变为低电平的过程
*/
void IIC_SendStart(void)
{
    uchar i=0;
    IIC_SCL=1;
    IIC_SDA=1;
    IIC_SDA=0;
    IIC_SCL=0; //总线开始工作、开始读写数据
}

/*
函数功能: 停止信号
当时钟线为高电平的时候，数据线由低电平变为高电平的过程
*/
void IIC_SendStop(void)
{
    uchar  i=0;
    IIC_SCL=0;
    IIC_SDA=0;
    IIC_SCL=1;
    IIC_SDA=1;
}

/*
函数功能: 获取应答信号
返 回 值: 0表示获取到应答 1表示没有获取到应答
*/
uchar IIC_GetAck(void)
{
    uchar i=0;
    IIC_SDA=1; //上拉
    IIC_SCL=0;
    IIC_SCL=1;
    while(IIC_SDA)
    {
        i++;
        if(i>250)return 1; //获取不到应答
    }
    IIC_SCL=0;
    return 0;
}

/*
函数功能: 发送应答信号
函数参数：0表示应答 1表示非应答
*/
void IIC_SendAck(uchar ack)
{
    uchar i=0;
    IIC_SCL=0;
    if(ack)IIC_SDA=1; //发送非应答
    else IIC_SDA=0; //发送应答
    IIC_SCL=1;
    IIC_SCL=0;
}

/*
函数功能: 发送一个字节数据
*/
void IIC_SendOneByte(uchar dat)
{
    uchar j=0,i=0;
    for(j=0;j<8;j++)
    {
        IIC_SCL=0;
        if(dat&0x80)IIC_SDA=1;
        else IIC_SDA=0;
        IIC_SCL=1;
        dat<<=1;
    }
    IIC_SCL=0;
}

/*
函数功能: 接收一个字节数据
*/
uchar IIC_RecvOneByte(void)
{
    uchar i=0,j=0;
    uchar dat=0;
    for(j=0;j<8;j++)
    {
        IIC_SCL=0;
        IIC_SCL=1;
        dat<<=1; //表示默认收到0
        if(IIC_SDA)dat|=0x01;
    }
    IIC_SCL=0;
    return dat;
}
/*
函数功能: 写一个字节
函数参数:
    uchar addr 数据的位置(0~255)
    uchar dat 数据范围(0~255)
*/
void AT24C02_WriteOneByte(uint addr,uchar dat)
{
    IIC_SendStart();//起始信号
    IIC_SendOneByte(0xa0);//发送设备地址
    IIC_GetAck();//获取应答
    IIC_SendOneByte(addr); //数据存放的地址
    IIC_GetAck();//获取应答
    IIC_SendOneByte(dat); //发送将要存放的数据
    IIC_GetAck();//获取应答
    IIC_SendStop(); //停止信号
    DelayMs(10); //等待写
}

/*
函数功能: 读一个字节
函数参数:
    uchar addr 数据的位置(0~255)
返回值: 读到的数据
*/
uchar AT24C02_ReadOneByte(uint addr)
{
    uchar dat=0;
    IIC_SendStart();//起始信号
    IIC_SendOneByte(0xa0);//发送设备地址
    IIC_GetAck();//获取应答
    IIC_SendOneByte(addr); //将要读取数据的地址
    IIC_GetAck();//获取应答
    
    IIC_SendStart();//起始信号
    IIC_SendOneByte(0xa1);//发送设备地址 					note：fixed here
    IIC_GetAck();//获取应答
    dat=IIC_RecvOneByte();//读取数据
    IIC_SendAck(1); //发送非应答
    IIC_SendStop(); //停止信号
    return dat;
}



//uchar at24c02_w[8]="1234567";
//uchar at24c02_r[8];

void Delay_Write() {
    unsigned int i;
    for (i = 0; i < 500; i++);  // 简单的延时循环，延时约5ms
}