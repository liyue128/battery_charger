#include <iostream>
#include "main.h"
#include "charger.h"
#include "operation.h"
//#include "batterydata.h"

using namespace std;

void DataCollection(double cell_voltage[NUM], int* bat, int* stat1, int* stat2, bool* SW1)
{
#ifdef i2c_data
	//从I2C接受的数据提取电压计算所需数据，IO口读取BAT,STAT1,STAT2的信息    
	data = DataGet_1();  //从i2c以及IO接收到的数据中提取有效信息,属于主体编程的内容
	v = calculate(data);
	bat = FXinn  //即GPIO口Xn收到输入电平
		stat1 = ;
	stat2 = ;
	//同理,可解析得stat1和stat2的值

#else
	//从文件中接收到的数据,逐行读取,将读取到的数据翻译成所需要的数据类型   
	Batterydata Battery_data;
	Battery_data.CellVoltage(cell_voltage);
	*bat = Battery_data.LevelOfAfe();
	*stat1 = Battery_data.levelOfStat1();
	*stat2 = Battery_data.levelOfStat2();
	*SW1 = Battery_data.levelOfSw();
#endif
}



//开启相应的旁路
void CellBypass(double cell[NUM], double min)
{
	for (int i = 0; i < NUM; i++)   //num表示电池中cell数目
	{
		//D1内部放电阈值,D2表外部放电阈值
		if (cell[i] - min >= D1 && cell[i] - min <= D2)
		{
			cout << "close inner bypass of " << i << endl;
			cout << "open outer bypass of " << i << endl;
			cell[i] -= D1;
		}  //仅闭合内部放电旁路

		if (cell[i] - min > D2&& cell[i] - min <= D1 + D2)
		{
			cout << "open inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
			cell[i] -= 0.25 * D2;
		}  //仅闭合外部放电旁路

		if (cell[i] - min > D1 + D2)
		{
			cout << "close inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
			cell[i] -= D1 + 0.25 * D2;
		}  //闭合内部和外部旁路
	}
}

//断开所有旁路
void OpenAllBypass(void)
{
	//同时操作寄存器内部寄存器和外部旁路开关
	cout << "open all bypass" << endl;
}

//平衡电压接口
void CellBalancing(double cell[NUM], double min)
{
	CellBypass(cell, min);
	Sleep(T4);
	//关闭所有旁路
	OpenAllBypass();
}



//极端不平衡状态标志
bool ExtermlyImbalanceFlag(double cell_voltage[NUM], bool extermly_imbalance_flag)
{
	double max_of_cell = MaxOfCell(cell_voltage);  //求最大值
	double min_of_cell = MinOfCell(cell_voltage);  //求最小值
	double sum_of_cell = SumOfCell(cell_voltage);  //求和

	int flag = (max_of_cell - min_of_cell >= D1) && (sum_of_cell > VLOWV) && \
		((max_of_cell >= RECH) || extermly_imbalance_flag);
	return flag;
}

//判断充电状态接口
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



//未检测到电池
void BatteryAbsent(double cell[NUM], int* stat1, int* stat2, bool* SW1)
{
	SwEnd(SW1);
	DisableTimer();
	/*	for (int i = 0; i < NUM; i++) {
			cell[i] = 0;
		}
		*stat1 = 0;
		*stat2 = 0;*/
		//	cout << "delay T1" << endl;
	Sleep(T1);
}

//极端不平衡状态处理方式
int DischargeInExtrem(double cell_voltage[NUM], bool* SW1)
{
	int extermly_imbalance_flag;
	double min_of_cell = MinOfCell(cell_voltage);  //求最小值
	bool timerout = false;

	InExtrem(SW1);
	cout << "extremly imbalance" << endl;
	if (timerout)  //计时器超时
	{
		extermly_imbalance_flag = 0;
		Abnormal(SW1);  //异常状态
	}
	else {
		extermly_imbalance_flag = 1;
		CellBalancing(cell_voltage, min_of_cell);
	}
	return extermly_imbalance_flag;
}

//充电完成
void ChargeComplete(bool* SW1)
{
	NotInExtrem(SW1);
	//	SW = 0;
	cout << "charge complete" << endl;
	SwEnd(SW1);
	cout << "亮黄灯" << endl;
	//	cout << "delay T1" << endl;
	Sleep(T1);
}

//充电异常
void Abnormal(bool* SW1)
{
	NotInExtrem(SW1);
	//	SW = 0;

	cout << "Abnormal" << endl;
	SwEnd(SW1);
	cout << "亮红灯, 蜂鸣" << endl;
	//	cout << "delay T2" << endl;
	Sleep(T2);
}

//显示正在充电状态
void InCharging(bool* SW1)
{
	NotInExtrem(SW1);
	cout << "亮绿灯" << endl;
}

//预充电状态
void Precharge(double cell[NUM], bool* flag, bool* SW1)
{
	*flag = 0;

	InCharging(SW1);
	cout << "stage of precharge" << endl;
	for (int i = 0; i < NUM; i++) {
		cell[i] += D1;
	}
	Sleep(T3);
	//	cout << "delay T3" << endl;
}

//恒流充电状态
void EqualCurrent(double cell_voltage[NUM], bool* flag, bool* SW1)
{
	double max_of_cell = MaxOfCell(cell_voltage);
	double min_of_cell = MinOfCell(cell_voltage);

	*flag = 0;

	InCharging(SW1);
	cout << "stage of equalcurrent" << endl;
	for (int i = 0; i < NUM; i++) {
		cell_voltage[i] += D1 + 0.5 * D2;
	}

	if (max_of_cell - min_of_cell > D1) {
		CellBalancing(cell_voltage, min_of_cell);
	}
	//	cout << "delay T4" << endl;
	Sleep(T4);
}

//恒压充电状态
void EqualVoltage(double cell[NUM], bool* SW1)
{
	InCharging(SW1);
	cout << "stage of equalvoltage" << endl;
	for (int i = 0; i < NUM; i++) {
		cell[i] += D1;
	}

	//	cout << "delay T5" << endl;
	Sleep(T5);
}