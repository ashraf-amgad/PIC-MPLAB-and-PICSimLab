#define _XTAL_FREQ  8000000UL
#include "config.h"
#include "LCD.h"
#include <stdint.h>
#include <string.h>
#include <xc.h>



uint8_t First_Row_Massage[16] = "LM35 Temp Sensor";
uint8_t Second_Row_Massage[16] = "Temp : ";



void main() {

    LCD_Init();

    TRISAbits.TRISA0 = 1;
    ADCON0 = 0x41;
    ADCON1 = 0x8E;
   __delay_us(30);
    
    volatile uint32_t adc_value=0;
    volatile uint32_t LM35_value=0;
    uint8_t LM35_Str[5] = {0}; 
    LCD_DisStrRC(First_Row_Massage,0,0);
    LCD_DisStrRC(Second_Row_Massage,1,0);
    
    while (1) {
        
        // start conversion
        GO_DONE = 1;
        // wait until the end of conversion
        while(ADCON0bits.GO_DONE)
        {
            
        }
        // read adc value
        adc_value = ((ADRESH << 8) | ADRESL);
        
        LM35_value = (adc_value*1500)/1023;
      
        LCD_Int2Str(LM35_value, LM35_Str);
        LCD_DisStrRC(LM35_Str, 1, 10);
        LCD_DisChar(' ');
        LCD_DisChar('C');
        LCD_DisChar(' ');
        
        
    }

}
