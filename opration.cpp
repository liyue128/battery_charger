#include "main.h"
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
	//若计时器未开启，则使能计时器
	printf("start timer.\n");
}

//禁用计时器
void DisableTimer(void)
{
	//若计时器已开启,则禁用计时器
	printf("end timer.\n");
}

//求电压最大值
float MaxOfCell(float cell[])
{
	float max = cell[0];
	for (int i = 0; i < NUM; i++)
		if (cell[i] > max) max = cell[i];
	return max;
}

//求电压最小值
float MinOfCell(float cell[])
{
	float min = cell[0];
	for (int i = 0; i < NUM; i++)
		if (cell[i] < min) min = cell[i];
	return min;
}

//求总电压
float SumOfCell(float cell[])
{
	float sum = 0;
	for (int i = 0; i < NUM; i++) sum += cell[i];
	return sum;
}

//闭合开关
void SwStart(bool* SW1)
{
	//若开关已断开,则闭合开关
	if (!*SW1) {
		*SW1 = true;
	}

	cout << "SW = 1" << endl;
}

//断开开关
void SwEnd(bool* SW1)
{
	//若开关已闭合,则断开开关
	if (*SW1) {
		*SW1 = false;
	}

	cout << "SW = 0" << endl;
}

void NotInExtrem(bool* SW1)
{
	SwStart(SW1);
	DisableTimer();
}

void InExtrem(bool* SW1)
{
	SwEnd(SW1);
	EnableTimer();
}

void NotDetectBat(bool* SW1)
{
	SwEnd(SW1);
	DisableTimer();
}