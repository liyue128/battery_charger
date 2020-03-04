#pragma once
#ifndef _MAIN_H
#define _MAIN_H


#include <string>

#define NUM 4
#define RECH 4.10
#define VLOWV 12.40
#define VRECH 16.40
#define T0 50
#define T1 200
#define T2 200
#define T3 350
#define T4 300
#define T5 350	
#define I_PRECHARGE 0.1
#define I_EQUAL_I 0.4
#define I_EQUAL_V 0.1
#define INNER_BYP 0.1
#define OUTER_BYP 0.1

enum {
	BATTERY_ABSENT,
	CHARGE_ABNORMAL,
	CHARGE_COMPLETE,
	PRECHARGE,
	FASTCHARGE,
	DISCHARGE,
};

volatile struct TypeOfStruct
{
	bool bat, stat1, stat2, SW1;
	float cell_voltage[NUM];
	float cell_current[NUM];

	TypeOfStruct()
	{
		memset(this, 0, sizeof(TypeOfStruct));  //结构体初始化
	}
};//定义结构体
TypeOfStruct DataReceive(bool* finish);

#endif
