#ifndef __I2C_H
#define __I2C_H

#include "sys.h"

#define I2C_REC_LEN 200


#define I2C1_MODE_WAITING		0    // Waiting for commands
#define I2C1_MODE_SLAVE_ADR_WR	1	 // Received slave address (writing)
#define I2C1_MODE_ADR_BYTE		2    // Received ADR byte
#define I2C1_MODE_DATA_BYTE_WR	3    // Data byte (writing)
#define I2C1_MODE_SLAVE_ADR_RD	4 	 // Received slave address (to read)
#define I2C1_MODE_DATA_BYTE_RD	5    // Data byte (to read)


void i2c1_init(void);
void dma_i2c1_init(void);

void I2C1_Write(u8 addr, u8 data);
u8 I2C1_Read(u8 nAddr);

#endif
