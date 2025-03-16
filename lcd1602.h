#include <STC12C5A60S2.H>

#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int


sbit LcdRs_P   = P3^5;			// 1602液晶的RS管脚       
sbit LcdRw_P   = P3^6;			// 1602液晶的RW管脚 
sbit LcdEn_P   = P3^7;			// 1602液晶的EN管脚


void DelayMs(uint time);

void LcdWriteCmd(uchar cmd);
void LcdWriteData(uchar dat);
void LcdGotoXY(uchar line,uchar column);
void LcdPrintStr(uchar *str);
void LcdInit();
void LcdOn() ;
void LcdOff() ;
void LcdShowInit();
void display_key_on_lcd(unsigned char key);