#define _XTAL_FREQ  8000000UL
#include "config.h"
#include "LCD.h"
#include <stdint.h>
#include <string.h>
#include <xc.h>


#define Massage_Length 40
uint8_t Massage[Massage_Length] = "This is a static massage right shifted";

uint8_t Offset = 0;


void main() {

    LCD_Init();


    uint8_t i = 0;
    while (1) {
        
        LCD_GotoRC(0,0);
         
        while (i < Massage_Length) {
            LCD_DisChar(Massage[i+Offset]);
            i++;
            if(i==16){
               LCD_GotoRC(1,0); 
            }
        }
         
        i = 0;
        Offset++;
        
        if(Offset > Massage_Length){
            Offset=0;
        }
        
        __delay_ms(500);
    }

}
