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
	std::string str;          //�Ƿ���Է���������ǰ����
	if (file) // �и��ļ�
	{
		istreambuf_iterator<char> beg(file), end;
		string strdoc(beg, end);                                  //���ļ�������ȫ����ȡ��str֮��
		for (int i = 0; i < strdoc.size() - 28; i++) 
		{
			if (strdoc[i] == 'i' && strdoc[i + 28] == 'o') str = strdoc.substr(i, i + 28);
		}
	}
	
	return str;
	
}

#endif
