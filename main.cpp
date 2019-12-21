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
	int t = 0;  //ģ���ʱ��

	TypeOfStruct *charger_data_structure;
	charger_data_structure = new TypeOfStruct;

	//���ݽ��ն˿�,���ղ���������
	*charger_data_structure = DataReceive(line_data);

	//�����ļ��ĵ�һ��, Ϊ������ݽṹ��ע��(�������ļ���д��Ŀ�)
	ofstream outData(s);
	outData << "#���ݽṹ: i cell[0] cell[1] cell[2] cell[3] bat stat1 stat2 SW1 o, Ĭ�ϳ�ʼ״̬Ϊδ��⵽���״̬" << endl;
	//	outData.close();//�Ӽ������벢д���ļ�

	while (1) {
		sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);

		//�����
		if (!charger_data_structure->bat) {
			state_of_charger = BATTERY_ABSENT;
		}

		//����쳣
		if (state_of_charger >= PRECHARGE && state_of_charger <= EQUAL_VOLTAGE) {
			if (charger_data_structure->stat1 == false && charger_data_structure->stat2 == false) {
				state_of_charger = CHARGE_ABNORMAL;
			}
		}

		//���˲�ƽ��״̬���ӿ�
		extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);
		cout << "delay  Tn" << endl;  //�ý׶����ĵ�ʱ��ΪTn

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
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);  //���µ�ѹ�ܺ�
			extermly_imbalance_flag = ExtermlyImbalanceFlag(charger_data_structure, extermly_imbalance_bit);  //���¼��˲�ƽ���ʶλ
			if (++t >= 30) { state_of_charger = CHARGE_ABNORMAL; }  //ģ��MCU����ʱ����ʱ
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
			state_of_charger = BATTERY_ABSENT;  //����һ����⵽��س�������Ƴ����,������ģ��
			break;

		case CHARGE_ABNORMAL:
			Abnormal(charger_data_structure);
			state_of_charger = BATTERY_ABSENT;  //������ģ��
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
			StateConvert(charger_data_structure);  //�ò�����Ӳ���Ͽ��Զ����
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

		//����ѭ��
		if (state_of_charger == BATTERY_ABSENT) {
			break;
		}
	}
	outData.close();//�Ӽ������벢д���ļ�

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