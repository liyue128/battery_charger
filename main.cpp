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
	
	//���������ļ�������
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
		//���ݽ��ն˿�,���ղ���������
		*charger_data_structure = DataReceive(strLine);
		sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);


		//�����ڲ���
		if (t == 0) {
			
			t = 1;
			extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);
			state_of_charger = ChargeStateJudge(charger_data_structure, extermly_imbalance_flag);
		}

			if (!charger_data_structure->bat) {
				state_of_charger = BATTERY_ABSENT;
			}
			else if (state_of_charger == BATTERY_ABSENT) {  //�տ�ʼ��⵽���
				t = 0;
				SwStart(&charger_data_structure->SW1);
				cout << "delay " << charger_data_structure->t << endl;
				cout << endl;
				continue;
			}

			//����쳣
			if (state_of_charger >= PRECHARGE && state_of_charger <= EQUAL_VOLTAGE) {
				if (charger_data_structure->stat1 == false && charger_data_structure->stat2 == false) {
					state_of_charger = CHARGE_ABNORMAL;
				}
			}

			//���˲�ƽ��״̬���ӿ�
			extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);
			cout << "delay " << T0 << endl;  //�ý׶����ĵ�ʱ��ΪTn
			charger_data_structure->t -= T0;

			switch (state_of_charger)
			{
			case BATTERY_ABSENT:
				BatteryAbsent(charger_data_structure);
				/**************״̬��ת*****************/
				if (charger_data_structure->bat) {
					state_of_charger = ChargeStateJudge(charger_data_structure, extermly_imbalance_flag);  //δ��⵽���״̬������ת������״̬
				}
				break;

			case EXTREMLY_IMBALANCE:
				extermly_imbalance_bit = DischargeInExtrem(charger_data_structure);
				/**************״̬��ת*****************/
				sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
				extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);  //���¼��˲�ƽ���ʶλ
				if (++t >= 15) { state_of_charger = CHARGE_ABNORMAL; }  //ģ��MCU����ʱ����ʱ
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
				/**************״̬��ת*****************/
				sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
				if (sum_of_cell >= VLOWV) {
					state_of_charger = EQUAL_CURRENT;
				}
				break;

			case EQUAL_CURRENT:
				EqualCurrent(charger_data_structure, &extermly_imbalance_bit);
				/**************״̬��ת*****************/
				sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
				extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);
				if (extermly_imbalance_flag) {
					state_of_charger = EXTREMLY_IMBALANCE;
				}
				else if (sum_of_cell >= VRECH) {  //�����Ѿ�������ѹ���ݸ���,ʹ����һʱ�̵����ݻ�����ӳ�
					state_of_charger = EQUAL_VOLTAGE;
				}
				break;

			case EQUAL_VOLTAGE:
				EqualVoltage(charger_data_structure);
				/**************״̬��ת*****************/
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