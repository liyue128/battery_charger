#include "bsp_usart1.h"
//#include "stdio.h"
//#include "delay.h"
//#include "main.h"
#include <string.h>

int main(void)
{
	//u32 t=0;
	//u8 ref=0;
	
	USART1_Config();
	
	//ref=I2C1_Read(VREF_CAL);
	printf("hello,world!");
	
	while(1){
		//printf("t:%d\r\n",t);
		//delay_ms(500);
		//t++;
		;
	}
}


