#include "06IRControl.h"

u32 hw_jsm;	  //定义一个32位数据变量，保存接收码
u8  hw_jsbz;  //定义一个8位数据的变量，用于指示接收标志
u8 KeyCnt=0;
u8 key_val=0;
u16 IRValue;

//红外初始化
void IrInit(void)
{
	IT0=1;//下降沿触发
	EX0=1;//打开中断0允许
	EA=1;	//打开总中断

	IRIN=1;//初始化端口
}

/*******************************************************************************
* 函 数 名         : HW_jssj
* 函数功能		   : 高电平持续时间，将记录的时间保存在t中返回，其中一次大约20us 
* 输    入         : 无
* 输    出         : t
*******************************************************************************/
u8 HW_jssj(void)
{
	u8 t=0;
	while(IRIN)//高电平
	{
		t++;
		delay10us();
		if(t>=250) return t;//超时溢出
	}
	return t;
}

/*
 * 帧数据有4个字节，第一个字节是遥控的ID，第二个字节是第一个字节的反码
 * 第三个数据是遥控的真正的键值，第四个字节是第三个字节的反码
 */
u8 GetIRData(void)
{
	u8 first_byte, sec_byte, tir_byte, fou_byte;  
	
	first_byte = hw_jsm >> 24;
	sec_byte = (hw_jsm>>16) & 0xff;
	tir_byte = hw_jsm >> 8;
	fou_byte = hw_jsm;
	
	if((first_byte==(~sec_byte)))
	{
		if(tir_byte ==(~fou_byte) )
	  		return tir_byte;
	}
	return 0;   /* 错误返回 */
}

u8 GetIRCount(){
	return KeyCnt;
}

void ClearIRData(){
	hw_jsm=0x00;
}

void delay20us(void)   //误差 0us
{
    unsigned char a,b;
    for(b=3;b>0;b--)
        for(a=1;a>0;a--);
}


//读取红外数值的中断函数
void ReadIr() interrupt 0
{
	u8 Tim=0,Ok=0,hw_Data=0;

	while(1)
	{
		if(IRIN)
		{
			IRValue=0;
			 Tim=HW_jssj();//获得此次高电平时间

			 if(Tim>=250)break;//不是有用的信号	
			 if(Tim>=200 && Tim<250)	/* 获得前导位 4ms~4.5ms */
			 {
			 	Ok=1;//收到起始信号
			 	KeyCnt = 1;
			 }
			 else if(Tim>=10 && Tim<50)		/* 0.56ms: 0.2ms~1ms */
			 {
			 	hw_Data=0;//收到数据 0
			 }
			 else if(Tim>=50 && Tim<100)	/* 1.68ms：1ms~2ms */
			 {
			 	hw_Data=1;//收到数据 1
			 }
			 else if(Tim>=100 && Tim<200)	/* 2.1ms：2ms~4ms *///连发码，在第二次中断出现
			 {
				hw_jsbz=1;
				KeyCnt++;  /* 按键次数加1 */
				break;
			 }

			 if(Ok==1)
			 {
			 	hw_jsm<<=1;
				hw_jsm+=hw_Data;
			 }
		}
	}			
}

