#include "bq76925.h"
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "delay.h"



void RegisterReset(void);
void ADC_Calibrate(void);

void bq76925_init(void)
{
	//向监控芯片内寄存器写入默认值
	RegisterReset();
	
	//使监控芯片开始正常工作
	I2C1_Write(POWER_CTL, 0x41);  //enable SLEEP_DIS, REF_EN.
	
	//选择参考电压--3V
	I2C1_Write(CONFIG_2, 0x01);
	
	//校准参考电压
	ADC_Calibrate();
	
	//将VCOUT配置成cell电压输出模式
	I2C1_Write(CELL_CTL, 0x10);
	
	//选择VCOUT增益
}

void RegisterReset(void)
{
	//reset STATUS REGISTER
	I2C1_Write(STATUS_REGISTER, 0x01);
	
	//reset CELL_CTL
	I2C1_Write(CELL_CTL, 0x00);
	
	//reset BAT_CTL
	I2C1_Write(BAT_SEL, 0x00);
	
	//reset CONFIG_1
	I2C1_Write(CONFIG_1, 0x00);
	
	//reset CONFIG_2
	I2C1_Write(CONFIG_2, 0x01);
	
	//reset POWER_CTL
	I2C1_Write(POWER_CTL, 0x00);
	
	//reset CHIP_ID
	I2C1_Write(CHIP_ID, 0x10);
}	

//应多测几组数组
void ADC_Calibrate(void)
{
	u16 adc_count,full_scale_count=0x0400;
	float vref1, vref2;
	float gain_error, offset_error;
	u8  vref_gain_corr, vref_offset_corr,vref_gc_4, vref_oc_4, vref_oc_5;
	
	//测量VREF*0.5电压值
	I2C1_Write(CELL_CTL,0x20);
	delay_ms(10);
	adc_count=SPI_ReceiceData() >> 2;
	vref1=((adc_count-0.5)/full_scale_count)*VREF_NOMINAL;
	
	//测量VREF*0.85电压值
	I2C1_Write(CELL_CTL,0x30);
	delay_ms(10);
	adc_count=SPI_ReceiceData() >> 2;
	vref2=(adc_count-0.5)*VREF_NOMINAL/full_scale_count;
	
	//计算校准参数
	gain_error=(vref2-vref1)/(0.85*VREF_NOMINAL-0.5*VREF_NOMINAL)-1;
	offset_error=vref1-(1+gain_error)*0.5*VREF_NOMINAL;
	
	vref_gain_corr=(u8)(gain_error*1000);
	vref_offset_corr=(u8)(offset_error*1000);
	
	vref_gc_4=(vref_gain_corr>>4) & 0x01;
	vref_gain_corr &= 0x0F;
	vref_oc_4=(vref_offset_corr>>4) & 0x01;
	vref_oc_5=(vref_offset_corr>>5) & 0x01;
	vref_offset_corr &= 0x0F;
	
	//向bq76925写入误差参数
	I2C1_Write(VREF_CAL, vref_gain_corr+(vref_offset_corr<<4));
	I2C1_Write(VREF_CAL_EXT, vref_gc_4+(vref_oc_4<<1)+(vref_oc_5<<2));
}
