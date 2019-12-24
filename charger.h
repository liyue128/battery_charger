#pragma once
#ifndef _CHARGER_H
#define _CHARGER_H

#define NUM 4

void CellBypass(float cell[NUM], float min);
void OpenAllBypass(void);
void CellBalancing(TypeOfStruct* cell_structure, float min);

bool ExtermlyImbalanceFlag(TypeOfStruct* cell_structure, bool extermly_imbalance_flag);
int ChargeStateJudge(TypeOfStruct * cell_structure, int extermly_imbalance_bit);

void BatteryAbsent(TypeOfStruct* cell_structure);
int DischargeInExtrem(TypeOfStruct* cell_structure);
void ChargeComplete(TypeOfStruct* cell_structure);
void Abnormal(TypeOfStruct* cell_structure);
void InCharging(TypeOfStruct* cell_structure);
void Precharge(TypeOfStruct* cell_structure, bool* flag);
void EqualCurrent(TypeOfStruct* cell_structure, bool* flag);
void EqualVoltage(TypeOfStruct* cell_structure);


#endif
