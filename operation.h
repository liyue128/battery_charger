#pragma once
#ifndef _OPERATION_H
#define _OPERATION_H

//#define STAT1 PXin(x1)
//#define STAT2 PXin(x2)

void EnableTimer(void);
void DisableTimer(void);
float MaxOfCell(float cell[]);
float MinOfCell(float cell[]);
float SumOfCell(float cell[]);

void ChargeAbsent();
void ChargeAbnormal();
void ChargeComplete();
void Precharge();
void Fastcharge();
void Discharge();

bool AbnormalDectect();
void AdcCorrect();
void CalculateVoltage();
void UpdateVoltage(TypeOfStruct* cell_structure);

#endif
