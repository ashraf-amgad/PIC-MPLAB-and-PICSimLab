#include "I2C.h"

void I2C_Master_Init(void)
{
  SSPCON = 0x28;
  SSPCON2 = 0x00;
  SSPSTAT = 0x00;
  SSPADD = ((_XTAL_FREQ/4)/I2C_BaudRate) - 1;
  TRISC3 = 1;
  TRISC4 = 1;
}

void I2C_Wait(void)
{
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}

void I2C_Start(void)
{
  //---[ Initiate I2C Start Condition Sequence ]---
  I2C_Wait();
  SEN = 1;
}

void I2C_Stop(void)
{
  //---[ Initiate I2C Stop Condition Sequence ]---
  I2C_Wait();
  PEN = 1;
}

void I2C_Restart(void)
{
  //---[ Initiate I2C Restart Condition Sequence ]---
  I2C_Wait();
  RSEN = 1;
}


void I2C_ACK(void)
{
  //---[ Send ACK - For Master Receiver Mode ]---
  I2C_Wait();
  ACKDT = 0; // 0 -> ACK, 1 -> NACK
  ACKEN = 1; // Send ACK Signal!
}

void I2C_NACK(void)
{
  //---[ Send NACK - For Master Receiver Mode ]---
  I2C_Wait();
  ACKDT = 1; // 1 -> NACK, 0 -> ACK
  ACKEN = 1; // Send NACK Signal!
}


uint8_t I2C_Write(uint8_t Data)
{
  //---[ Send Byte, Return The ACK/NACK ]---
  I2C_Wait();
  SSPBUF = Data;
  I2C_Wait();
  return ACKSTAT;
}

uint8_t I2C_Read(void)
{
  uint8_t Data;
  I2C_Wait();
  RCEN = 1;
  I2C_Wait();
  Data = SSPBUF;
  I2C_NACK();
  return Data;
}