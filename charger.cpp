#include <iostream>
#include "main.h"
#include "charger.h"
#include "operation.h"
#include "batterydata.h"
#define D1 0.02
#define D2 0.08
#define RECH 4.1
#define VLOWV 9.3
#define VRECH 12.3
#define BATTERY_ABSENT 0
#define EXTREMLY_IMBALANCE 1
#define CHARGE_COMPLETE 2
#define CHARGE_ABNORMAL 3
#define PRECHARGE 4
#define EQUAL_CURRENT 5
#define EQUAL_VOLTAGE 6
#define T0 50
#define T1 2000
#define T2 2000
#define T3 1500
#define T4 1000
#define T5 800

using namespace std;

void DataCollection(double cell_voltage[NUM], int* bat, int* stat1, int* stat2)
{
#ifdef i2c_data
	//��I2C���ܵ�������ȡ��ѹ�����������ݣ�IO�ڶ�ȡBAT,STAT1,STAT2����Ϣ    
	DataGet_1();  //��i2c�Լ�IO���յ�����������ȡ��Ч��Ϣ,���������̵�����
	v = calculate(data);
	bat = FXinn  //��GPIO��Xn�յ������ƽ
	//ͬ��,�ɽ�����stat1��stat2��ֵ

#else
	//���ļ��н��յ�������,���ж�ȡ,����ȡ�������ݷ��������Ҫ����������   
	Batterydata Battery_data;
	Battery_data.CellVoltage(cell_voltage);
	*bat = Battery_data.LevelOfAfe();
	*stat1 = Battery_data.levelOfStat1();
	*stat2 = Battery_data.levelOfStat2();
#endif
}

//�Ͽ�������·
void OpenAllBypass(void)
{
	cout << "open all bypass" << endl;
}

//������Ӧ����·
void CellBypass(double cell[NUM], double min)
{
	for (int i = 0; i < NUM; i++)   //num��ʾ�����cell��Ŀ
	{
		if (cell[i] - min > D1&& cell[i] - min <= D2)
			//D1�ڲ��ŵ���ֵ,D2���ⲿ�ŵ���ֵ
		{
			cout << "close inner bypass of " << i << endl;
			cout << "open outer bypass of " << i << endl;
		}  //���ڲ��ŵ���·

		if (cell[i] - min > D2&& cell[i] - min <= D1 + D2)
		{
			cout << "open inner bypass of " << i << endl;
			cout << "close outer bypass of " << i << endl;
		}  //���ⲿ�ŵ���·

		if (cell[i] > D1 + D2)
		{
			cout << "open inner bypass of " << i << endl;
			cout << "open outer bypass of " << i << endl;
		}
	}
}

//ƽ���ѹ�ӿ�
void CellBalancing(double cell[NUM], double min)
{
	CellBypass(cell, min);
	cout << "delay T4" << endl;
	OpenAllBypass();  //�ر����г����·
}

//��ʾ���ڳ��״̬
void InCharging(void)
{
	cout << "���̵�" << endl;
}

void BatteryAbsent(void)
{
	SwEnd();
	DisableTimer();
	cout << "delay T1" << endl;
}

//������
void ChargeComplete(void)
{
	SwStart();
	DisableTimer();
//	SW = 0;
	cout << "���Ƶ�" << endl;
	cout << "delay T1" << endl;
}

//����쳣
void Abnormal(void)
{
	SwStart();
	DisableTimer();
//	SW = 0;
	cout << "�����, ����" << endl;
	cout << "delay T2" << endl;
}

//Ԥ���״̬
void Precharge(void)
{
	SwStart();
	DisableTimer();
	InCharging();
	cout << "stage of precharge" << endl;
	cout << "delay T3" << endl;
}

//�������״̬
void EqualCurrent(double cell_voltage[NUM])
{
	double max_of_cell = MaxOfCell(cell_voltage);
	double min_of_cell = MinOfCell(cell_voltage);

	SwStart();
	DisableTimer();
	InCharging();
	cout << "stage of equalcurrent" << endl;
	if (max_of_cell - min_of_cell > D1) {
		CellBalancing(cell_voltage, min_of_cell);
	}
	cout << "delay T4" << endl;
}

//��ѹ���״̬
void EqualVoltage(void)
{
	SwStart();
	DisableTimer();
	InCharging();
	cout << "stage of equalvoltage" << endl;
	cout << "delay T5" << endl;
}

//���˲�ƽ��״̬��־
int ExtermlyImbalanceFlag(double cell_voltage[NUM], int extermly_imbalance_flag)
{
	double max_of_cell = MaxOfCell(cell_voltage);  //�����ֵ
	double min_of_cell = MinOfCell(cell_voltage);  //����Сֵ
	double sum_of_cell = SumOfCell(cell_voltage);  //���

	SwEnd();
	EnableTimer();
	int flag = max_of_cell - min_of_cell > D1&& sum_of_cell > VLOWV&& \
		(max_of_cell > RECH || extermly_imbalance_flag);
	return flag;
}

//���˲�ƽ��״̬����ʽ
int DischargeInExtrem(double cell_voltage[NUM])
{
	int extermly_imbalance_flag;
	double min_of_cell = MinOfCell(cell_voltage);  //����Сֵ
	bool timerout = false;

	if (timerout)  //��ʱ����ʱ
	{
		extermly_imbalance_flag = 0;
		Abnormal();  //�쳣״̬
	}
	else {
		extermly_imbalance_flag = 1;
		CellBalancing(cell_voltage, min_of_cell);
	}
	return extermly_imbalance_flag;
}

//�жϼ��˲�ƽ��״̬�ӿ�
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
