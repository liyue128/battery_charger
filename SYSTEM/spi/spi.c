#include "spi.h"

//参考文章:http://news.eeworld.com.cn/mcu/2019/ic-news030843389.html
void spi1_init(void)
{
	/* 使能与 SPI1 有关的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	/* PA6-MISO*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入,适用于ADC输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* PA5-SCK, PA7-M0SI(只接受数据模式下该引脚空闲) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*	PA4-NSS	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* interrupt config */
	NVIC_InitStructure.NVIC_IRQChannel                   = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
 
	 /* SPI 模式配置 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly;  //仅接收模式
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
 
	 /* 使能 SPI */
	SPI_Cmd(SPI1, ENABLE);
}

void SPI1_IRQHandler(void) 
{
	if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_OVR)) {
		SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_OVR);
	}
}

u16 SPI_ReceiceData(void)
{
	//CS引脚变为低电平
	SPI_CS_LOW();
	
	//delay_ms(500);
	
	//等到RXNE=1
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){;}
	
	//CS引脚变为高电平
	SPI_CS_HIGH();
		
	
	return SPI_I2S_ReceiveData(SPI1);
}

