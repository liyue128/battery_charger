#include "main.h"

inline string Batterydata::GetBatteryData()  //内联函数
{
	ifstream file;
	file.open("d:\\test\\i\\extrem.txt", ios::in);
	//	if (!file.is_open())          //如果文件未正确打开
	//         cout << "failed!" << endl;
	std::string str;          //是否可以放在主函数前声明
	if (file) // 有该文件
	{
		istreambuf_iterator<char> beg(file), end;
		string strdoc(beg, end);                                  //将文件的内容全部读取到str之中
		for (int i = 0; i < strdoc.size() - 30; i++)
		{
			if (strdoc[i] == 'i' && strdoc[i + 30] == 'o') str = strdoc.substr(i, i + 30);
		}
	}

	return str;
}

void Batterydata::CellVoltage(double voltage_of_cell[NUM])
{
	string str = Batterydata::GetBatteryData();

	if (!str.empty()) {
		for (int i = 0; i < NUM; i++) {
			string cut = str.substr(5 * i + 2, 4);
			voltage_of_cell[i] = atof(cut.c_str());
		}
	}
}

int Batterydata::LevelOfAfe()
{
	string str = Batterydata::GetBatteryData();
	int bat = 0;
	if (!str.empty()) {
		bat = str[22] - '0';
	}
	return bat;
}

int Batterydata::levelOfStat1()
{
	string str = Batterydata::GetBatteryData();
	int stat1 = 0;
	if (!str.empty()) {
		stat1 = str[24] - '0';
	}
	return stat1;
}

int Batterydata::levelOfStat2()
{
	string str = Batterydata::GetBatteryData();
	int stat2 = 0;
	if (!str.empty()) {
		stat2 = str[26] - '0';
	}
	return stat2;
}

int Batterydata::levelOfSw(void)
{
	string str = Batterydata::GetBatteryData();
	int SW1 = 0;
	if (!str.empty()) {
		SW1 = str[28] - '0';
	}
	return SW1;
}