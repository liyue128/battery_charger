#include <iostream>
#include "main.h"
#include "charger.h"
#include "operation.h"
#include "batterydata.h"
#define D1 0.02
#define D2 0.08
#define RECH 4.1
#define VLOWV 9.3
#define VRECH 12.3
#define BATTERY_ABSENT 0
#define EXTREMLY_IMBALANCE 1
#define CHARGE_COMPLETE 2
#define CHARGE_ABNORMAL 3
#define PRECHARGE 4
#define EQUAL_CURRENT 5
#define EQUAL_VOLTAGE 6
#define T0 50
#define T1 2000
#define T2 2000
#define T3 1500
#define T4 1000
#define T5 800

using namespace std;

void DataCollection(double cell_voltage[NUM], int* bat, int* stat1, int* stat2)
{
#ifdef i2c_data
	//从I2C接受的数据提取电压计算所需数据，IO口读取BAT,STAT1,STAT2的信息    
	DataGet_1();  //从i2c以及IO接收到的数据中提取有效信息,属于主体编程的内容
	v = calculate(data);
	bat = FXinn  //即GPIO口Xn收到输入电平
	//同理,可解析得stat1和stat2的值

#else
	//从文件中接收到的数据,逐行读取,将读取到的数据翻译成所需要的数据类型   
	Batterydata Battery_data;
	Battery_data.CellVoltage(cell_voltage);
	*bat = Battery_data.LevelOfAfe();
	*stat1 = Battery_data.levelOfStat1();
	*stat2 = Battery_data.levelOfStat2();
#endif
}

//断开所有旁路
void OpenAllBypass(void)
{
	cout << "open all bypass" << endl;
}

//开启相应的旁路
void CellBypass(double cell[NUM], double min)
{
	for (int i = 0; i < NUM; i++)   //num表示电池中cell数目
	{
		if (cell[i] - min > D1&& cell[i] - min <= D2)
			//D1内部放电阈值,D2表外部放电阈值
		{
			cout << "close inner bypass of " << i << endl;
			cout << "open outer bypass of " << i << endl;
		}  //打开内部放电旁路

		if (cell[i] - min > D2&& cell[i] - min <= D1 + D2)
		{
			cout << "open inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
		}  //打开外部放电旁路

		if (cell[i] > D1 + D2)
		{
			cout << "open inner bypass of " << i << endl;
			cout << "open outer bypass of " << i << endl;
		}
	}
}

//平衡电压接口
void CellBalancing(double cell[NUM], double min)
{
	CellBypass(cell, min);
	cout << "delay T4" << endl;
	OpenAllBypass();  //关闭所有充电旁路
}

//显示正在充电状态
void InCharging(void)
{
	cout << "亮绿灯" << endl;
}

void BatteryAbsent(void)
{
	SwEnd();
	DisableTimer();
	cout << "delay T1" << endl;
}

//充电完成
void ChargeComplete(void)
{
	SwStart();
	DisableTimer();
//	SW = 0;
	cout << "亮黄灯" << endl;
	cout << "delay T1" << endl;
}

//充电异常
void Abnormal(void)
{
	SwStart();
	DisableTimer();
//	SW = 0;
	cout << "亮红灯, 蜂鸣" << endl;
	cout << "delay T2" << endl;
}

//预充电状态
void Precharge(void)
{
	SwStart();
	DisableTimer();
	InCharging();
	cout << "stage of precharge" << endl;
	cout << "delay T3" << endl;
}

//恒流充电状态
void EqualCurrent(double cell_voltage[NUM])
{
	double max_of_cell = MaxOfCell(cell_voltage);
	double min_of_cell = MinOfCell(cell_voltage);

	SwStart();
	DisableTimer();
	InCharging();
	cout << "stage of equalcurrent" << endl;
	if (max_of_cell - min_of_cell > D1) {
		CellBalancing(cell_voltage, min_of_cell);
	}
	cout << "delay T4" << endl;
}

//恒压充电状态
void EqualVoltage(void)
{
	SwStart();
	DisableTimer();
	InCharging();
	cout << "stage of equalvoltage" << endl;
	cout << "delay T5" << endl;
}

//极端不平衡状态标志
int ExtermlyImbalanceFlag(double cell_voltage[NUM], int extermly_imbalance_flag)
{
	double max_of_cell = MaxOfCell(cell_voltage);  //求最大值
	double min_of_cell = MinOfCell(cell_voltage);  //求最小值
	double sum_of_cell = SumOfCell(cell_voltage);  //求和

	SwEnd();
	EnableTimer();
	int flag = max_of_cell - min_of_cell > D1&& sum_of_cell > VLOWV&& \
		(max_of_cell > RECH || extermly_imbalance_flag);
	return flag;
}

//极端不平衡状态处理方式
int DischargeInExtrem(double cell_voltage[NUM])
{
	int extermly_imbalance_flag;
	double min_of_cell = MinOfCell(cell_voltage);  //求最小值
	bool timerout = false;

	if (timerout)  //计时器超时
	{
		extermly_imbalance_flag = 0;
		Abnormal();  //异常状态
	}
	else {
		extermly_imbalance_flag = 1;
		CellBalancing(cell_voltage, min_of_cell);
	}
	return extermly_imbalance_flag;
}

//判断极端不平衡状态接口
int ChargeStateJudge(int battery_detction, int extermly_imbalance_bit, double voltage[NUM], int stat1, int stat2)
{
	int bit = 0;

	double sum = SumOfCell(voltage);
	if (!battery_detction) {
		bit = BATTERY_ABSENT;
	}
	else {
		if (extermly_imbalance_bit) {
			bit = EXTREMLY_IMBALANCE;
		}
		else {
			//         charger_state_bit = START_CHARGE;
			if (stat1 == 1 && stat2 == 0) {
				//					bit = CHARGE_IN_PROCESS;
				if (sum < VLOWV) {
					bit = PRECHARGE;
				}
				else if (sum < VRECH) {
					bit = EQUAL_CURRENT;
				}
				else {
					bit = EQUAL_VOLTAGE;
				}
			}
			if (stat1 == 0 && stat2 == 1) {
				bit = CHARGE_COMPLETE;
			}
			if (stat1 == 0 && stat2 == 0) {
				bit = CHARGE_ABNORMAL;
			}
		}
	}

	return bit;
}
