//#include "main.h"
#include "operation.h"
#include "charger.h"
#define T 5
#define NUM 4
#define BATTERY_ABSENT 0
#define EXTREMLY_IMBALANCE 1
#define CHARGE_COMPLETE 2
#define CHARGE_ABNORMAL 3
#define PRECHARGE 4
#define EQUAL_CURRENT 5
#define EQUAL_VOLTAGE 6

int main()
{
	double cell_voltage[NUM];
	int extermly_imbalance_flag = 0;
	int extermly_imbalance_bit = 0;
	int t = 0;
	int state_of_charger = 0;
	int bat, stat1, stat2;



	while (1) {
		//数据接收端口,接收到的数据包括cell_voltage[NUM],bat,stat1,stat2
		DataCollection(cell_voltage, &bat, &stat1, &stat2);
		//电池检测接口
//		battery_detction_bit = BatteryDetect(cell_voltage, &bat, &stat1, &stat2);
		if (t < T) {
			if (bat) {
				//极端不平衡状态接口
				extermly_imbalance_flag = ExtermlyImbalanceFlag(cell_voltage, extermly_imbalance_bit);
				state_of_charger = ChargeStateJudge(bat, extermly_imbalance_flag, cell_voltage, stat1, stat2);
			}
			t++;
		}
		else {
			t = 0;
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
			Precharge();
			break;
		case EQUAL_CURRENT: 
			EqualCurrent(cell_voltage);
			break;
		case EQUAL_VOLTAGE: 
			EqualVoltage();
			break;
		default: BatteryAbsent();
		}
	}
	return 0;
}

