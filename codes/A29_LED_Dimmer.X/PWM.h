#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdint.h>
#include "config.h"


void PWM1_Init(void);

void PWM1_Set_Duty(uint16_t DC);
