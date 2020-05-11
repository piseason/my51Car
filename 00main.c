#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library	
#include "01public.h"
#include "02st7565.h"
//#include "03lcd1602.h"
#include "04mpu6050.h"

u16 fPWM = 100;	//100Hz
//TU=100us
u16 THInit=0xFF, TLInit=0x9C, T=1;	//100us
//u16 THInit=0xFC, TLInit=0x18, T=10;	//1ms
//u16 THInit=0xD8, TLInit=0xF0, T=100;	//10ms
int timer1,maxCount,valueL,valueR;
//maxCount=1e4/fPWM/T

//左侧电机优先启动，导致低电压下前进偏右，后退偏左
sbit PosR=P1^0;
sbit GndR=P1^1; //右侧电机, 占空比min:15%
sbit PosL=P1^2;
sbit GndL=P1^3; //左侧电机, 占空比min:10%
bit DIR;

/*******************************************************************************
* 函 数 名         : Timer1Init
* 函数功能		   : 定时器1初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Timer1Init()
{
	TMOD|=0x10;//选择为定时器1模式，工作方式1，仅用TR1打开启动。

	TH1 = THInit; 
	TL1 = TLInit;
		
	ET1=1;//打开定时器1中断允许
	EA=1;//打开总中断
	TR1=1;//打开定时器
}

//*********************************************************
//显示加速度计和陀螺仪的10位原始数据
//*********************************************************
void testMPU6050()
{ 
	delay(500);		//上电延时		
	Lcd12864_Init();		//液晶初始化
	Lcd12864_ClearScreen();
	InitMPU6050();	//初始化MPU6050
	delay(150);
	while(1)
	{
		//Lcd12864_WriteListChar(2, 0, 'A:', 2);
		Lcd12864_Write10BitData(GetData(ACCEL_XOUT_H), 16, 0);	//显示X轴加速度
		Lcd12864_Write10BitData(GetData(ACCEL_YOUT_H), 56, 0);	//显示Y轴加速度
		Lcd12864_Write10BitData(GetData(ACCEL_ZOUT_H), 96, 0);	//显示Z轴加速度
		Lcd12864_Write10BitData(GetData(GYRO_XOUT_H),16,2);	//显示X轴角速度
		Lcd12864_Write10BitData(GetData(GYRO_YOUT_H),56,2);	//显示Y轴角速度
		Lcd12864_Write10BitData(GetData(GYRO_ZOUT_H),96,2);	//显示Z轴角速度
		//Display10BitData(GetData(GYRO_ZOUT_H),12,1);	//显示Z轴角速度
		delay(500);
		
	}
}
//*********************************************************
//展示PWM占空比对速度的影响
//*********************************************************
void testPWM()
{	
	timer1=0;maxCount = 10000/fPWM/T;
	delay(500);		//上电延时		
	Lcd12864_Init();		//液晶初始化
	Lcd12864_ClearScreen();
	Timer1Init();

	DIR=0;
	valueL=maxCount/20;valueR=maxCount/20+5;
	while(1){

		if(valueL>maxCount/3) DIR=0;
		if(valueL<maxCount/7) DIR=1;
		if(DIR==1){		//电机转速先由慢到快，再由快到慢
			valueL+=5;
			valueR+=5;
		}			
		if(DIR==0){
			valueL-=5;
			valueR-=5;
		}
		Lcd12864_Writeu16(valueL, 16, 0);
		Lcd12864_Writeu16(valueR, 64, 0);
		delay(500);
	}		
}

//*********************************************************
//通过陀螺仪的角加速度调整左右电机的转速来简单控制小车走直线
//*********************************************************
void PWMwithMPU6050()
{	
	u16 tmp=0;
	int accAlfz;
	
	delay(500);		//上电延时		
	Lcd12864_Init();		//液晶初始化
	Lcd12864_ClearScreen();
	InitMPU6050();
	delay(150);
	timer1=0;maxCount = 10000/fPWM/T;
	Timer1Init();

	DIR=1;
	valueL=maxCount/2;valueR=maxCount/20;
	Lcd12864_Writeu16(valueL, 16, 0);Lcd12864_Writeu16(valueR, 64, 0);
	
	while(1){
		tmp++;
		if(tmp%5==0){
			accAlfz=GetData(GYRO_ZOUT_H)/64;	//默认为-4
			if(accAlfz<-5 && valueL>10){			//正在右转
				valueL--;
			}
			if(accAlfz>-3 && valueR>15){			//正在左转
				valueR--;
			}
			if(valueL>maxCount/3) DIR=0;
			if(valueL<maxCount/7) DIR=1;
			Lcd12864_Writeu16(accAlfz,56,2);
		}
		
		if(tmp>50){
			tmp=0;
			if(DIR==1){
			  if(valueL<maxCount/3) valueL+=5;
				if(valueR<maxCount/3) valueR+=5;
			}			
			if(DIR==0){
			  if(valueL>maxCount/7) valueL-=5;
				if(valueR>maxCount/7) valueR-=5;
			}
			Lcd12864_Writeu16(valueL, 16, 0);
			Lcd12864_Writeu16(valueR, 64, 0);
		}
	}	
}

//*********************************************************
//主程序
//*********************************************************
void main()
{ 
	//testMPU6050();
	//testPWM();
	PWMwithMPU6050();
}

/*******************************************************************************
* 函 数 名       : Time1
* 函数功能		 : 定时器1的中断函数
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
void Time1(void) interrupt 3    //3 为定时器1的中断号  1 定时器0的中断号 0 外部中断1 2 外部中断2  4 串口中断
{
	TH1 = THInit; 	//2~4us
	TL1 = TLInit;   //2~4us
	timer1++; 		//4~10us
	
	if(timer1>maxCount) timer1=0;	
	if(timer1 <valueL){
		PosL=1;
		GndL=0;
	}
	else{
		PosL=0;
		GndL=0;
	}	
	if(timer1 <valueR){
		PosR=1;
		GndR=0;
	}
	else{
		PosR=0;
		GndR=0;
	}	
	
}
