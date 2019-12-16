#pragma once
#ifndef BATTERTDATA_H_
#define BATTERTDATA_H_

#include <iostream>
#include <fstream>
#include <string>
#define NUM 4
using namespace std;

class Batterydata  // class declaration
{
private:
	string GetBatteryData(void);
public:
	void CellVoltage(double voltage_of_cell[NUM]);
	int LevelOfAfe(void);
	int levelOfStat1(void);
	int levelOfStat2(void);
};    // note semicolon at the end

inline string Batterydata::GetBatteryData()  //��������
{
	ifstream file;
	file.open("D:\\data.txt", ios::in);
	//	if (!file.is_open())          //����ļ�δ��ȷ��
	//         cout << "failed!" << endl;
	std::string strLine, str;          //�Ƿ���Է���������ǰ����
	while(getline(file, strLine)){}
	if (strLine.size() >= 29) {
		for (long i = 0; i < strLine.size(); i++) {
			if (strLine[i] == 'i' && strLine[i + 28] == 'o') {
				for (int j = 0; j <= 28; j++) str[j] = strLine[i + j];
			}
		}
//		return str;
	}
	return str;
	
}

#endif
