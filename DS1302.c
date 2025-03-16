#include <STC12C5A60S2.H>

//引脚定义；
sbit  DS1302_SCLK=P3^3;
sbit  DS1302_CE=P3^2;
sbit  DS1302_IO=P3^1;

//对应寄存器位置定义
#define  DS1302_SECOND		0X80
#define  DS1302_MINUTE  	0X82
#define  DS1302_HOUR	    0X84
#define  DS1302_DATE	    0X86
#define  DS1302_MONTH       0X88
#define  DS1302_DAY         0X8A
#define  DS1302_YEAR        0X8C
#define  DS1302_WP          0X8E 

//定义数组，用于存放设定的时间，年月日时分秒星期
unsigned char DS1302_Time[]={23,10,28,19,03,55,6};//顺序：年月日时分秒星期
/**
*@breaf DS1302初始化
*@param无
*@retval无
*/
void  DS1302_Init()
{
	DS1302_CE = 0; // 将使能位置0，低电平；
	DS1302_SCLK=0;//将时钟位置0，低电平；
}

/**
*@breaf DS1302单字节写入函数
*@param command：写入控制指令的指令，包含要写入寄存器的地址；
*@param Data：将要写入的数据内容；
*@retval 无
*/
void  DS1302_WriteBety(unsigned char command, Data)
{
	unsigned char i;
	DS1302_CE = 1;//使能位置高电平；
	for (i = 0; i<8; i++)//控制寄存器数据需要通过IO线一个一个写入控制寄存器;低位先写入
	{
		DS1302_IO = command&(0x01 << i);//相当于把第1--7位置0，只留第0位，如果第0位是0，则为0；反之则为1；
		DS1302_SCLK = 1;
		DS1302_SCLK = 0;
	}

	for (i = 0; i<8; i++)//数据写入
	{
		DS1302_IO = Data&(0x01 << i);//相当于把第1--7位置0，只留第0位，如果第0位是0，则为0；反之则为1；
		DS1302_SCLK = 1;
		DS1302_SCLK = 0;
	}
	DS1302_CE = 0;
}

/**
*@breaf  DS1302单字节读出函数
*@param command：写入控制指令的指令，包含要读出寄存器的地址；
*@retval Data：读出的数据；
*/
unsigned char   DS1302_ReadBety(unsigned char command)
{
	unsigned i, Data = 0X00;
	command |= 0X01;//写入指令与读出指令只在最后一位相差1，故在此利用或运算消除；
	DS1302_CE = 1;//使能位置高电平；
	for (i = 0; i<8; i++)//控制寄存器数据需要通过IO线一个一个写入控制寄存器;低位先写入
	{
		DS1302_IO = command&(0x01 << i);//相当于把第1--7位置0，只留第0位，如果第0位是0，则为0；反之则为1；
		DS1302_SCLK = 0;
		DS1302_SCLK = 1;
	}
	DS1302_IO = 0;
	for (i = 0; i<8; i++)//数据读出
	{
		DS1302_SCLK = 1;
		DS1302_SCLK = 0;
		if (DS1302_IO)
		{
			Data |= (0X01 << i);
		}
	}
	DS1302_CE = 0;
	return Data;
}

/**
*@breaf 向DS1302内设定时间
*@param无
*@retval无
*/
void DS1302_SetTime()
{
	DS1302_WriteBety(DS1302_WP, 0x00);//操作 DS1302 之前，关闭写保护，不然指令无法进入控制寄存器；
	// DS1302_WriteBety(DS1302_YEAR, DS1302_Time[0] / 10 * 16 + DS1302_Time[0] % 10);//写入年，并将10进制转化BCD码；
	// DS1302_WriteBety(DS1302_MONTH, DS1302_Time[1] / 10 * 16 + DS1302_Time[1] % 10);//写入月，并将10进制转化BCD码；
	// DS1302_WriteBety(DS1302_DATE, DS1302_Time[2] / 10 * 16 + DS1302_Time[2] % 10);//写入日，并将10进制转化BCD码；
	// DS1302_WriteBety(DS1302_HOUR, DS1302_Time[3] / 10 * 16 + DS1302_Time[3] % 10);//写入时，并将10进制转化BCD码；
	DS1302_WriteBety(DS1302_MINUTE, DS1302_Time[4] / 10 * 16 + DS1302_Time[4] % 10);//写入分，并将10进制转化BCD码；
	DS1302_WriteBety(DS1302_SECOND, DS1302_Time[5] / 10 * 16 + DS1302_Time[5] % 10);//写入秒，并将10进制转化BCD码；
	//DS1302_WriteBety(DS1302_DAY, DS1302_Time[6] / 10 * 16 + DS1302_Time[6] % 10);//写入星期，并将10进制转化BCD码；
	DS1302_WriteBety(DS1302_WP, 0x80);//写入结束，开启写保护；
}

/**
*@breaf 读取DS1302内时间
*@param无
*@retval无
*/void DS1302_ReadTime()
{

    //DS1302_Time[3] = DS1302_ReadBety(DS1302_HOUR);//读取小时BCD码；
    DS1302_Time[4]=DS1302_ReadBety(DS1302_MINUTE);//读取分钟BCD码；
    DS1302_Time[5]=DS1302_ReadBety(DS1302_SECOND);//读取秒BCD码；

}