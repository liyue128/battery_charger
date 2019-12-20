#pragma once
#ifndef _OPERATION_H
#define _OPERATION_H

void EnableTimer(void);
void DisableTimer(void);
double MaxOfCell(double cell[]);
double MinOfCell(double cell[]);
double SumOfCell(double cell[]);
void SwStart(bool* SW1);
void SwEnd(bool* SW1);
void NotInExtrem(bool* SW1);
void InExtrem(bool* SW1);
void NotDetectBat(bool* SW1);

#endif
