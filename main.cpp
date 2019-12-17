#include "main.h"
#include <iostream>
#include "operation.h"
#include "charger.h"
#define T 3
#define BATTERY_ABSENT 0
#define EXTREMLY_IMBALANCE 1
#define CHARGE_COMPLETE 2
#define CHARGE_ABNORMAL 3
#define PRECHARGE 4
#define EQUAL_CURRENT 5
#define EQUAL_VOLTAGE 6

int main()
{
	double cell_voltage[NUM] = { 0 };
	bool extermly_imbalance_flag = 0;
	bool extermly_imbalance_bit = 0;
	int t = 1;
	int state_of_charger = 0;
	int bat = 0, stat1 = 0, stat2 = 0;

	while (1) {
		//数据接收端口,接收到的数据包括cell_voltage[NUM],bat,stat1,stat2
		DataCollection(cell_voltage, &bat, &stat1, &stat2);
		if (t < T) {
			if (bat) {
				std::cout << "delay " << T - t << "*Tn" << std::endl;  //该阶段消耗的时间为(T-1)*Tn
				//判断极端不平衡状态
				extermly_imbalance_flag = ExtermlyImbalanceFlag(cell_voltage, extermly_imbalance_bit);  //extermly_imbalance_bit表示上一次循环是否为极端不平衡
				state_of_charger = ChargeStateJudge(bat, extermly_imbalance_flag, cell_voltage, stat1, stat2);
				t = 0;
			}
			t++;
		}
		else {
			t = 1;
			state_of_charger = BATTERY_ABSENT; 
		}

		switch (state_of_charger)
		{
		case BATTERY_ABSENT: 
			BatteryAbsent();
			break;
		case EXTREMLY_IMBALANCE: 
			extermly_imbalance_bit = DischargeInExtrem(cell_voltage);
			break;
		case CHARGE_COMPLETE: 
			ChargeComplete();
			break;
		case CHARGE_ABNORMAL: 
			Abnormal();
			break;
		case PRECHARGE: 
			Precharge(&extermly_imbalance_bit);
			break;
		case EQUAL_CURRENT: 
			EqualCurrent(cell_voltage, &extermly_imbalance_bit);
			break;
		case EQUAL_VOLTAGE: 
			EqualVoltage();
			break;
		default: BatteryAbsent();
		}
		std::cout << std::endl;
	}
	return 0;
}

