#include <iostream>
#include "main.h"
#include "charger.h"
#include "operation.h"
//#include "batterydata.h"

using namespace std;

void DataCollection(double cell_voltage[NUM], int* bat, int* stat1, int* stat2, bool* SW1)
{
#ifdef i2c_data
	//��I2C���ܵ�������ȡ��ѹ�����������ݣ�IO�ڶ�ȡBAT,STAT1,STAT2����Ϣ    
	data = DataGet_1();  //��i2c�Լ�IO���յ�����������ȡ��Ч��Ϣ,���������̵�����
	v = calculate(data);
	bat = FXinn  //��GPIO��Xn�յ������ƽ
		stat1 = ;
	stat2 = ;
	//ͬ��,�ɽ�����stat1��stat2��ֵ

#else
	//���ļ��н��յ�������,���ж�ȡ,����ȡ�������ݷ��������Ҫ����������   
	Batterydata Battery_data;
	Battery_data.CellVoltage(cell_voltage);
	*bat = Battery_data.LevelOfAfe();
	*stat1 = Battery_data.levelOfStat1();
	*stat2 = Battery_data.levelOfStat2();
	*SW1 = Battery_data.levelOfSw();
#endif
}



//������Ӧ����·
void CellBypass(double cell[NUM], double min)
{
	for (int i = 0; i < NUM; i++)   //num��ʾ�����cell��Ŀ
	{
		//D1�ڲ��ŵ���ֵ,D2���ⲿ�ŵ���ֵ
		if (cell[i] - min >= D1 && cell[i] - min <= D2)
		{
			cout << "close inner bypass of " << i << endl;
			cout << "open outer bypass of " << i << endl;
			cell[i] -= D1;
		}  //���պ��ڲ��ŵ���·

		if (cell[i] - min > D2&& cell[i] - min <= D1 + D2)
		{
			cout << "open inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
			cell[i] -= 0.25 * D2;
		}  //���պ��ⲿ�ŵ���·

		if (cell[i] - min > D1 + D2)
		{
			cout << "close inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
			cell[i] -= D1 + 0.25 * D2;
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
void CellBalancing(double cell[NUM], double min)
{
	CellBypass(cell, min);
	Sleep(T4);
	//�ر�������·
	OpenAllBypass();
}



//���˲�ƽ��״̬��־
bool ExtermlyImbalanceFlag(double cell_voltage[NUM], bool extermly_imbalance_flag)
{
	double max_of_cell = MaxOfCell(cell_voltage);  //�����ֵ
	double min_of_cell = MinOfCell(cell_voltage);  //����Сֵ
	double sum_of_cell = SumOfCell(cell_voltage);  //���

	int flag = (max_of_cell - min_of_cell >= D1) && (sum_of_cell > VLOWV) && \
		((max_of_cell >= RECH) || extermly_imbalance_flag);
	return flag;
}

//�жϳ��״̬�ӿ�
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



//δ��⵽���
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

//���˲�ƽ��״̬����ʽ
int DischargeInExtrem(double cell_voltage[NUM], bool* SW1)
{
	int extermly_imbalance_flag;
	double min_of_cell = MinOfCell(cell_voltage);  //����Сֵ
	bool timerout = false;

	InExtrem(SW1);
	cout << "extremly imbalance" << endl;
	if (timerout)  //��ʱ����ʱ
	{
		extermly_imbalance_flag = 0;
		Abnormal(SW1);  //�쳣״̬
	}
	else {
		extermly_imbalance_flag = 1;
		CellBalancing(cell_voltage, min_of_cell);
	}
	return extermly_imbalance_flag;
}

//������
void ChargeComplete(bool* SW1)
{
	NotInExtrem(SW1);
	//	SW = 0;
	cout << "charge complete" << endl;
	SwEnd(SW1);
	cout << "���Ƶ�" << endl;
	//	cout << "delay T1" << endl;
	Sleep(T1);
}

//����쳣
void Abnormal(bool* SW1)
{
	NotInExtrem(SW1);
	//	SW = 0;

	cout << "Abnormal" << endl;
	SwEnd(SW1);
	cout << "�����, ����" << endl;
	//	cout << "delay T2" << endl;
	Sleep(T2);
}

//��ʾ���ڳ��״̬
void InCharging(bool* SW1)
{
	NotInExtrem(SW1);
	cout << "���̵�" << endl;
}

//Ԥ���״̬
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

//�������״̬
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

//��ѹ���״̬
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