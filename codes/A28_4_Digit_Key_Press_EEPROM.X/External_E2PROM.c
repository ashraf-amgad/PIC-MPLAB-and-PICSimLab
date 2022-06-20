#include "External_E2PROM.h"


void External_EEPROM_Init(void){
    I2C_Master_Init();
}



void External_EEPROM_Write(uint8_t Device_Address, uint8_t word_Address , uint8_t Data)
{
  I2C_Start();
  I2C_Write(Device_Address); // Slave I2C Device Address + Write
  I2C_Write(word_Address); // word address 
  I2C_Write(Data); // Data To Be Sent
  I2C_Stop();
}


uint8_t External_EEPROM_Read(uint8_t Device_Address, uint8_t word_Address)
{
    
  uint8_t temp;      
  I2C_Start();
  temp = Device_Address & ~(0x01);
  I2C_Write(temp); // Slave I2C Device Address + Write
  I2C_Write(word_Address); // word address 
  I2C_Start(); // I2C Start Sequence
  temp = Device_Address | (0x01);
  I2C_Write(temp); // Slave I2C Device Address + read
  temp = I2C_Read();
  I2C_Stop();
  return (temp);
  
}