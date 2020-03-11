# battery_charger
a 4 cell charger using STM32F103CB

MCU在充电期间通过BQ76925(TI)与ADCS7477(TI)实现对电池电压的监控, 并根据电压状态以及充电芯片(BQ24616)的充电状态决定是否关闭充电芯片。
