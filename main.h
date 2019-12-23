#pragma once
#ifndef _MAIN_H
#define _MAIN_H

#include <string>

#define NUM 4
#define RECH 4.10
#define VLOWV 12.40
#define VRECH 16.40
#define T1 1000
#define T2 1000
#define T3 1000
#define T4 1000
#define T5 800	

enum {
	BATTERY_ABSENT,
	EXTREMLY_IMBALANCE,
	CHARGE_COMPLETE,
	CHARGE_ABNORMAL,
	PRECHARGE,
	EQUAL_CURRENT,
	EQUAL_VOLTAGE,
};

struct TypeOfStruct
{
	bool bat, stat1, stat2;
	float cell_voltage[NUM];
	bool SW1;

	TypeOfStruct()
	{
		memset(this, 0, sizeof(TypeOfStruct));  //结构体初始化
	}
};//定义结构体

#endif
