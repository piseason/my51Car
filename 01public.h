#ifndef __myPUBLIC_H
#define __myPUBLIC_H

#include <REG52.H>	
#include <intrins.h>

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

typedef unsigned char  u8;
typedef unsigned int   u16;
typedef unsigned long  u32;

//定义数码管段选和位选控制管脚
#define SMG_PORT	P0
sbit LSA=P2^0;
sbit LSB=P2^1;
sbit LSC=P2^2;
sbit LSD=P2^3;

//定义按键控制管脚
sbit KEY1=P1^4;
sbit KEY2=P1^5;
sbit KEY3=P1^6;
sbit KEY4=P1^7;

extern u8 KeyValue;
extern u8 DisplayData[4];
extern u8 smgduan[17];
extern u16 IRValue;

void  delay_us(u16 i);
void  delay(unsigned int k);										//延时
void  Delay5us();
void delay10us(void);
void DigDisplay(void);
u8 KeyScan();

#endif
