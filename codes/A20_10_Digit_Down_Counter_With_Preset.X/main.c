#include "config.h"
#include "LCD.h"
#include <stdint.h>
#include <string.h>
#include <xc.h>

uint8_t First_Row_Massage[16] = "10 Digit Counter";
uint8_t Second_Row_Massage[16] = "Count:";
uint8_t Counter[10] = "0000000000";

#define Start_State     1
#define Preset_State     2

uint8_t Start_Preset_Status = Preset_State;
uint8_t Field_Offset = 0;

void __interrupt() ISR(void) {

    if (INTF) {
        switch (Start_Preset_Status) {
            case (Preset_State):
                Counter[Field_Offset]++;
                if(Counter[Field_Offset] > '9')
                    Counter[Field_Offset]='0';
                if(Counter[Field_Offset] < '0')
                    Counter[Field_Offset]='9';
                break;
        }
        INTF = 0;
    }


}

void main() {

    LCD_Init();
    LCD_DisStrRC(First_Row_Massage, 0, 0);
    LCD_DisStrRC(Second_Row_Massage, 1, 0);

    LCD_SendCMD(CMD_SET_DICTION_LEFT);

    TRISBbits.TRISB0 = 1; // for INT0 
    TRISBbits.TRISB3 = 1;
    TRISBbits.TRISB4 = 1;


    INTE = 1;
    INTEDG = 0;
    PEIE = 1; // Peripherals Interrupts Enable Bit
    GIE = 1; // Global Interrupts Enable Bit

    uint8_t i = 0;
    while (1) {


        if (RB3 == 0) {
            while (RB3 == 0);

            switch (Start_Preset_Status) {
                case Start_State:
                    LCD_GotoRC(1, 15);
                    LCD_SendCMD(CMD_CURRSOR_ON);
                    Field_Offset = 0;
                    Start_Preset_Status = Preset_State;
                    break;

                case Preset_State:
                    LCD_SendCMD(CMD_CURRSOR_OFF);
                    Start_Preset_Status = Start_State;
                    break;
            }
        }

        if (Start_Preset_Status == Start_State) {

            for (i = 0; i < 10; i++) {
                if(Counter[i] > '9')
                    Counter[i]='0';
                if(Counter[i] < '0'){
                    Counter[i]='9';
                    if(Counter[i+1] == '0'){
                        Counter[i+1]='9';
                    }
                    else{
                        Counter[i + 1]--;
                    }
                }
            }
        }




        switch (Start_Preset_Status) {
            case Start_State:
                Counter[0]--;
                LCD_GotoRC(1, 15);
                LCD_DisChar(Counter[0]);
                LCD_DisChar(Counter[1]);
                LCD_DisChar(Counter[2]);
                LCD_DisChar(Counter[3]);
                LCD_DisChar(Counter[4]);
                LCD_DisChar(Counter[5]);
                LCD_DisChar(Counter[6]);
                LCD_DisChar(Counter[7]);
                LCD_DisChar(Counter[8]);
                LCD_DisChar(Counter[9]);
                break;

            case Preset_State:
                if (RB4 == 0) {
                    while (RB4 == 0);
                    Field_Offset++;
                    if (Field_Offset > 10)
                        Field_Offset = 0;
                }

                LCD_SendCMD(CMD_CURRSOR_ON);
                LCD_GotoRC(1, 15 - Field_Offset);
                LCD_SendCMD(CMD_CURRSOR_OFF);
                LCD_DisChar(Counter[Field_Offset]);
                LCD_GotoRC(1, 15 - Field_Offset);

                break;
        }


        if (strcmp(Counter, "0000000000") == 0)
            Start_Preset_Status = Preset_State;

    }
}
