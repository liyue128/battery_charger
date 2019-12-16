#pragma once
#ifndef _CHARGER_H
#define _CHARGER_H

#define NUM 4

void DataCollection(double cell_voltage[NUM], int* bat, int* stat1, int* stat2);
int ExtermlyImbalanceFlag(double cell_voltage[NUM], int extermly_imbalance_flag);
int DischargeInExtrem(double cell_voltage[NUM]);
void InCharging(void);
void BatteryAbsent(void);
void ChargeComplete(void);
void Abnormal(void);
void Precharge(void);
void EqualCurrent(double cell_voltage[NUM]);
void EqualVoltage(void);
void CellBalancing(double cell[NUM], double min);
void CellBypass(double cell[NUM], double min);
//int BatteryDetect(double cell_voltage[NUM], int* bat, int* stat1, int* stat2);
int ExtermlyImbalanceFlag(double cell_voltage[NUM], int extermly_imbalance_flag);
int ChargeStateJudge(int battery_detction, int extermly_imbalance_bit, double voltage[NUM], int stat1, int stat2);


#endif
