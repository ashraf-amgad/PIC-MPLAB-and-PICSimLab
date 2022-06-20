#include <stdint.h>
#include <xc.h>
#include "I2C.h"


void External_EEPROM_Init(void);


void External_EEPROM_Write(uint8_t Device_Address, uint8_t word_Address , uint8_t Data);

uint8_t External_EEPROM_Read(uint8_t Device_Address, uint8_t word_Address);
