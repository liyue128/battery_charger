#include "main.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "operation.h"
#include "charger.h"

using namespace std;


int main()
{
	TypeOfStruct* charger_data_structure;
	charger_data_structure = new TypeOfStruct;
	bool extermly_imbalance_flag = 0;
	bool extermly_imbalance_bit = 0;
	int state_of_charger = 0;
	float sum_of_cell = 0;
	int t = 1;
	
	//测试输入文件数据流
	ifstream file;
	file.open("D:\\test\\i\\input1.txt", ios::in);
	if (!file.is_open())
		return 0;

	string strLine;

	while (getline(file, strLine))
	{
		if (strLine.empty() || strLine[0] == '#')
			continue;

//		while (1) {
		//数据接收端口,接收并解析数据
		*charger_data_structure = DataReceive(strLine);
		sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);


		//仅用于测试
		if (t == 0) {
			
			t = 1;
			extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);
			state_of_charger = ChargeStateJudge(charger_data_structure, extermly_imbalance_flag);
		}

			if (!charger_data_structure->bat) {
				state_of_charger = BATTERY_ABSENT;
			}
			else if (state_of_charger == BATTERY_ABSENT) {  //刚开始检测到电池
				t = 0;
				SwStart(&charger_data_structure->SW1);
				cout << "delay " << charger_data_structure->t << endl;
				cout << endl;
				continue;
			}

			//检测异常
			if (state_of_charger >= PRECHARGE && state_of_charger <= EQUAL_VOLTAGE) {
				if (charger_data_structure->stat1 == false && charger_data_structure->stat2 == false) {
					state_of_charger = CHARGE_ABNORMAL;
				}
			}

			//极端不平衡状态检测接口
			extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);
			cout << "delay " << T0 << endl;  //该阶段消耗的时间为Tn
			charger_data_structure->t -= T0;

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
				sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
				extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);  //更新极端不平衡标识位
				if (++t >= 15) { state_of_charger = CHARGE_ABNORMAL; }  //模拟MCU充电计时器超时
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
				break;

			case CHARGE_ABNORMAL:
				Abnormal(charger_data_structure);
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
				if (charger_data_structure->stat1 == false && charger_data_structure->stat2 == true) {
					state_of_charger = CHARGE_COMPLETE;
				}
				break;

			default: BatteryAbsent(charger_data_structure);
			}

			cout << "delay " << charger_data_structure->t << endl;
			cout << endl;

			Sleep(10);
//		}
	}

	delete charger_data_structure;
	return 0;
}


/*void StateConvert(TypeOfStruct* cell_structure)
{
	double sum = SumOfCell(cell_structure->cell_voltage);
	if (sum >= 16.8) {
		cell_structure->stat1 = false;
		cell_structure->stat2 = true;
	}
}*/