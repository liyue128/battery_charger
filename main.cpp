#include "main.h"
#include <iostream>
#include "operation.h"
#include "charger.h"

using namespace std;

int main()
{
	bool extermly_imbalance_flag = 0;
	bool extermly_imbalance_bit = 0;
	double sum_of_cell = 0;
	int state_of_charger = BATTERY_ABSENT;
	int t = 0;  //t����������ʱ�ڲ���ʱ���Ƿ�ʱ���

	struct voltage
	{
		int bat, stat1, stat2;
		double cell_voltage[NUM];
		bool SW1;

		voltage()
		{
			memset(this, 0, sizeof(voltage));  //�ṹ���ʼ��
		}
	}*cell;//����ṹ��

	cell = new voltage;

	//���ݽ��ն˿�,���ղ���������
	DataCollection(cell->cell_voltage, &cell->bat, &cell->stat1, &cell->stat2, &cell->SW1);

	//�����ļ��ĵ�һ��, Ϊ������ݽṹ��ע��(�������ļ���д��Ŀ�)
	ofstream outData("d:\\test\\o\\extrem.txt");
	outData << "#���ݽṹ: i cell[0] cell[1] cell[2] cell[3] bat stat1 stat2 SW1 o" << endl;
	//	outData.close();//�Ӽ������벢д���ļ�

	while (1) {
		sum_of_cell = SumOfCell(cell->cell_voltage);

		//�����
		if (!cell->bat) {
			state_of_charger = BATTERY_ABSENT;
		}

		//����쳣
		if (state_of_charger >= PRECHARGE && state_of_charger <= EQUAL_VOLTAGE) {
			if (cell->stat1 == 0 && cell->stat2 == 0) {
				state_of_charger = CHARGE_ABNORMAL;
			}
		}

		//���˲�ƽ��״̬���ӿ�
		extermly_imbalance_flag = ExtermlyImbalanceFlag(cell->cell_voltage, extermly_imbalance_bit);
		cout << "delay  Tn" << endl;  //�ý׶����ĵ�ʱ��ΪTn

		switch (state_of_charger)
		{
		case BATTERY_ABSENT:
			BatteryAbsent(cell->cell_voltage, &cell->stat1, &cell->stat2, &cell->SW1);
			/**************״̬��ת*****************/
			if (cell->bat) {
				state_of_charger = ChargeStateJudge(cell->bat, extermly_imbalance_flag, cell->cell_voltage, cell->stat1, cell->stat2);
			}
			break;

		case EXTREMLY_IMBALANCE:
			extermly_imbalance_bit = DischargeInExtrem(cell->cell_voltage, &cell->SW1);
			/**************״̬��ת*****************/
			sum_of_cell = SumOfCell(cell->cell_voltage);  //���µ�ѹ�ܺ�
			extermly_imbalance_flag = ExtermlyImbalanceFlag(cell->cell_voltage, extermly_imbalance_bit);  //���¼��˲�ƽ���ʶ
			//�ŵ��ʱ����ʱ���
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
			ChargeComplete(&cell->SW1);
			state_of_charger = BATTERY_ABSENT;  //����һ����⵽��س�������Ƴ����,������ģ��
			break;

		case CHARGE_ABNORMAL:
			Abnormal(&cell->SW1);
			state_of_charger = BATTERY_ABSENT;  //������ģ��
			break;

		case PRECHARGE:
			Precharge(cell->cell_voltage, &extermly_imbalance_bit, &cell->SW1);
			/**************״̬��ת*****************/
			sum_of_cell = SumOfCell(cell->cell_voltage);
			if (sum_of_cell >= VLOWV) {
				state_of_charger = EQUAL_CURRENT;
			}
			break;

		case EQUAL_CURRENT:
			EqualCurrent(cell->cell_voltage, &extermly_imbalance_bit, &cell->SW1);
			/**************״̬��ת*****************/
			sum_of_cell = SumOfCell(cell->cell_voltage);
			extermly_imbalance_flag = ExtermlyImbalanceFlag(cell->cell_voltage, extermly_imbalance_bit);
			if (extermly_imbalance_flag) {
				state_of_charger = EXTREMLY_IMBALANCE;
			}
			else if (sum_of_cell >= VRECH) {  //�����Ѿ�������ѹ���ݸ���,ʹ����һʱ�̵����ݻ�����ӳ�
				state_of_charger = EQUAL_VOLTAGE;
			}
			break;

		case EQUAL_VOLTAGE:
			EqualVoltage(cell->cell_voltage, &cell->SW1);
			/**************״̬��ת*****************/
			sum_of_cell = SumOfCell(cell->cell_voltage);
			StateConvert(sum_of_cell, &cell->stat1, &cell->stat2);  //�ò�����Ӳ���Ͽ��Զ����
			if (cell->stat1 == 0 && cell->stat2 == 1) {
				state_of_charger = CHARGE_COMPLETE;
			}
			break;

		default: BatteryAbsent(cell->cell_voltage, &cell->stat1, &cell->stat2, &cell->SW1);
		}

		cout << endl;

		outData << "i " << cell->cell_voltage[0] << " " << cell->cell_voltage[1] << " " << cell->cell_voltage[2] << " " \
			<< cell->cell_voltage[3] << " " << cell->bat << " " << cell->stat1 << " " << cell->stat2 << " " << cell->SW1 << " o\n";

		//����ѭ��
		if (state_of_charger == BATTERY_ABSENT) {
			break;
		}
	}
	outData.close();//�Ӽ������벢д���ļ�

	delete cell;
}

void StateConvert(double sum, int* stat1, int* stat2)
{
	if (sum >= 16.8) {
		*stat1 = 0;
		*stat2 = 1;
	}
}