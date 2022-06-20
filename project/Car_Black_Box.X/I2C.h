#include "config.h"
#include <stdint.h>
#include <xc.h>


#define _XTAL_FREQ 8000000 // Fosc
#define I2C_BaudRate 100000 // I2C Baud Rate = 100 Kbps

void I2C_Master_Init(void);

void I2C_Wait(void);

void I2C_Start(void);

void I2C_Stop(void);

void I2C_Restart(void);

void I2C_ACK(void);

void I2C_NACK(void);

uint8_t I2C_Write(uint8_t Data);

uint8_t I2C_Read(void);

