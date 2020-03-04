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

//ʹ�ܼ�ʱ��
void EnableTimer(void)
{
	//����ʱ��δ��������ʹ�ܼ�ʱ��
	cout << "if DISABLE, start timer.\n";
}

//���ü�ʱ��
void DisableTimer(void)
{
	//����ʱ���ѿ���,����ü�ʱ��
	cout << "if ENABLE,end timer.\n";
}

//���ѹ���ֵ
float MaxOfCell(float cell[])
{
	float max = cell[0];
	for (int i = 0; i < NUM; i++)
		if (cell[i] > max) max = cell[i];
	return max;
}

//���ѹ��Сֵ
float MinOfCell(float cell[])
{
	float min = cell[0];
	for (int i = 0; i < NUM; i++)
		if (cell[i] < min) min = cell[i];
	return min;
}

//���ܵ�ѹ
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
	//��ʱupdate cell_voltage-- if(count>=20) {UpdateVoltage(cell_structure);count=0;}
	//indicate absent state--����� LED_RED=!LED_RED;
}

void ChargeAbnormal()
{
	//CE=0;
	//TIM_Cmd(TIMx1, DISABLE);TIM_Cmd(TIMx2, DISABLE);TIM_Cmd(TIMx3, DISABLE);
	//indicate abnormal state--��Ƴ���:LED_RED=1;
}

void ChargeComplete()
{
	//CE=0
	//disable fastcharge_timer--TIM_Cmd(TIMx2, DISABLE);
	//indicate complete state--�̵Ƴ���:LED_GREEN=1
}

void Precharge()
{
	//CE=1
	//TIM_Cmd(TIMx2, DISABLE);
	//TIM_Cmd(TIMx3, DISABLE);
	//if(!(TIMx1_TypeDef->CR1&0x01)) {TIM_Cmd(TIMx1, ENABLE);}
	//indicate charge in process--�̵���˸:LED_GREEN=!LED_GREEN;
}

void Fastcharge()
{
	//CE=1
	//TIM_Cmd(TIMx1, DISABLE);
	//TIM_Cmd(TIMx3, DISABLE);
	//if(!(TIMx2_TypeDef->CR1&0x01)) {TIM_Cmd(TIMx2, ENABLE);}
	//indicate charge in process--�̵���˸:LED_GREEN=!LED_GREEN;
}

void Discharge()
{
	//CE=0
	//TIM_Cmd(TIMx1, DISABLE);
	//TIM_Cmd(TIMx2, DISABLE);
	//if(!(TIMx3_TypeDef->CR1&0x01)) {
	//TIM_Cmd(TIMx3, ENABLE);
	//�ŵ�ӿ� CellBypass(data_structure);}
	//if(timer_out)	CloseBypass();
	//indicate charge in process--�̵���˸:LED_GREEN=!LED_GREEN;
}


bool AbnormalDectect()
{
	//�쳣��⺯���������쳣��־λ, flag����Ϊstatic
	static bool flag = false;
	//if((stat1=OFF && stat2=OFF) ||  �����������(���洢���ֵ���ֵ���ò��ֿ���ʱ����)) flag=true;
	//if((TIMx3_TypeDef->SR & 0x02) && sum_of_voltage<VLOWV)  flag=true;  //�ο����̣�ȷ���Ƿ�ʹ��CC1Fλ�жϼ�ʱ�����
	//if(TIMx2_out && sum_of_voltage<VRECH)  flag=true;
	return flag;
}

/*int AdcData()
{
	//��ADC���ݼĴ����е�������ȡ������Ϊdata(16 bit)
	if(!(data<<14==0) &&  (data>>12==0)) {
		//baocuo
	}
	for(int i=0;i<10;i++) {  //��������ת��Ϊʮ����
		oprate=data%2;
		data>>=2;
		result+=operate<<2;
	}
	return result
}
*/

void AdcCorrect()
{
	/*//����ADC��������ƫ�����
	//���Ƚ������������ź�ת��Ϊ��ѹֵ
	//��ȡ�ο���ѹΪ0.5*Vref��0.85*Vref��ADC��ѹֵV1,V2
	//���ݵ�ѹ���߼�����������ƫ��
	Gerror=(V2-V1)/(0.85*Vref-0.5*Vref)
	OFFSET=V1-Gerror*(0.5*Vref)
	������������ƫ��
	*/
}

void CalculateVoltage()
{
	/*calculate voltages of cells
	ÿ��ѭ����Ҫ�õ�һ���ѹ��������,����Щ���ݽ����ֵ�ѹֵ��Ҫ���¼���
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
	//��ѹ���º���,��������n�ε�ѹ,ȡƽ��ֵ
	/*double sum;

	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 5; i++) {
			cell_structure->voltage[i] = CalculateVoltage();
			sum += cell_structure->voltage[i];
		}
		cell_structure->voltage[j] = sum / 5;
	}*/
}
