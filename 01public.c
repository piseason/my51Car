#include "01public.h"

u8 KeyValue=0;
u8 DisplayData[4];
//0、1、2、3、4、5、6、7、8、9、A、b、C、d、E、F、H的显示码
u8 smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0X76};

/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void delay_us(u16 i)
{
	while(i--);	
}

//****************************************
//延时
//****************************************
void delay(unsigned int k)	
{						
	unsigned int i,j;				
	for(i=0;i<k;i++)
	{			
		for(j=0;j<121;j++);
	}						
}

//**************************************
//延时5微秒(STC90C52RC@12M)
//不同的工作环境,需要调整此函数
//当改用1T的MCU时,请调整此延时函数
//**************************************
void Delay5us()
{
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}

void delay10us(void)   //误差 0us
{
    unsigned char a,b;
    for(b=1;b>0;b--)
        for(a=2;a>0;a--);
}

//数码管动态显示函数
void DigDisplay(void)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=0;LSB=1;LSC=1;LSD=1; break;//显示第0位
			case(1):
				LSA=1;LSB=0;LSC=1;LSD=1; break;//显示第1位
			case(2):
				LSA=1;LSB=1;LSC=0;LSD=1; break;//显示第2位
			case(3):
				LSA=1;LSB=1;LSC=1;LSD=0; break;//显示第3位	
		}
		SMG_PORT=DisplayData[i];//发送段码
		delay(1); //间隔一段时间扫描	
		SMG_PORT=0x00;//消隐
	}
}

u8 KeyScan()
{
	u8 tmp=0;
	if(KEY1==0||KEY2==0||KEY3==0||KEY4==0)
	{
		delay_us(1000);  //消抖处理
		if(KEY1==0)	tmp=0x01;
		else if(KEY2==0) tmp=0x02;
		else if(KEY3==0) tmp=0x04;
		else if(KEY4==0) tmp=0x08;
	}
	while(KEY1==0||KEY2==0||KEY3==0||KEY4==0);
	return tmp;
}