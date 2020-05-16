#include "05car.h"

void Car_ForwardRunL(void)//前进L
{
		PosL=1;
		GndL=0;	
}
void Car_ForwardRunR(void)//前进R
{
		PosR=1;
		GndR=0;	
}

void Car_BackwardRunL(void)//后退L
{
		PosL=0;
		GndL=1;	
}
void Car_BackwardRunR(void)//后退R
{
		PosR=0;
		GndR=1;	
}

void Car_StopRunL(void)//停止L
{
		PosL=0;
		GndL=0;	
}
void Car_StopRunR(void)//停止R
{
		PosR=0;
		GndR=0;	
}

