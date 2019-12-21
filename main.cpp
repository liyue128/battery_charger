#include "main.h"
#include <iostream>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include "operation.h"
#include "charger.h"

using namespace std;

TypeOfStruct DataReceive(string s);
void StateConvert(TypeOfStruct* cell_structure);
void ChargeProcess(string s, string line_data);

int main()
{
	string s1 = "d:\\test\\o\\testfile";
	string s2 = ".txt";
	char j = '1';
	
	ifstream file;
	file.open("D:\\test\\data.txt", ios::in);
	if (!file.is_open())
		return 0;
	std::string strLine;
	while (getline(file, strLine))
	{
		if (strLine.empty())
			continue;
		s1 += j++;
		ChargeProcess(s1 + s2, strLine);
		s1.pop_back();
	}
	return 0;
}

void ChargeProcess(string s, string line_data)
{
	bool extermly_imbalance_flag = 0;
	bool extermly_imbalance_bit = 0;
	double sum_of_cell = 0;
	int state_of_charger = BATTERY_ABSENT;
	int t = 0;  //模拟计时器

	TypeOfStruct *charger_data_structure;
	charger_data_structure = new TypeOfStruct;

	//数据接收端口,接收并解析数据
	*charger_data_structure = DataReceive(line_data);

	//测试文件的第一行, 为充电数据结构的注释(调用向文件中写入的库)
	ofstream outData(s);
	outData << "#数据结构: i cell[0] cell[1] cell[2] cell[3] bat stat1 stat2 SW1 o, 默认初始状态为未检测到电池状态" << endl;
	//	outData.close();//从键盘输入并写入文件

	while (1) {
		sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);

		//检测电池
		if (!charger_data_structure->bat) {
			state_of_charger = BATTERY_ABSENT;
		}

		//检测异常
		if (state_of_charger >= PRECHARGE && state_of_charger <= EQUAL_VOLTAGE) {
			if (charger_data_structure->stat1 == false && charger_data_structure->stat2 == false) {
				state_of_charger = CHARGE_ABNORMAL;
			}
		}

		//极端不平衡状态检测接口
		extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);
		cout << "delay  Tn" << endl;  //该阶段消耗的时间为Tn

		switch (state_of_charger)
		{
		case BATTERY_ABSENT:
			BatteryAbsent(charger_data_structure);
			/**************状态跳转*****************/
			if (charger_data_structure->bat) {
				state_of_charger = ChargeStateJudge(charger_data_structure, extermly_imbalance_flag);  //未检测到电池状态可能跳转到任意状态
			}
			break;

		case EXTREMLY_IMBALANCE:
			extermly_imbalance_bit = DischargeInExtrem(charger_data_structure);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);  //更新电压总和
			extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);  //更新极端不平衡标识位
			if (++t >= 30) { state_of_charger = CHARGE_ABNORMAL; }  //模拟MCU充电计时器超时
			if (sum_of_cell < VLOWV) {
				state_of_charger = PRECHARGE;
			}
			else if (!extermly_imbalance_flag) {
				if (sum_of_cell < VRECH) {
					state_of_charger = EQUAL_CURRENT;
				}
				else {
					state_of_charger = EQUAL_VOLTAGE;
				}
			}
			break;

		case CHARGE_COMPLETE:
			ChargeComplete(charger_data_structure);
			state_of_charger = BATTERY_ABSENT;  //假设一旦检测到电池充满电就移除电池,仅用于模拟
			break;

		case CHARGE_ABNORMAL:
			Abnormal(charger_data_structure);
			state_of_charger = BATTERY_ABSENT;  //仅用于模拟
			break;

		case PRECHARGE:
			Precharge(charger_data_structure, &extermly_imbalance_bit);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			if (sum_of_cell >= VLOWV) {
				state_of_charger = EQUAL_CURRENT;
			}
			break;

		case EQUAL_CURRENT:
			EqualCurrent(charger_data_structure, &extermly_imbalance_bit);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);
			if (extermly_imbalance_flag) {
				state_of_charger = EXTREMLY_IMBALANCE;
			}
			else if (sum_of_cell >= VRECH) {  //由于已经发生电压数据更新,使用上一时刻的数据会造成延迟
				state_of_charger = EQUAL_VOLTAGE;
			}
			break;

		case EQUAL_VOLTAGE:
			EqualVoltage(charger_data_structure);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			StateConvert(charger_data_structure);  //该步骤在硬件上可自动完成
			if (charger_data_structure->stat1 == false && charger_data_structure->stat2 == true) {
				state_of_charger = CHARGE_COMPLETE;
			}
			break;

		default: BatteryAbsent(charger_data_structure);
		}

		cout << endl;

		outData << "i " << charger_data_structure->cell_voltage[0] << " " << charger_data_structure->cell_voltage[1] << " " << charger_data_structure->cell_voltage[2] << " " \
			<< charger_data_structure->cell_voltage[3] << " " << charger_data_structure->bat << " " << charger_data_structure->stat1 << " " << charger_data_structure->stat2\
			<< " " << charger_data_structure->SW1 << " o\n";

		//跳出循环
		if (state_of_charger == BATTERY_ABSENT) {
			break;
		}
	}
	outData.close();//从键盘输入并写入文件

	Sleep(1000);
}

void StateConvert(TypeOfStruct* cell_structure)
{
	double sum = SumOfCell(cell_structure->cell_voltage);
	if (sum >= 16.8) {
		cell_structure->stat1 = false;
		cell_structure->stat2 = true;
	}
}