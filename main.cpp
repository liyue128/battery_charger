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
		memset(this, 0, sizeof(EiStructure));  //�ṹ���ʼ��
	}
}*ExtremlyImbalance;//����ṹ��

TypeOfStruct DataReceive(string s);
void ChargeProcess(string line_data, EiStructure *ExtremlyImbalance);

int main()
{
	ExtremlyImbalance = new EiStructure;
	//���������ļ�������
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
	//���ݽ��ն˿�,���ղ���������
	*charger_data_structure = DataReceive(line_data);
	
//	bool extermly_imbalance_flag = 0;
//	bool extermly_imbalance_bit = 0;
	float sum_of_cell = 0;
	ExtremlyImbalance->flag = ExtermlyImbalanceFlag(charger_data_structure, ExtremlyImbalance->bit);
	int state_of_charger = ChargeStateJudge(charger_data_structure, ExtremlyImbalance->flag);

//	while (1) {
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
		ExtremlyImbalance->flag = ExtermlyImbalanceFlag(charger_data_structure, ExtremlyImbalance->bit);
		cout << "delay " << T0 << endl;  //�ý׶����ĵ�ʱ��ΪTn
		charger_data_structure->t -= T0;

		switch (state_of_charger)
		{
		case BATTERY_ABSENT:
			BatteryAbsent(charger_data_structure);
			/**************״̬��ת*****************/
			if (charger_data_structure->bat) {
				state_of_charger = ChargeStateJudge(charger_data_structure, ExtremlyImbalance->flag);  //δ��⵽���״̬������ת������״̬
			}
			break;

		case EXTREMLY_IMBALANCE:
			ExtremlyImbalance->bit = DischargeInExtrem(charger_data_structure);
			/**************״̬��ת*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			ExtremlyImbalance->flag = ExtermlyImbalanceFlag(charger_data_structure, ExtremlyImbalance->bit);  //���¼��˲�ƽ���ʶλ
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
			/**************״̬��ת*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			if (sum_of_cell >= VLOWV) {
				state_of_charger = EQUAL_CURRENT;
			}
			break;

		case EQUAL_CURRENT:
			EqualCurrent(charger_data_structure, &ExtremlyImbalance->bit);
			/**************״̬��ת*****************/
			sum_of_cell = SumOfCell(charger_data_structure->cell_voltage);
			ExtremlyImbalance->flag = ExtermlyImbalanceFlag(charger_data_structure, ExtremlyImbalance->bit);
			if (ExtremlyImbalance->flag) {
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