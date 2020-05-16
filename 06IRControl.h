#ifndef __IRControl_H
#define __IRControl_H

#include "01public.h"

//使用红外遥控器可控制小车运行，同时数码管显示对应键编码和连按次数。
//          按下红外遥控器的“Mode”或“2”键可使小车前进
//          按下红外遥控器的“VOL-”或“8”键可使小车后退
//          按下红外遥控器的“>||”或“4”键可使小车左转
//          按下红外遥控器的“>>|”或“6”键可使小车右转
//注意：将循迹模块上的短接片J32/J33/J34拔掉。

//定义红外接收头管脚
sbit IRIN=P3^2;

//定义红外接收数据值
#define FORWARD_1_PRESS		0X62
#define BACK_1_PRESS		0XA8
#define LEFT_1_PRESS		0X22
#define RIGHT_1_PRESS		0XC2
#define STOP_1_PRESS		0X02

#define FORWARD_2_PRESS		0X18
#define BACK_2_PRESS		0X4A
#define LEFT_2_PRESS		0X10
#define RIGHT_2_PRESS		0X5A
#define STOP_2_PRESS		0X38


void IrInit(void);
u8 HW_jssj(void);
u8 GetIRCount();
u8 GetIRData(void);
void ClearIRData();
	

#endif