#include <stdint.h>
#include <xc.h>
#include "I2C.h"


void E2PROM_24C04_Init(void);


void E2PROM_24C04_Write(uint8_t Device_Address, uint8_t word_Address , uint8_t Data);

uint8_t E2PROM_24C04_Read(uint8_t Device_Address, uint8_t word_Address);
