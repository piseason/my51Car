#ifndef __LCD1602_H
#define __LCD1602_H

#include "01public.h"

//--��ʱʹ�õ�IO��--//
#define DataPort P0		//LCD1602���ݶ˿�
sbit    LCM_RS=P2^6;		//LCD1602����˿�		
sbit    LCM_RW=P2^5;		//LCD1602����˿�		
sbit    LCM_EN=P2^7;		//LCD1602����˿� 


void  InitLcd();								//��ʼ��lcd1602
void  lcd_printf(uchar *s,int temp_data);
void  WriteDataLCM(uchar dataW);						//LCD����
void  WriteCommandLCM(uchar CMD,uchar Attribc);				//LCDָ��
void  DisplayOneChar(uchar X,uchar Y,uchar DData);			//��ʾһ���ַ�
void  DisplayListChar(uchar X,uchar Y,uchar *DData,L);	//��ʾ�ַ���
void WaitForEnable(void);
void Display10BitData(int value,uchar x,uchar y);


#endif