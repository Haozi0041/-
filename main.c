#include <STC12C5A60S2.H>
#include <stdio.h>
#include <intrins.h>
#include <lcd1602.h>
#include <ICC.h>

//#include <DS1302.h>
#define uchar unsigned char
#define uint unsigned int
	
sbit switch_ = P2^0;
sbit Bee = P2^3;

uint lock_time = 0;
uint lock_stata = 0;//锁定状态，1为锁定，0为正常

void bee()
{
	uint times = 50;
	while(times--)
	{
	Bee=1;
	DelayMs(20);
	Bee=0;
	DelayMs(20);
	}
}

void T0_time_Init()
{
	TMOD = 0x01;		//第一步：配置TMOD		//  0000 0001 ； 选择方式1
	TH0 = 0xfc;
	TL0 = 0x18;		// 第二步：赋初值 		//  0xfc18 = 64536.  定时1000us = 1ms （500HZ）
	TR0 = 1;			//第三步：配置TCON		// 定时方式，选择定时器 T0
	ET0 = 1;									// 定时器/计数器0中断允许位，打开T0中断
	EA = 1;	
}


void timer0() interrupt 1
{
	lock_time++;
	if(lock_time == 2671)//14
	{
		lock_stata = 0;
		LcdShowInit();
		bee();
	}
}

// 延时函数
void delay(uint z) {
    uint i, j;
    for (i = z; i > 0; i--)
        for (j = 110; j > 0; j--);
}

// 检测按键并返回按键编号
uchar keyscan() {
    uchar i, j;
    for (i = 0; i < 4; i++) {
        P0 = ~(1 << i); // 将P0口的某一位置低电平
        for (j = 0; j < 4; j++) {
            if (!(P0 & (0x10 << j))) { // 检测是否有按键按下
                delay(20); // 消抖
                if (!(P0 & (0x10 << j))) { // 再次检测按键状态
                    while (!(P0 & (0x10 << j))); // 等待按键释放
                    return j * 4 + i; // 返回按键编号，先从左到右再从上到下
                }
            }
        }
    }
    P0 = 0x0f; // 将P0口的低四位置高电平
    return 0xFF; // 没有按键按下
}

void Lock()
{
	lock_time =0;
	T0_time_Init();
}




