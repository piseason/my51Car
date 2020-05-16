#ifndef __carControl_H
#define __carControl_H

#include "01public.h"

//定义小车状态
#define	FORWARD_CAR		0x01
#define	BACKWARD_CAR	0x02
#define	LEFT_CAR			0x04
#define	RIGHT_CAR			0x08
#define	STOP_CAR			0x00

//定义红外壁障感应控制管脚
sbit Forward_In=P2^1;

//左侧电机优先启动，导致低电压下前进偏右，后退偏左
sbit PosR=P1^0;
sbit GndR=P1^1; //右侧电机, 占空比min:15%
sbit PosL=P1^2;
sbit GndL=P1^3; //左侧电机, 占空比min:10%


void Car_ForwardRunL(void);//前进L
void Car_ForwardRunR(void);//前进R
void Car_BackwardRunL(void);//后退L
void Car_BackwardRunR(void);//后退R
void Car_StopRunL(void);//停止L
void Car_StopRunR(void);//停止R


#endif