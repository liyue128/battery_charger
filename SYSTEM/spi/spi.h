#ifndef _SPI_H
#define _SPI_H

#include "sys.h"

#define SPI_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)

void spi1_init(void);
void SPI1_IRQHandler(void);
u16 SPI_ReceiceData(void);

#endif
