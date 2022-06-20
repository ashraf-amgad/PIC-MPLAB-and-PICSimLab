#include <stdint.h>
#include <xc.h>

void EEPROM_Write(uint8_t Address, uint8_t Data);
uint8_t EEPROM_Read(uint8_t Address);
