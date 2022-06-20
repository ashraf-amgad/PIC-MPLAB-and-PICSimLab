#include "config.h"
#include "LCD.h"
#include <stdint.h>
#include <string.h>
#include <xc.h>

uint8_t First_Row_Massage[16] = "10 Digit Counter";
uint8_t Second_Row_Massage[16] = "Count:";
uint8_t Counter[11] = "9999999999";

void main() {

    LCD_Init();
    LCD_DisStrRC(First_Row_Massage, 0, 0);
    LCD_DisStrRC(Second_Row_Massage, 1, 0);

    LCD_SendCMD(CMD_SET_DICTION_LEFT);




    uint8_t i = 0;
    while (1) {

        Counter[0]--;
        for (i = 0; i < 10; i++) {
            if (Counter[i] < '0'){
                Counter[i+1]--;
                Counter[i]='9';
            }
        }
        
        LCD_DisStrRC(Counter, 2, 15);

    }
}
