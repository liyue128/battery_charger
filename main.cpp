#include "main.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "operation.h"
#include "charger.h"

using namespace std;

struct EiStructure
{
	bool flag;
	bool bit;

	EiStructure()
	{
		memset(this, 0, sizeof(EiStructure));  //结构体初始化
	}
}*ExtremlyImbalance;//定义结构体

TypeOfStruct DataReceive(string s);
void ChargeProcess(string line_data, EiStructure *ExtremlyImbalance);

int main()
{
	ExtremlyImbalance = new EiStructure;
	//测试输入文件数据流
	ifstream file;
	file.open("D:\\test\\i\\input2.txt", ios::in);
	if (!file.is_open())
		return 0;

	string strLine;

	while (getline(file, strLine))
	{
		if (strLine.empty() || strLine[0] == '#')
			continue;

		ChargeProcess(strLine, ExtremlyImbalance);

	}

	delete ExtremlyImbalance;
	return 0;
}

void ChargeProcess(string line_data, EiStructure* ExtremlyImbalance)
{
	TypeOfStruct* charger_data_structure;
	charger_data_structure = new TypeOfStruct;
	//数据接收端口,接收并解析数据
	*charger_data_structure = DataReceive(line_data);
	
//	bool extermly_imbalance_flag = 0;
//	bool extermly_imbalance_bit = 0;
	float sum_of_cell = 0;
	ExtremlyImbalance->flag = ExtermlyImbalanceFlag(charger_data_structure, ExtremlyImbalance->bit);
	int state_of_charger = ChargeStateJudge(charger_data_structure, ExtremlyImbalance->flag);

//	while (1) {
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
		ExtremlyImbalance->flag = ExtermlyImbalanceFlag(charger_data_structure, ExtremlyImbalance->bit);
		cout << "delay " << T0 << endl;  //该阶段消耗的时间为Tn
		charger_data_structure->t -= T0;

		switch (state_of_charger)
		{
		case BATTERY_ABSENT:
			BatteryAbsent(charger_data_structure);
			/**************状态跳转*****************/
			if (charger_data_structure->bat) {
				state_of_charger = ChargeStateJudge(charger_data_structure, ExtremlyImbalance->flag);  //未检测到电池状态可能跳转到任意状态
			}
			break;

		case EXTREMLY_IMBALANCE:
			ExtremlyImbalance->bit = DischargeInExtrem(charger_data_structure);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			ExtremlyImbalance->flag = ExtermlyImbalanceFlag(charger_data_structure, ExtremlyImbalance->bit);  //更新极端不平衡标识位
			if (sum_of_cell < VLOWV) {
				state_of_charger = PRECHARGE;
			}
			else if (!ExtremlyImbalance->flag) {
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
			Precharge(charger_data_structure, &ExtremlyImbalance->bit);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			if (sum_of_cell >= VLOWV) {
				state_of_charger = EQUAL_CURRENT;
			}
			break;

		case EQUAL_CURRENT:
			EqualCurrent(charger_data_structure, &ExtremlyImbalance->bit);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			ExtremlyImbalance->flag = ExtermlyImbalanceFlag(charger_data_structure, ExtremlyImbalance->bit);
			if (ExtremlyImbalance->flag) {
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

		cout << "delay " << charger_data_structure->t <<endl;
		cout << endl;

//	}

	Sleep(10);
	delete charger_data_structure;
}
/*void StateConvert(TypeOfStruct* cell_structure)
{
	double sum = SumOfCell(cell_structure->cell_voltage);
	if (sum >= 16.8) {
		cell_structure->stat1 = false;
		cell_structure->stat2 = true;
	}
}*/