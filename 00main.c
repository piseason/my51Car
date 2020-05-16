#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library	
#include "01public.h"
#include "02st7565.h"
//#include "03lcd1602.h"
#include "04mpu6050.h"
#include "05car.h"
#include "06IRControl.h"

u16 fPWM = 100;	//100Hz
//TU=100us
u16 THInit=0xFF, TLInit=0x9C, T=1;	//100us
//u16 THInit=0xFC, TLInit=0x18, T=10;	//1ms
//u16 THInit=0xD8, TLInit=0xF0, T=100;	//10ms
u8 timer1,maxCount,valueL,valueR,minVal=10,maxVal=50;
//maxCount=1e4/fPWM/T
u8 IRState,carState=FORWARD_CAR,defaultState=STOP_CAR;



//定时器0初始化
void Time0_Init(void)
{
	TMOD|=0x01;
	TH0 = THInit;
	TL0 = TLInit;	//定时0.1ms
	
	ET0 = 1;
	EA  = 1;
	TR0 = 1;
}

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
	bit DIR=0;
	timer1=0;maxCount = 10000/fPWM/T;
	delay(500);		//上电延时		
	Lcd12864_Init();		//液晶初始化
	Lcd12864_ClearScreen();
	Timer1Init();

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
//通过陀螺仪的角速度调整左右电机的转速来简单控制小车走直线
//*********************************************************
void PWMwithMPU6050()
{	
	bit DIR=1;
	u16 tmp=0;
	int accAlfz;
	
	delay(500);		//上电延时		
	Lcd12864_Init();		//液晶初始化
	Lcd12864_ClearScreen();
	InitMPU6050();
	delay(150);
	timer1=0;maxCount = 10000/fPWM/T;
	Timer1Init();

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
//红外避障
//*********************************************************
void IRAvoiding()	//Infrared obstacle avoidance, 红外避障
{	
	bit DIR=1;
	u16 tmp=0;
	int accAlfz;
	
	delay(500);		//上电延时		
	Lcd12864_Init();		//液晶初始化
	Lcd12864_ClearScreen();
	InitMPU6050();
	delay(150);
	timer1=0;maxCount = 10000/fPWM/T;
	Timer1Init();

	valueL=maxCount/10;valueR=maxCount/10+5;
	Lcd12864_Writeu16(valueL, 16, 0);Lcd12864_Writeu16(valueR, 64, 0);
	
	while(1){
		tmp++;
		if(tmp%3==0){
			accAlfz=GetData(GYRO_ZOUT_H);	//默认为-4, -4说明低8位全为1, 是合成数据时考虑得不严谨
			if(accAlfz>7 && valueL>10){			//前进->右转
				if(carState&FORWARD_CAR) valueL--;
				if(carState&BACKWARD_CAR) valueR--;
			}
			if(accAlfz<-7 && valueR>15){			//前进->左转
				if(carState&FORWARD_CAR) valueR--;
				if(carState&BACKWARD_CAR) valueL--;
			}
			if(valueL>maxVal) DIR=0;
			if(valueL<minVal) DIR=1;
			Lcd12864_Writeu16(carState,16,2);
			Lcd12864_Writeu16(accAlfz,56,2);
			Lcd12864_Writeu16(valueL, 16, 0);
			Lcd12864_Writeu16(valueR, 64, 0);
		}
		
		if(tmp>5){
			tmp=0;
			if(DIR==1){
			  if(valueL<maxVal) valueL+=1;
				if(valueR<maxVal) valueR+=1;
			}			
			if(DIR==0){
			  if(valueL>minVal) valueL-=1;
				if(valueR>minVal) valueR-=1;
			}
			Lcd12864_Writeu16(valueL, 16, 0);
			Lcd12864_Writeu16(valueR, 64, 0);
		}
		
		if(Forward_In==1)	//壁障红外传感器未检测到障碍物--前进
		{
			carState = FORWARD_CAR;
		}
		else			  //壁障红外传感器检测到障碍物--后退，右转
		{
			carState = BACKWARD_CAR;
			delay(100);
			carState = RIGHT_CAR;
			delay(100);
		}
		
	}	
}
//*********************************************************
//测试红外遥控
//*********************************************************
void testIR()
{
	u8 key_count,key_val=0;
	IrInit();
	Timer1Init();
	
	while(1)
	{
		key_count=GetIRCount();
		key_val=GetIRData();

		DisplayData[0]=smgduan[key_val/16];
		DisplayData[1]=smgduan[key_val%16];
		DisplayData[2]=0x40;
		DisplayData[3]=smgduan[key_count%16];

		// DisplayData[0]=smgduan[IRValue>>12&0x000F];
		// DisplayData[1]=smgduan[IRValue>>8&0x000F];
		// DisplayData[2]=smgduan[IRValue>>4&0x000F];
		// DisplayData[3]=smgduan[IRValue&0x000F];
		DigDisplay();
		if (IRValue&0x0800) ClearIRData();
	}	

}

//*********************************************************
//红外遥控小车
//*********************************************************
void IRAControl()
{	
	u8 tmp,step,key_val;
	int accAlfz;
	
	step=5;
	while(1){

		tmp = KeyScan();
		if(tmp&1)	maxVal+=step;
		else if(tmp&2) step=15-step;
		else if(tmp&4) maxVal-=step;
		else if(tmp&8) break;
		tmp=maxVal/100;
		DisplayData[0]=tmp?smgduan[tmp]:0x00;
		DisplayData[1]=0x40;
		DisplayData[2]=smgduan[maxVal%100/10];
		DisplayData[3]=smgduan[maxVal%10];
		DigDisplay();	
	}
	
	delay(500);		//上电延时		
	InitMPU6050();
	delay(150);
	timer1=0;maxCount = 10000/fPWM/T;
	Timer1Init();
	IrInit();

	valueL=maxCount/10;valueR=maxCount/10+5;	
	while(1){
		tmp++;
		if(tmp%3==0){
			accAlfz=GetData(GYRO_ZOUT_H);	//默认为-4, -4说明低8位全为1, 是合成数据时考虑得不严谨
			if(accAlfz>7 && valueL>minVal){			//前进->右转
				if(carState&FORWARD_CAR) valueL--;
				if(carState&BACKWARD_CAR) valueR--;
			}
			if(accAlfz<-7 && valueR>(minVal+5)){			//前进->左转
				if(carState&FORWARD_CAR) valueR--;
				if(carState&BACKWARD_CAR) valueL--;
			}
		}
		
		if(tmp>5){
			tmp=0;
			if(valueL<maxVal) valueL+=1;
			if(valueR<maxVal) valueR+=1;	
		}
		
		
		key_val=GetIRData();
		switch(key_val)
		{
			case(0x18):
				IRState = FORWARD_CAR; 		break;
			case(0x4A):
				IRState = BACKWARD_CAR; 	break;
			case(0x38):
				IRState = STOP_CAR;				break;
			case(0x10):
				IRState = LEFT_CAR;				break;	
			case(0x5A):
				IRState = RIGHT_CAR;			break;	
			case(0xE2):
				defaultState = STOP_CAR;				break;
			case(0x98):
				defaultState = FORWARD_CAR;			break;
		}
		if(Forward_In==1)	//壁障红外传感器未检测到障碍物--受控
		{
			if (key_val) carState = IRState;
			else carState = defaultState;
		}
		else			  //壁障红外传感器检测到障碍物--后退，右转
		{
			carState = BACKWARD_CAR;
			delay(100);
			carState = RIGHT_CAR;
			delay(100);
			carState = defaultState;
		}
		DisplayData[0]=0;
		DisplayData[1]=0;
		DisplayData[2]=smgduan[key_val/16];
		DisplayData[3]=smgduan[key_val%16];

		// DisplayData[0]=smgduan[IRValue>>12&0x000F];
		// DisplayData[1]=smgduan[IRValue>>8&0x000F];
		// DisplayData[2]=smgduan[IRValue>>4&0x000F];
		// DisplayData[3]=smgduan[IRValue&0x000F];
		DigDisplay();
		if (IRValue&0x0010) ClearIRData();
	}	
}

//*********************************************************
//主程序
//*********************************************************
void main()
{ 
	//testMPU6050();
	//testPWM();
	//PWMwithMPU6050();
	//IRAvoiding();
	//testIR();
	IRAControl();
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
	
	if(timer1>maxCount) {timer1=0;IRValue++;}	
	if(timer1 <valueL){
		if(carState&(FORWARD_CAR|RIGHT_CAR)) Car_ForwardRunL();
		if(carState&BACKWARD_CAR) Car_BackwardRunL();
	}
	else{
		Car_StopRunL();
	}	
	if(timer1 <valueR){
		if(carState&(FORWARD_CAR|LEFT_CAR)) Car_ForwardRunR();
		if(carState&BACKWARD_CAR) Car_BackwardRunR();
	}
	else{
		Car_StopRunR();
	}	
	
}