uint password[15]={2,0,2,5,0,0,0,0,0,0,0,0,0,0,0};
uint Input_password[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint passwordLength = 0;
uint wrongtimes = 0;
uint res =0;//0不通过  1通过
uint set_mode = 0; //0为正常模式  1为输入模式
uint Safe_mode = 0;//0为正常模式，1为安全模式
uint off_switch = 0;//0为正常模式，1为关闭模式
uint time = 11000;//休眠时间
uint Count = 11000;//休眠时间计数

void Clear_password()
{
	uint i = 0;
	for(i = 0;i<15;i++)
	{
		Input_password[i] = 0;
		password[i] = 0;
	}
}


void Set_password()
{
	uint i =0;
	for(;i<15;i++)
	{
		uchar num ;
		num = Input_password[i];
		AT24C02_WriteOneByte(i,num);
		Delay_Write();
	}
}

void Set_Init_password()
{
	uint i =0;
	for(;i<15;i++)
	{
		uchar num ;
		num = password[i];
		AT24C02_WriteOneByte(i,num);
		Delay_Write();
	}
}

void password_Init()
{
	uint i =0;
	for(;i<15;i++)
	{
		uchar num = AT24C02_ReadOneByte(i);
		password[i]=num;
	
	}
}

void confirm()    //确认输入结果
{
	uint i = 0;
	if(set_mode==0)  //正常确认模式
	{
		for(i =0;i<15;i++)
		{
		
		if(Input_password[i]!=password[i])
		{
			res = 0;
			break;
		}
		res = 1;
		}

		if(res == 0)
		{
			wrongtimes +=1;
			LcdGotoXY(1,0);
			LcdPrintStr("wrong password!         ");	// 第1行显示
			if(wrongtimes==3)
			{
				lock_stata = 1;
				LcdGotoXY(1,0);
				LcdPrintStr("Locking........");	// 第1行显示
				bee();
				Lock();
				passwordLength = 0;
				wrongtimes=0;
				
			}
			
		}

		if(res==1)
		{
			wrongtimes =0;
			LcdGotoXY(1,0);
			LcdPrintStr("passed!               ");	// 第1行显示
			passwordLength=0;
			switch_ = 1;//继电器开
		}
	}
	
	if(set_mode==1)    //设置密码
	{
		for(i =0;i<15;i++)
		{
			password[i]=Input_password[i];
		}
		set_mode=0;//退出设置模式
		res=0;//退出正确输入状态
		Set_password();
		LcdGotoXY(1,0);
		LcdPrintStr("Reload Successfully!      ");	// 第1行显示
		DelayMs(5000);
		LcdShowInit();
		switch_= 0;//继电器关
		passwordLength = 0;
	}
	

}
	
void del()    //退格
{
	Input_password[passwordLength-1]=0;
	LcdGotoXY(0,passwordLength-1);
	LcdPrintStr(" ");
}

void Setcode()     //设置新密码 ，需要在正确输入密码情况下设置，否则无效
{
	if(res ==1)
	{
		LcdGotoXY(0,0);
		LcdPrintStr("                    ");	// 第1行显示
		LcdGotoXY(1,0);
		LcdPrintStr("Setting......               ");	// 第1行显示
		set_mode = 1;
	}
	else
	{
		set_mode=0;
	}
}

void Safe_code()
{
	uint i = 0 ;
	if(Safe_mode==1)
	{
		for(;i<passwordLength;i++)
		{
			LcdGotoXY(0,i);
			LcdPrintStr("*");
		}
	}
	if(Safe_mode==0)
	{
		
		for(;i<passwordLength;i++)
		{
			LcdGotoXY(0,i);
			display_key_on_lcd(Input_password[i]);
		}
	}
}

void Sleep()
{
	if(off_switch ==1)
	{
		LcdOff();
	}
	if(off_switch ==0)
	{
		LcdOn();
	}

}


/*********************************************************/
// 主函数
/*********************************************************/


void main() 
{
	LcdInit();								// 液晶功能初始化	
	LcdShowInit();
	password_Init();
	//Set_Init_password();
	switch_ = 0;//继电器关

	while(1)
	{
		uchar keynum = keyscan();
		Sleep();
		if(keynum !=0xff)
		{
			off_switch = 0;
			Count = time;
			if(lock_stata ==0)
			{
				switch (keynum)
				{
				
					case 10://确认
						confirm();
						
					break;

					case 11: //退格
						del();
						passwordLength-=1;
					break;
					
					case 12: //设置新密码
					Setcode();
					Clear_password();
					break;
					case 13://关闭密码锁
					res =0 ;
					set_mode=0;
					passwordLength=0;
					switch_ = 0;//继电器关
					LcdShowInit();
					break;


					case 14://安全模式，即输入密码不可见形式
					Safe_mode = 1-Safe_mode;
					Safe_code();
					break;
					
					case 15: //强制休眠开关
					off_switch = 1-off_switch;
					Sleep();
					break;

					default : //按键输入显示
					LcdGotoXY(0,passwordLength);
					if(Safe_mode==1)
					{
						LcdPrintStr("*");
					}
					else
					{
						display_key_on_lcd(keynum);
					}
					Input_password[passwordLength]=keynum;
					passwordLength+=1;
					break;

				}
				if(passwordLength==-1){passwordLength=0;}
				if(passwordLength==17){passwordLength=16;}
			}
			
			
		
		}
		Count--;
		if(Count==0)
		{
			off_switch = 1;
		}
	}
}