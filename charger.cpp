#include "main.h"
#include <iostream>
#include <fstream>
#include "charger.h"
#include "operation.h"
#include <Windows.h>
#include <time.h>
#define DATA_UPDATE_TIME 0.2
#define D1 0.02

using namespace std;
//TypeOfStruct DataReceive(string s);


TypeOfStruct DataReceive(bool *finish)  //模拟电压输入;
{
	TypeOfStruct cell;
#ifdef _I2C_H

	cell.cell_voltage[i]
		cell.bat
		cell.stat1
		cell.stat2
		cell.SW1

#else
	static int idx = 1;
	int icount = 0;
	float time_left;
	clock_t start_count, end_count;
	start_count = clock();
	ifstream file;

	string s;
	file.open("D:\\test\\i\\input1.txt", ios::in);
	if (!file.is_open())
		s = "";
	
	while (getline(file, s))
	{
		if (s.empty() || s[0] == '#')
			continue;
		icount++;
		if (icount == idx && s[0] == 'i' && s[30] == 'o') {
			for (int i = 0; i < NUM; i++) {
				for (int i = 0; i < NUM; i++) {
					string cut = s.substr(5 * i + 2, 4);
					cell.cell_voltage[i] = atof(cut.c_str());
					cell.cell_current[i] = 0;
				}
			}
			cell.bat = s[22] - '0';  //测试文件中不便模拟电池检测接口,暂时省略之
			cell.stat1 = s[24] - '0';
			cell.stat2 = s[26] - '0';
			cell.SW1 = s[28] - '0';
			break;
		}
	}
	if (file.eof()) *finish = true;
	idx++;

	end_count = clock();
	time_left = DATA_UPDATE_TIME - (end_count - start_count) * 1.0 / (CLOCKS_PER_SEC * 1000);
	if (time_left > 0) {
		Sleep(time_left * 1000);
	}
#endif  

	return cell;
}

//旁路接口
void CellBypass(TypeOfStruct* cell_structure)
{
	double min = MaxOfCell(cell_structure->cell_voltage);
	
	if (cell_structure->cell_voltage[0] - min > D1) {
		;  //开启相应放电旁路:BYPASS0=1
	}
	if (cell_structure->cell_voltage[1] - min > D1) {
		;  //开启相应放电旁路:BYPASS1=1
	}
	if (cell_structure->cell_voltage[2] - min > D1) {
		;  //开启相应放电旁路:BYPASS2=1
	}
	if (cell_structure->cell_voltage[3] - min > D1) {
		;  //开启相应放电旁路:BYPASS3=1
	}
	
}

/*
//关闭所有放电旁路
void CloseBypass()
{
	BYPASS0=0;
	BYPASS1=0;
	BYPASS2=0;
	BYPASS3=0;
}
*/




