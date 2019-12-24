#include "main.h"
#include <iostream>
#include "charger.h"
#include "operation.h"
#include <Windows.h>
float D1 = 0.02;
float D2 = 0.08;

using namespace std;
TypeOfStruct DataReceive(string s);


TypeOfStruct DataReceive(string s)
{
	TypeOfStruct cell;
#ifdef _I2C_H

	cell.cell_voltage[i]
		cell.bat
		cell.stat1
		cell.stat2
		cell.SW1

#else
	if (s[0] == 'i' && s[30] == 'o') {
		for (int i = 0; i < NUM; i++) {
			for (int i = 0; i < NUM; i++) {
				string cut = s.substr(5 * i + 2, 4);
				cell.cell_voltage[i] = atof(cut.c_str());
			}
		}
		cell.bat = s[22] - '0';
		cell.stat1 = s[24] - '0';
		cell.stat2 = s[26] - '0';
		cell.SW1 = s[28] - '0';
		cell.t = 500;
	}
#endif  

	return cell;
}

//开启相应的旁路
void CellBypass(float cell[NUM], float min)
{
	for (int i = 0; i < NUM; i++)   //num表示电池中cell数目
	{
		//D1内部放电阈值,D2表外部放电阈值
		if (cell[i] - min >= D1 && cell[i] - min <= D2)
		{
			cout << "close inner bypass of " << i << endl;
			cout << "open outer bypass of " << i << endl;
		}  //仅闭合内部放电旁路

		if (cell[i] - min > D2&& cell[i] - min <= D1 + D2)
		{
			cout << "open inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
		}  //仅闭合外部放电旁路

		if (cell[i] - min > D1 + D2)
		{
			cout << "close inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
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
void CellBalancing(TypeOfStruct* cell_structure, float min)
{
	CellBypass(cell_structure->cell_voltage, min);
	cout << "delay " << T4 << endl;
	cell_structure->t -= T4;
	//关闭所有旁路
	OpenAllBypass();
}



//极端不平衡状态标志
bool ExtermlyImbalanceFlag(TypeOfStruct* cell_structure, bool extermly_imbalance_flag)
{
	float max_of_cell = MaxOfCell(cell_structure->cell_voltage);  //求最大值
	float min_of_cell = MinOfCell(cell_structure->cell_voltage);  //求最小值
	float sum_of_cell = SumOfCell(cell_structure->cell_voltage);  //求和

	int flag = (max_of_cell - min_of_cell >= D1) && (sum_of_cell > VLOWV) && \
		((max_of_cell >= RECH) || extermly_imbalance_flag);
	return flag;
}

//判断充电状态接口
int ChargeStateJudge(TypeOfStruct* cell_structure, int extermly_imbalance_bit)
{
	int bit = 0;

	float sum = SumOfCell(cell_structure->cell_voltage);
	if (!cell_structure->bat) {
		bit = BATTERY_ABSENT;
	}
	else {
		if (extermly_imbalance_bit) {
			bit = EXTREMLY_IMBALANCE;
		}
		else {
			//         charger_state_bit = START_CHARGE;
			if (cell_structure->stat1 == true && cell_structure->stat2 == false) {
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
			if (cell_structure->stat1 == false && cell_structure->stat2 == true) {
				bit = CHARGE_COMPLETE;
			}
			if (cell_structure->stat1 == false && cell_structure->stat2 == false) {
				bit = CHARGE_ABNORMAL;
			}
		}
	}

	return bit;
}



//未检测到电池
void BatteryAbsent(TypeOfStruct* cell_structure)
{
	SwEnd(&cell_structure->SW1);
	DisableTimer();
	cout << "亮红灯\n" << "battery absent" << endl;

	cout << "delay "<< T1 << endl;
	cell_structure->t -= T1;
}

//极端不平衡状态处理方式
int DischargeInExtrem(TypeOfStruct* cell_structure)
{
	int extermly_imbalance_flag;
	float min_of_cell = MinOfCell(cell_structure->cell_voltage);  //求最小值
	bool timerout = false;

	InExtrem(&cell_structure->SW1);
	cout << "extremly imbalance" << endl;
	cout << "亮绿灯" << endl;
	if (timerout)  //计时器超时
	{
		extermly_imbalance_flag = 0;
		Abnormal(cell_structure);  //异常状态
	}
	else {
		extermly_imbalance_flag = 1;
		CellBalancing(cell_structure, min_of_cell);
	}
	return extermly_imbalance_flag;
}

//充电完成
void ChargeComplete(TypeOfStruct* cell_structure)
{
	cout << "charge complete" << endl;
	SwEnd(&cell_structure->SW1);
	cout << "亮黄灯" << endl;

	cout << "delay " << T1 << endl;
	cell_structure->t -= T1;
}

//充电异常
void Abnormal(TypeOfStruct* cell_structure)
{
	cout << "Abnormal" << endl;
	SwEnd(&cell_structure->SW1);
	cout << "亮红灯, 蜂鸣" << endl;

	cout << "delay " << T2 << endl;
	cell_structure->t -= T2;
}

//显示正在充电状态
void InCharging(TypeOfStruct* cell_structure)
{
	BeforeCharging(&cell_structure->SW1);
	cout << "亮绿灯" << endl;
}

//预充电状态
void Precharge(TypeOfStruct* cell_structure, bool* flag)
{
	*flag = 0;

	InCharging(cell_structure);
	cout << "stage of precharge" << endl;

	cout << "delay " << T3 << endl;
	cell_structure->t -= T3;
}

//恒流充电状态
void EqualCurrent(TypeOfStruct* cell_structure, bool* flag)
{
	*flag = 0;

	InCharging(cell_structure);
	cout << "stage of equalcurrent" << endl;

	float max_of_cell = MaxOfCell(cell_structure->cell_voltage);
	float min_of_cell = MinOfCell(cell_structure->cell_voltage);
	if (max_of_cell - min_of_cell > D1) {
		CellBalancing(cell_structure, min_of_cell);
	}
	else {
		cout << "delay " << T4 << endl;
		cell_structure->t -= T4;
	}
}

//恒压充电状态
void EqualVoltage(TypeOfStruct* cell_structure)
{
	InCharging(cell_structure);
	cout << "stage of equalvoltage" << endl;

	cout << "delay " << T5 << endl;
	cell_structure->t -= T5;
}