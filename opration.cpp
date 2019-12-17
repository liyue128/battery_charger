#include <iostream>
#include "charger.h"

using namespace std;

/*calculate voltages of cells
double calcute(data)
{
	int VCOUT = ADC_Count/Full_Scale_Count*VREF_NOMINAL;

	int GC_VCOUT=[(VCn_GC_4<<4)+VCn_GAIN_CORR]*0.001;
	int OC_VCOUT=[(VCn_OC_4<<4)+VCn_OFFSET_CORR]*0.001;
	int GC_VREF=(1+[(VREF_GC_4<<4)+VREF_GAIN_CORR]*0.001)+\
	[(VREF_OC_5<<5)+(VREF_OC_4<<4)+VREF_OFFSET_CORR]*0.001/VREF_NOMINAL;

	VCn=(VCOUT*GC_VREF+OC_VCOUT)/G_VCOUT*(1+GC_VCOUT);
	return VCn;
}*/

//使能计时器
void EnableTimer(void)
{
	printf("start timer.\n");
	//	TIM_Cmd(TIM1, ENABLE);
//	TIMER_SWITCH_FLAG = 1;
}

//禁用计时器
void DisableTimer(void)
{
	printf("end timer.\n");
	//	TIM_Cmd(TIM1, DISABLE);
//	TIMER_SWITCH_FLAG = 0;
}

//求电压最大值
double MaxOfCell(double cell[])
{
	double max = cell[0];
	for (int i = 0; i < NUM; i++)
		if (cell[i] > max) max = cell[i];
	return max;
}

//求电压最小值
double MinOfCell(double cell[])
{
	double min = cell[0];
	for (int i = 0; i < NUM; i++)
		if (cell[i] < min) min = cell[i];
	return min;
}

//求总电压
double SumOfCell(double cell[])
{
	double sum = 0;
	for (int i = 0; i < NUM; i++) sum += cell[i];
	return sum;
}

//闭合开关
void SwStart(void)
{
	cout << "SW = 1" << endl;
}

//断开开关
void SwEnd(void)
{
	cout << "SW = 0" << endl;
}

