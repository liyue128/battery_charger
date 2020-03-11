#include "i2c.h"
#include "sys.h"

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C1_OWN_ADDRESS7      0X0c 
/* STM32 I2C 快速模式 */
#define I2C_400KHZ             400000  
#define I2C_100KHZ			   100000


u8 i2c_data[I2C_REC_LEN];

/****************************************************
						I2C1
****************************************************/
void i2c1_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure; 

	/* 使能与 I2C1 有关的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);  
    
  /* PB6-I2C1_SCL、PB7-I2C1_SDA*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* interrupt config */
	NVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
  /* I2C 配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 =I2C1_OWN_ADDRESS7; 
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	/* I2C的寻址模式 */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	/* 通信速率 */
  I2C_InitStructure.I2C_ClockSpeed = I2C_100KHZ;
	/* I2C1 初始化 */
  I2C_Init(I2C1, &I2C_InitStructure);
	I2C_DMACmd(I2C1, ENABLE);
	I2C_ITConfig(I2C1, I2C_IT_BUF | I2C_IT_EVT, ENABLE);
	/* 使能 I2C1 */
  I2C_Cmd(I2C1, ENABLE);   
	
}

void dma_i2c1_init()
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
  DMA_DeInit(DMA1_Channel7);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&I2C1->DR); 
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)i2c_data;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
	DMA_InitStructure.DMA_BufferSize = I2C_REC_LEN;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	DMA_Cmd(DMA1_Channel7,ENABLE);	
}


void I2C1_ClearFlag(void) {
  // ADDR-Flag clear
  while((I2C1->SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR) {
    I2C1->SR1;
    I2C1->SR2;
  }

  // STOPF Flag clear
  while((I2C1->SR1&I2C_SR1_STOPF) == I2C_SR1_STOPF) {
    I2C1->SR1;
    I2C1->CR1 |= 0x1;
  }
}

uint32_t idx = 0;
unsigned char i2c1_mode=I2C1_MODE_WAITING;
void I2C1_EV_IRQHandler(void)
{
	//uint8_t wert;
	uint32_t event;
	
	event=I2C_GetLastEvent(I2C1);
	
	if(event==I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED) {
    // Master has sent the slave address to send data to the slave
    i2c1_mode=I2C1_MODE_SLAVE_ADR_WR;
		//idx = 0;
  }
  else if(event==I2C_EVENT_SLAVE_BYTE_RECEIVED) {
    // Master has sent a byte to the slave
    //wert=I2C_ReceiveData(I2C1);
		//i2c_data[idx++] = I2C_ReceiveData(I2C1);
		
    // Check address
    if(i2c1_mode==I2C1_MODE_SLAVE_ADR_WR) {
      i2c1_mode=I2C1_MODE_ADR_BYTE;
      // Set current ram address
      
    }
    else {
      i2c1_mode=I2C1_MODE_DATA_BYTE_WR;
      // Store data in RAM
      
    }
  }
  else if(event==I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED) {
    // Master has sent the slave address to read data from the slave
    
  }
  else if(event==I2C_EVENT_SLAVE_BYTE_TRANSMITTED) {
    // Master wants to read another byte of data from the slave
    
  }
  else if(event==I2C_EVENT_SLAVE_STOP_DETECTED) {
    // Master has STOP sent
		I2C1_ClearFlag();
    i2c1_mode=I2C1_MODE_WAITING;
		
		DMA_Cmd(DMA1_Channel7,DISABLE);
		PBout(3)=0;
		DMA_SetCurrDataCounter(DMA1_Channel7, I2C_REC_LEN);
		DMA_Cmd(DMA1_Channel7,ENABLE);
  }
}

void I2C1_ER_IRQHandler(void) {
  if (I2C_GetITStatus(I2C1, I2C_IT_AF)) {
    I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
  }
}


void I2C1_Write(u8 addr, u8 data)
{
	/*  */ 
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); 
	
	/*	产生START信号	*/
	I2C_GenerateSTART(I2C1,ENABLE); 
	
	/*	EV5, check start signal,master mode,BUS busy	*/
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;}
	
		/*	发送从机地址	*/	
	I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter); 
		
	/*	EV6	*/
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;} 
	
	/*	send data	*/	
	I2C_SendData(I2C1,data);
		
	/*	EV8	*/	
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
		
	/* 发送当前地址 */
	//I2C_SendData(I2C1, *pBuffer); 
		
	/* EV8_2 */	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
		
	/* generate stop */	
	I2C_GenerateSTOP(I2C1,ENABLE); 
}
 
//参考https://blog.csdn.net/qq_41281601/article/details/81671455
u8 I2C1_Read(u8 nAddr)
{
  //*((u8 *)0x4001080c) |=0x80; 
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008    
    
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send slave address for write */
  I2C_Send7bitAddress(I2C1, nAddr, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  //I2C_Cmd(I2C1, ENABLE);
	
	/* NACK */	
	I2C_AcknowledgeConfig(I2C1,DISABLE);
		
	/* generate stop signal */
	I2C_GenerateSTOP(I2C1,ENABLE);
	
	return I2C_ReceiveData(I2C1);
}
