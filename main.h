#pragma once
#ifndef _MAIN_H
#define _MAIN_H


#include <string>

#define NUM 4
#define RECH 4.10
#define VLOWV 12.40
#define VRECH 16.40
#define T0 100
#define T1 200
#define T2 200
#define T3 350
#define T4 300
#define T5 350	

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
	bool bat, stat1, stat2, SW1;
	float cell_voltage[NUM];
	int t;

	TypeOfStruct()
	{
		memset(this, 0, sizeof(TypeOfStruct));  //结构体初始化
	}
};//定义结构体

#endif
