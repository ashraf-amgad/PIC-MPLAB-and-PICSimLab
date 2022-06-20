#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdint.h>
#include "config.h"
#include "LCD.h"
#include "PWM.h"


uint8_t msg1[16] = "LED Dimmer Level";
uint8_t LED_Dimmer_Indecation[10] = {0};

#define Decrement_Key_Direction     TRISBbits.TRISB1
#define Decrement_Key                    RB1
#define Increment_Key_Direction     TRISBbits.TRISB2
#define Increment_Key                    RB2

int16_t Duty_Cycle = 0;

void main(void) {

    Decrement_Key_Direction = 1;
    Increment_Key_Direction = 1;
    LCD_Init();
    PWM1_Init();

    LCD_DisStrRC(msg1, 0, 0);


    uint8_t Decrement_flag = 0;
    uint8_t Increment_flag = 0;
    int8_t offset = 0;

    while (1) {

        if (Decrement_Key == 0) {
            //while (Decrement_Key == 0);
            Duty_Cycle--;
            if (Duty_Cycle < 0) {
                Duty_Cycle = 0;
            }
            PWM1_Set_Duty(Duty_Cycle);


            if (Duty_Cycle % 10 == 0) {
                offset--;
            }
            if (offset < 0) {
                offset = 0;
            }
            Decrement_flag = 1;

        }

        if (Increment_Key == 0) {
            //while (Increment_Key == 0);
            Duty_Cycle++;
            if (Duty_Cycle > 100) {
                Duty_Cycle = 100;
            }
            PWM1_Set_Duty(Duty_Cycle);

            if (Duty_Cycle % 10 == 0) {
                offset++;
            }
            if (offset > 10) {
                offset = 10;
            }
            Increment_flag = 1;
        }

        LCD_GotoRC(1, 3 + offset);
        if (Increment_flag == 1) {
            LCD_DisChar(255);
            Increment_flag = 0;
        }

        if (Decrement_flag == 1) {
            LCD_DisChar(' ');
            Decrement_flag = 0;
        }

        __delay_ms(10);

    }


}