
#include "PWM.h"

void PWM1_Init(void)
{
    //--[ Configure The CCP Module For PWM Mode ]--
  CCP1M3 = 1;
  CCP1M2 = 1;
  TRISCbits.TRISC2 = 0; // The CCP1 Output Pin (PWM)
  // Set The PWM Frequency
  PR2 = 124;
  // Set The PS For Timer2 (1:4 Ratio)
  T2CKPS0 = 1;
  T2CKPS1 = 0;
  // Start CCP1 PWM !
  TMR2ON = 1;
  
}


void PWM1_Set_Duty(uint16_t DC)
{
  // Check The DC Value To Make Sure it's Within 10-Bit Range
  uint16_t DutyCycle = (DC*5); 
  if(DutyCycle < 1023)
  {
    uint8_t temp = (DutyCycle & 1);
    CCP1Y = temp;
    temp = (DutyCycle & 2);
    CCP1X = temp;
    CCPR1L = (DutyCycle >> 2);
  }
}
