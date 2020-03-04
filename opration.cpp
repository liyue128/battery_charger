#include "main.h"
#include <iostream>
#include "charger.h"

//#define CE PXout(x)

using namespace std;

/*calculate voltages of cells
double calcute(data)
{
	int VCOUT = ADC_Count/Full_Scale_Count*VREF_NOMINAL;

	int GC_VCOUT=[(VCn_GC_4<<4)+VCn_GAIN_CORR]*0.001;
	int OC_VCOUT=[(VCn_OC_4<<4)+VCn_OFFSET_CORR]*0.001;
	int GC_VREF=(1+[(VREF_GC_4<<4)+VREF_GAIN_CORR]*0.001)+\
	[(VREF_OC_5<<5)+(VREF_OC_4<<4)+VREF_OFFSET_CORR]*0.001/VREF_NOMINAL;

	VCn=(VCOUT*GC_VREF+OC_VCOUT)/G_VCOUT*(1+GC_VCOUT);
	return VCn;
}*/

//使能计时器
void EnableTimer(void)
{
	//若计时器未开启，则使能计时器
	cout << "if DISABLE, start timer.\n";
}

//禁用计时器
void DisableTimer(void)
{
	//若计时器已开启,则禁用计时器
	cout << "if ENABLE,end timer.\n";
}

//求电压最大值
float MaxOfCell(float cell[])
{
	float max = cell[0];
	for (int i = 0; i < NUM; i++)
		if (cell[i] > max) max = cell[i];
	return max;
}

//求电压最小值
float MinOfCell(float cell[])
{
	float min = cell[0];
	for (int i = 0; i < NUM; i++)
		if (cell[i] < min) min = cell[i];
	return min;
}

//求总电压
float SumOfCell(float cell[])
{
	float sum = 0;
	for (int i = 0; i < NUM; i++) sum += cell[i];
	return sum;
}

void ChargeAbsent()
{
	//static u8 count=0;
	//CE=0;
	//disable all timer--TIM_Cmd(TIMx, DISABLE);
	//定时update cell_voltage-- if(count>=20) {UpdateVoltage(cell_structure);count=0;}
	//indicate absent state--闪红灯 LED_RED=!LED_RED;
}

void ChargeAbnormal()
{
	//CE=0;
	//TIM_Cmd(TIMx1, DISABLE);TIM_Cmd(TIMx2, DISABLE);TIM_Cmd(TIMx3, DISABLE);
	//indicate abnormal state--红灯长亮:LED_RED=1;
}

void ChargeComplete()
{
	//CE=0
	//disable fastcharge_timer--TIM_Cmd(TIMx2, DISABLE);
	//indicate complete state--绿灯长亮:LED_GREEN=1
}

void Precharge()
{
	//CE=1
	//TIM_Cmd(TIMx2, DISABLE);
	//TIM_Cmd(TIMx3, DISABLE);
	//if(!(TIMx1_TypeDef->CR1&0x01)) {TIM_Cmd(TIMx1, ENABLE);}
	//indicate charge in process--绿灯闪烁:LED_GREEN=!LED_GREEN;
}

void Fastcharge()
{
	//CE=1
	//TIM_Cmd(TIMx1, DISABLE);
	//TIM_Cmd(TIMx3, DISABLE);
	//if(!(TIMx2_TypeDef->CR1&0x01)) {TIM_Cmd(TIMx2, ENABLE);}
	//indicate charge in process--绿灯闪烁:LED_GREEN=!LED_GREEN;
}

void Discharge()
{
	//CE=0
	//TIM_Cmd(TIMx1, DISABLE);
	//TIM_Cmd(TIMx2, DISABLE);
	//if(!(TIMx3_TypeDef->CR1&0x01)) {
	//TIM_Cmd(TIMx3, ENABLE);
	//放电接口 CellBypass(data_structure);}
	//if(timer_out)	CloseBypass();
	//indicate charge in process--绿灯闪烁:LED_GREEN=!LED_GREEN;
}


bool AbnormalDectect()
{
	//异常检测函数，返回异常标志位, flag可设为static
	static bool flag = false;
	//if((stat1=OFF && stat2=OFF) ||  电池寿命结束(检测存储部分的数值，该部分可暂时不管)) flag=true;
	//if((TIMx3_TypeDef->SR & 0x02) && sum_of_voltage<VLOWV)  flag=true;  //参考例程，确定是否使用CC1F位判断计时器溢出
	//if(TIMx2_out && sum_of_voltage<VRECH)  flag=true;
	return flag;
}

/*int AdcData()
{
	//将ADC数据寄存器中的数据提取出来设为data(16 bit)
	if(!(data<<14==0) &&  (data>>12==0)) {
		//baocuo
	}
	for(int i=0;i<10;i++) {  //将二进制转化为十进制
		oprate=data%2;
		data>>=2;
		result+=operate<<2;
	}
	return result
}
*/

void AdcCorrect()
{
	/*//计算ADC增益误差和偏移误差
	//首先将二进制数字信号转化为电压值
	//获取参考电压为0.5*Vref和0.85*Vref的ADC电压值V1,V2
	//根据电压曲线计算增益误差和偏移
	Gerror=(V2-V1)/(0.85*Vref-0.5*Vref)
	OFFSET=V1-Gerror*(0.5*Vref)
	返回增益误差和偏移
	*/
}

void CalculateVoltage()
{
	/*calculate voltages of cells
	每次循环都要用到一组电压计算数据,但这些数据仅数字电压值需要重新计算
	double calcute(data)
	{
		double VCOUT = ADC_Count/Full_Scale_Count*VREF_NOMINAL;
		double GC_VCOUT=[(VCn_GC_4<<4)+VCn_GAIN_CORR]*0.001;
		double OC_VCOUT=[(VCn_OC_4<<4)+VCn_OFFSET_CORR]*0.001;
		double GC_VREF=(1+[(VREF_GC_4<<4)+VREF_GAIN_CORR]*0.001)+\
		[(VREF_OC_5<<5)+(VREF_OC_4<<4)+VREF_OFFSET_CORR]*0.001/VREF_NOMINAL;
		double VCn=(VCOUT*GC_VREF+OC_VCOUT)/G_VCOUT*(1+GC_VCOUT);
		return VCn;
	}*/
}

void UpdateVoltage(TypeOfStruct* cell_structure)
{
	//电压更新函数,连续测量n次电压,取平均值
	/*double sum;

	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 5; i++) {
			cell_structure->voltage[i] = CalculateVoltage();
			sum += cell_structure->voltage[i];
		}
		cell_structure->voltage[j] = sum / 5;
	}*/
}
