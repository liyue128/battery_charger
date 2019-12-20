#include "main.h"
#include <iostream>
#include "operation.h"
#include "charger.h"

using namespace std;

struct voltage
{
	int bat, stat1, stat2;
	double cell_voltage[NUM];
	bool SW1;

	voltage()
	{
		memset(this, 0, sizeof(voltage));  //结构体初始化
	}
}*cell_struct;//定义结构体
voltage DataReceive(string s);

int main()
{
	string s1 = "d:\\test\\o\\extrem";
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
	int t = 0;

	cell_struct = new voltage;

	//数据接收端口,接收并解析数据
	*cell_struct = DataReceive(line_data);

	//测试文件的第一行, 为充电数据结构的注释(调用向文件中写入的库)
	ofstream outData(s);
	outData << "#数据结构: i cell[0] cell[1] cell[2] cell[3] bat stat1 stat2 SW1 o" << endl;
	//	outData.close();//从键盘输入并写入文件

	while (1) {
		sum_of_cell = SumOfCell(cell_struct->cell_voltage);

		//检测电池
		if (!cell_struct->bat) {
			state_of_charger = BATTERY_ABSENT;
		}

		//检测异常
		if (state_of_charger >= PRECHARGE && state_of_charger <= EQUAL_VOLTAGE) {
			if (cell_struct->stat1 == 0 && cell_struct->stat2 == 0) {
				state_of_charger = CHARGE_ABNORMAL;
			}
		}

		//极端不平衡状态检测接口
		extermly_imbalance_flag = ExtermlyImbalanceFlag(cell_struct->cell_voltage, extermly_imbalance_bit);
		cout << "delay  Tn" << endl;  //该阶段消耗的时间为Tn

		switch (state_of_charger)
		{
		case BATTERY_ABSENT:
			BatteryAbsent(cell_struct->cell_voltage, &cell_struct->stat1, &cell_struct->stat2, &cell_struct->SW1);
			/**************状态跳转*****************/
			if (cell_struct->bat) {
				state_of_charger = ChargeStateJudge(cell_struct->bat, extermly_imbalance_flag, cell_struct->cell_voltage, cell_struct->stat1, cell_struct->stat2);
			}
			break;

		case EXTREMLY_IMBALANCE:
			extermly_imbalance_bit = DischargeInExtrem(cell_struct->cell_voltage, &cell_struct->SW1);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(cell_struct->cell_voltage);  //更新电压总和
			extermly_imbalance_flag = ExtermlyImbalanceFlag(cell_struct->cell_voltage, extermly_imbalance_bit);  //更新极端不平衡标识
			//放电计时器超时检测
			if (++t >= 20) { state_of_charger = CHARGE_ABNORMAL; }
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
			ChargeComplete(&cell_struct->SW1);
			state_of_charger = BATTERY_ABSENT;  //假设一旦检测到电池充满电就移除电池,仅用于模拟
			break;

		case CHARGE_ABNORMAL:
			Abnormal(&cell_struct->SW1);
			state_of_charger = BATTERY_ABSENT;  //仅用于模拟
			break;

		case PRECHARGE:
			Precharge(cell_struct->cell_voltage, &extermly_imbalance_bit, &cell_struct->SW1);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(cell_struct->cell_voltage);
			if (sum_of_cell >= VLOWV) {
				state_of_charger = EQUAL_CURRENT;
			}
			break;

		case EQUAL_CURRENT:
			EqualCurrent(cell_struct->cell_voltage, &extermly_imbalance_bit, &cell_struct->SW1);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(cell_struct->cell_voltage);
			extermly_imbalance_flag = ExtermlyImbalanceFlag(cell_struct->cell_voltage, extermly_imbalance_bit);
			if (extermly_imbalance_flag) {
				state_of_charger = EXTREMLY_IMBALANCE;
			}
			else if (sum_of_cell >= VRECH) {  //由于已经发生电压数据更新,使用上一时刻的数据会造成延迟
				state_of_charger = EQUAL_VOLTAGE;
			}
			break;

		case EQUAL_VOLTAGE:
			EqualVoltage(cell_struct->cell_voltage, &cell_struct->SW1);
			/**************状态跳转*****************/
			sum_of_cell = SumOfCell(cell_struct->cell_voltage);
			StateConvert(sum_of_cell, &cell_struct->stat1, &cell_struct->stat2);  //该步骤在硬件上可自动完成
			if (cell_struct->stat1 == 0 && cell_struct->stat2 == 1) {
				state_of_charger = CHARGE_COMPLETE;
			}
			break;

		default: BatteryAbsent(cell_struct->cell_voltage, &cell_struct->stat1, &cell_struct->stat2, &cell_struct->SW1);
		}

		cout << endl;

		outData << "i " << cell_struct->cell_voltage[0] << " " << cell_struct->cell_voltage[1] << " " << cell_struct->cell_voltage[2] << " " \
			<< cell_struct->cell_voltage[3] << " " << cell_struct->bat << " " << cell_struct->stat1 << " " << cell_struct->stat2 << " " << cell_struct->SW1 << " o\n";

		//跳出循环
		if (state_of_charger == BATTERY_ABSENT) {
			break;
		}
	}
	outData.close();//从键盘输入并写入文件

	Sleep(1000);
}

void StateConvert(double sum, int* stat1, int* stat2)
{
	if (sum >= 16.8) {
		*stat1 = 0;
		*stat2 = 1;
	}
}

voltage DataReceive(string s)
{
	voltage cell;
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

	return cell;
}