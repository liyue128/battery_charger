#include "batterydata.h"

void Batterydata::CellVoltage(double voltage_of_cell[NUM])
{
	string str = Batterydata::GetBatteryData();

	for (int i = 0; i < NUM; i++) {
		string cut = str.substr(5 * i + 2, 4);
		voltage_of_cell[i] = atof(cut.c_str());
	}
}

int Batterydata::LevelOfAfe()
{
	string str = Batterydata::GetBatteryData();
	int bat = str[22] - '0';
	return bat;
}

int Batterydata::levelOfStat1()
{
	string str = Batterydata::GetBatteryData();
	int bat = str[24] - '0';
	return bat;
}

int Batterydata::levelOfStat2()
{
	string str = Batterydata::GetBatteryData();
	int bat = str[26] - '0';
	return bat;
}
