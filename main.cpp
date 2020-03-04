#include "main.h"
#include <iostream>
#include <Windows.h>
//#include <time.h>  //计算实时时间需用到
#include "operation.h"
#include "charger.h"
#define MCU_TIMEROUT 100
#define CHIP_TIMEROUT 500
#define CYCLE_TIME 0.5
#define OFF 0
#define ON 1
#define D1 0.02

using namespace std;


int main()
{
	TypeOfStruct* charger_data_structure;
	charger_data_structure = new TypeOfStruct;
	bool charge_abnormal_flag = false;
	int status_of_charger = 0;
	double battery_voltage = 0.0, max_cell_voltage=0.0, min_cell_voltage=0.0;
	bool doc_eof = false;
	//uint8_t time_stage1, time_stage2;  //acquire current timestamp

	bool precharge_timerout=false, fastcharge_timerout=false, discharge_timerout=false;  //定义充电计时器超时标志位

	//数据接收端口,接收并解析数据
	//*charger_data_structure = DataReceive(&doc_eof);

	//初始化
	/*timer_init();
	i2c_init();
	spi_init();
	gpio_init();
	*/

	//计算ADC误差并将计算值返回到监控芯片内部存储

	while (1) {
		
		// uint8_t time_stage1 = rtcTime.Seconds;  //acquire current timestamp

		//每次while(1)循环,必先检查bat;如果处于precharge or fastcharge,再检查异常
		//if(reset)	status_of_charger=BATTERY_ABSENT;
		//u8 bat=GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, u16 GPIO_Pin);
		if (status_of_charger == PRECHARGE || status_of_charger == FASTCHARGE || status_of_charger == CHARGE_COMPLETE) {
			charge_abnormal_flag = AbnormalDectect();
		}

		switch (status_of_charger)
		{
		case BATTERY_ABSENT:
			ChargeAbsent();
			/**************状态跳转*****************/
			if (!charger_data_structure->bat) {  
				status_of_charger = BATTERY_ABSENT;
			}
			else if (battery_voltage < VLOWV) {
				status_of_charger = PRECHARGE;
			}
			else if(max_cell_voltage - min_cell_voltage < D1) {
				status_of_charger = FASTCHARGE;
			}
			else {
				status_of_charger = DISCHARGE;
			}
			break;

		case CHARGE_ABNORMAL:
			ChargeAbnormal();
			/**************状态跳转*****************/
			if (!charger_data_structure->bat) {  //|| SYSTEM_RESET
				status_of_charger = BATTERY_ABSENT;
			}
			else {
				status_of_charger = CHARGE_ABNORMAL;
			}
			break;

		case CHARGE_COMPLETE:
			ChargeComplete();
			/**************状态跳转*****************/
			if (!charger_data_structure->bat) {  //|| SYSTEM_RESET
				status_of_charger = BATTERY_ABSENT;
			}
			else if (charge_abnormal_flag)
			{
				status_of_charger = CHARGE_ABNORMAL;
			}
			else {
				status_of_charger = CHARGE_COMPLETE;
			}
			break;

		case PRECHARGE:
			Precharge();
			/**************状态跳转*****************/
			if (!charger_data_structure->bat) {  //|| SYS_RESET
				status_of_charger = BATTERY_ABSENT;
			}
			else if (charge_abnormal_flag) {
				status_of_charger = CHARGE_ABNORMAL;
			}
			else if (!precharge_timerout) {
				status_of_charger = PRECHARGE;
			}
			else if (precharge_timerout) {
				UpdateVoltage(charger_data_structure);  //更新电池电压数据
				if (max_cell_voltage - min_cell_voltage >= D1) {
					status_of_charger = DISCHARGE;
				}
				else {
					status_of_charger = FASTCHARGE;
				}
			}
			break;

		case FASTCHARGE:
			Fastcharge();
			/**************状态跳转*****************/
			if (!charger_data_structure->bat) {  //|| SYS_RESET
				status_of_charger = BATTERY_ABSENT;
			}
			else if (charge_abnormal_flag) {
				status_of_charger = CHARGE_ABNORMAL;
			}
			else if (charger_data_structure->stat1 == OFF && charger_data_structure->stat2 == ON) {
				status_of_charger = CHARGE_COMPLETE;
			}
			else {
				status_of_charger = FASTCHARGE;
			}
			break;

		case DISCHARGE:  //在该模式下,没有考虑异常情况
			Discharge();
			/**************状态跳转*****************/
			if (!charger_data_structure->bat) {  //|| SYS_RESET
				status_of_charger = BATTERY_ABSENT;
			}
			else if (!discharge_timerout) {
				status_of_charger = DISCHARGE;
			}
			else {
				UpdateVoltage(charger_data_structure);  //更新电池电压数据
				if (battery_voltage < VLOWV) {
					status_of_charger = PRECHARGE;
				}
				else if (max_cell_voltage - min_cell_voltage < D1) {
					status_of_charger = FASTCHARGE;
				}
				else {
					status_of_charger = DISCHARGE;
				}
			}
			break;
		
		default: ChargeAbsent();
		}
		
	}

	// uint8_t time_stage2 = rtcTime.Seconds;  //acquire current timestamp
	//if(T>time_stage2-time_stage1) delay(T-(time_stage2-time_stage1));


	delete charger_data_structure;
	return 0;
}