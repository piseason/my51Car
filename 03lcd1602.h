#ifndef __LCD1602_H
#define __LCD1602_H

#include "01public.h"

//--定时使用的IO口--//
#define DataPort P0		//LCD1602数据端口
sbit    LCM_RS=P2^6;		//LCD1602命令端口		
sbit    LCM_RW=P2^5;		//LCD1602命令端口		
sbit    LCM_EN=P2^7;		//LCD1602命令端口 


void  InitLcd();								//初始化lcd1602
void  lcd_printf(uchar *s,int temp_data);
void  WriteDataLCM(uchar dataW);						//LCD数据
void  WriteCommandLCM(uchar CMD,uchar Attribc);				//LCD指令
void  DisplayOneChar(uchar X,uchar Y,uchar DData);			//显示一个字符
void  DisplayListChar(uchar X,uchar Y,uchar *DData,L);	//显示字符串
void WaitForEnable(void);
void Display10BitData(int value,uchar x,uchar y);


#endif