#pragma once
#ifndef _MAIN_H
#define _MAIN_H

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define NUM 4
#define D1 0.02
#define D2 0.08
#define RECH 4.10
#define VLOWV 12.40
#define VRECH 16.40
/*#define BATTERY_ABSENT 0
#define EXTREMLY_IMBALANCE 1
#define CHARGE_COMPLETE 2
#define CHARGE_ABNORMAL 3
#define PRECHARGE 4
#define EQUAL_CURRENT 5
#define EQUAL_VOLTAGE 6*/
#define T0 50
#define T1 100
#define T2 100
#define T3 100
#define T4 100
#define T5 80

void StateConvert(double sum, int* stat1, int* stat2);

enum {
	BATTERY_ABSENT,
	EXTREMLY_IMBALANCE,
	CHARGE_COMPLETE,
	CHARGE_ABNORMAL,
	PRECHARGE,
	EQUAL_CURRENT,
	EQUAL_VOLTAGE,
};

class Batterydata  // class declaration
{
private:
	string GetBatteryData(void);
public:
	void CellVoltage(double voltage_of_cell[NUM]);
	int LevelOfAfe(void);
	int levelOfStat1(void);
	int levelOfStat2(void);
	int levelOfSw(void);
};    // note semicolon at the end

#endif
