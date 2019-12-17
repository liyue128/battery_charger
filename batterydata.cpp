#include "batterydata.h"

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
