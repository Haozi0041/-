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
uint lock_stata = 0;//����״̬��1Ϊ������0Ϊ����

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
	TMOD = 0x01;		//��һ��������TMOD		//  0000 0001 �� ѡ��ʽ1
	TH0 = 0xfc;
	TL0 = 0x18;		// �ڶ���������ֵ 		//  0xfc18 = 64536.  ��ʱ1000us = 1ms ��500HZ��
	TR0 = 1;			//������������TCON		// ��ʱ��ʽ��ѡ��ʱ�� T0
	ET0 = 1;									// ��ʱ��/������0�ж�����λ����T0�ж�
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

// ��ʱ����
void delay(uint z) {
    uint i, j;
    for (i = z; i > 0; i--)
        for (j = 110; j > 0; j--);
}

// ��ⰴ�������ذ������
uchar keyscan() {
    uchar i, j;
    for (i = 0; i < 4; i++) {
        P0 = ~(1 << i); // ��P0�ڵ�ĳһλ�õ͵�ƽ
        for (j = 0; j < 4; j++) {
            if (!(P0 & (0x10 << j))) { // ����Ƿ��а�������
                delay(20); // ����
                if (!(P0 & (0x10 << j))) { // �ٴμ�ⰴ��״̬
                    while (!(P0 & (0x10 << j))); // �ȴ������ͷ�
                    return j * 4 + i; // ���ذ�����ţ��ȴ������ٴ��ϵ���
                }
            }
        }
    }
    P0 = 0x0f; // ��P0�ڵĵ���λ�øߵ�ƽ
    return 0xFF; // û�а�������
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
uint res =0;//0��ͨ��  1ͨ��
uint set_mode = 0; //0Ϊ����ģʽ  1Ϊ����ģʽ
uint Safe_mode = 0;//0Ϊ����ģʽ��1Ϊ��ȫģʽ
uint off_switch = 0;//0Ϊ����ģʽ��1Ϊ�ر�ģʽ
uint time = 11000;//����ʱ��
uint Count = 11000;//����ʱ�����

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

void confirm()    //ȷ��������
{
	uint i = 0;
	if(set_mode==0)  //����ȷ��ģʽ
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
			LcdPrintStr("wrong password!         ");	// ��1����ʾ
			if(wrongtimes==3)
			{
				lock_stata = 1;
				LcdGotoXY(1,0);
				LcdPrintStr("Locking........");	// ��1����ʾ
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
			LcdPrintStr("passed!               ");	// ��1����ʾ
			passwordLength=0;
			switch_ = 1;//�̵�����
		}
	}
	
	if(set_mode==1)    //��������
	{
		for(i =0;i<15;i++)
		{
			password[i]=Input_password[i];
		}
		set_mode=0;//�˳�����ģʽ
		res=0;//�˳���ȷ����״̬
		Set_password();
		LcdGotoXY(1,0);
		LcdPrintStr("Reload Successfully!      ");	// ��1����ʾ
		DelayMs(5000);
		LcdShowInit();
		switch_= 0;//�̵�����
		passwordLength = 0;
	}
	

}
	
void del()    //�˸�
{
	Input_password[passwordLength-1]=0;
	LcdGotoXY(0,passwordLength-1);
	LcdPrintStr(" ");
}

void Setcode()     //���������� ����Ҫ����ȷ����������������ã�������Ч
{
	if(res ==1)
	{
		LcdGotoXY(0,0);
		LcdPrintStr("                    ");	// ��1����ʾ
		LcdGotoXY(1,0);
		LcdPrintStr("Setting......               ");	// ��1����ʾ
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
// ������
/*********************************************************/


void main() 
{
	LcdInit();								// Һ�����ܳ�ʼ��	
	LcdShowInit();
	password_Init();
	//Set_Init_password();
	switch_ = 0;//�̵�����

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
				
					case 10://ȷ��
						confirm();
						
					break;

					case 11: //�˸�
						del();
						passwordLength-=1;
					break;
					
					case 12: //����������
					Setcode();
					Clear_password();
					break;
					case 13://�ر�������
					res =0 ;
					set_mode=0;
					passwordLength=0;
					switch_ = 0;//�̵�����
					LcdShowInit();
					break;


					case 14://��ȫģʽ�����������벻�ɼ���ʽ
					Safe_mode = 1-Safe_mode;
					Safe_code();
					break;
					
					case 15: //ǿ�����߿���
					off_switch = 1-off_switch;
					Sleep();
					break;

					default : //����������ʾ
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