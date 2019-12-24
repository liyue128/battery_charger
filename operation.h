#pragma once
#ifndef _OPERATION_H
#define _OPERATION_H

void EnableTimer(void);
void DisableTimer(void);
float MaxOfCell(float cell[]);
float MinOfCell(float cell[]);
float SumOfCell(float cell[]);
void SwStart(bool* SW1);
void SwEnd(bool* SW1);
void BeforeCharging(bool* SW1);
void InExtrem(bool* SW1);
void NotDetectBat(bool* SW1);

#endif
