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

//������Ӧ����·
void CellBypass(float cell[NUM], float min)
{
	for (int i = 0; i < NUM; i++)   //num��ʾ�����cell��Ŀ
	{
		//D1�ڲ��ŵ���ֵ,D2���ⲿ�ŵ���ֵ
		if (cell[i] - min >= D1 && cell[i] - min <= D2)
		{
			cout << "close inner bypass of " << i << endl;
			cout << "open outer bypass of " << i << endl;
		}  //���պ��ڲ��ŵ���·

		if (cell[i] - min > D2&& cell[i] - min <= D1 + D2)
		{
			cout << "open inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
		}  //���պ��ⲿ�ŵ���·

		if (cell[i] - min > D1 + D2)
		{
			cout << "close inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
		}  //�պ��ڲ����ⲿ��·
	}
}

//�Ͽ�������·
void OpenAllBypass(void)
{
	//ͬʱ�����Ĵ����ڲ��Ĵ������ⲿ��·����
	cout << "open all bypass" << endl;
}

//ƽ���ѹ�ӿ�
void CellBalancing(TypeOfStruct* cell_structure, float min)
{
	CellBypass(cell_structure->cell_voltage, min);
	cout << "delay " << T4 << endl;
	cell_structure->t -= T4;
	//�ر�������·
	OpenAllBypass();
}



//���˲�ƽ��״̬��־
bool ExtermlyImbalanceFlag(TypeOfStruct* cell_structure, bool extermly_imbalance_flag)
{
	float max_of_cell = MaxOfCell(cell_structure->cell_voltage);  //�����ֵ
	float min_of_cell = MinOfCell(cell_structure->cell_voltage);  //����Сֵ
	float sum_of_cell = SumOfCell(cell_structure->cell_voltage);  //���

	int flag = (max_of_cell - min_of_cell >= D1) && (sum_of_cell > VLOWV) && \
		((max_of_cell >= RECH) || extermly_imbalance_flag);
	return flag;
}

//�жϳ��״̬�ӿ�
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



//δ��⵽���
void BatteryAbsent(TypeOfStruct* cell_structure)
{
	SwEnd(&cell_structure->SW1);
	DisableTimer();
	cout << "�����\n" << "battery absent" << endl;

	cout << "delay "<< T1 << endl;
	cell_structure->t -= T1;
}

//���˲�ƽ��״̬����ʽ
int DischargeInExtrem(TypeOfStruct* cell_structure)
{
	int extermly_imbalance_flag;
	float min_of_cell = MinOfCell(cell_structure->cell_voltage);  //����Сֵ
	bool timerout = false;

	InExtrem(&cell_structure->SW1);
	cout << "extremly imbalance" << endl;
	cout << "���̵�" << endl;
	if (timerout)  //��ʱ����ʱ
	{
		extermly_imbalance_flag = 0;
		Abnormal(cell_structure);  //�쳣״̬
	}
	else {
		extermly_imbalance_flag = 1;
		CellBalancing(cell_structure, min_of_cell);
	}
	return extermly_imbalance_flag;
}

//������
void ChargeComplete(TypeOfStruct* cell_structure)
{
	cout << "charge complete" << endl;
	SwEnd(&cell_structure->SW1);
	cout << "���Ƶ�" << endl;

	cout << "delay " << T1 << endl;
	cell_structure->t -= T1;
}

//����쳣
void Abnormal(TypeOfStruct* cell_structure)
{
	cout << "Abnormal" << endl;
	SwEnd(&cell_structure->SW1);
	cout << "�����, ����" << endl;

	cout << "delay " << T2 << endl;
	cell_structure->t -= T2;
}

//��ʾ���ڳ��״̬
void InCharging(TypeOfStruct* cell_structure)
{
	BeforeCharging(&cell_structure->SW1);
	cout << "���̵�" << endl;
}

//Ԥ���״̬
void Precharge(TypeOfStruct* cell_structure, bool* flag)
{
	*flag = 0;

	InCharging(cell_structure);
	cout << "stage of precharge" << endl;

	cout << "delay " << T3 << endl;
	cell_structure->t -= T3;
}

//�������״̬
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

//��ѹ���״̬
void EqualVoltage(TypeOfStruct* cell_structure)
{
	InCharging(cell_structure);
	cout << "stage of equalvoltage" << endl;

	cout << "delay " << T5 << endl;
	cell_structure->t -= T5;
}