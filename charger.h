#pragma once
#ifndef _CHARGER_H
#define _CHARGER_H

#define NUM 4

void DataCollection(double cell_voltage[NUM], int* bat, int* stat1, int* stat2, bool* SW1);

void CellBypass(double cell[NUM], double min);
void OpenAllBypass(void);
void CellBalancing(double cell[NUM], double min);

bool ExtermlyImbalanceFlag(double cell_voltage[NUM], bool extermly_imbalance_flag);
int ChargeStateJudge(int battery_detction, int extermly_imbalance_bit, double voltage[NUM], int stat1, int stat2);

void BatteryAbsent(double cell[NUM], int* stat1, int* stat2, bool* SW1);
int DischargeInExtrem(double cell_voltage[NUM], bool* SW1);
void ChargeComplete(bool* SW1);
void Abnormal(bool* SW1);
void InCharging(bool* SW1);
void Precharge(double cell[NUM], bool* flag, bool* SW1);
void EqualCurrent(double cell_voltage[NUM], bool* flag, bool* SW1);
void EqualVoltage(double cell[NUM], bool* SW1);


#endif
