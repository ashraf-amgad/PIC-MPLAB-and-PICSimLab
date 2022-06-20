#include "E2PROM_24C04.h"


void E2PROM_24C04_Init(void){
    I2C_Master_Init();
}



void E2PROM_24C04_Write(uint8_t Device_Address, uint8_t word_Address , uint8_t Data)
{
  I2C_Start();
  I2C_Write(Device_Address); // Slave I2C Device Address + Write
  I2C_Write(word_Address); // word address 
  I2C_Write(Data); // Data To Be Sent
  I2C_Stop();
}


uint8_t E2PROM_24C04_Read(uint8_t Device_Address, uint8_t word_Address)
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