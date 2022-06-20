
#define _XTAL_FREQ  8000000UL
#include "Button.h"

void Button_Init(void){
    
    Dio_Init();
    
    Button_1_Direction = 1;
    Button_2_Direction = 1;
    Button_3_Direction = 1;
    Button_4_Direction = 1;
    Button_5_Direction = 1;
    
    
    
}

uint8_t Button_Read(uint8_t Button){
    
    if(Button == 0){
         __delay_ms(10);
         if(Button == 0){
             return 0;
         }
    }
   return 1;
    
    
}