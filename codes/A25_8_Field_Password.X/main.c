#define _XTAL_FREQ  8000000UL
#include "config.h"
#include "LCD.h"
#include "KeyPad.h"
#include <stdint.h>
#include <string.h>
#include <xc.h>

/* KeyPad Keys
 *   '1' -----> password key
 *   '2' -----> password key
 */

#define LED_BLINKING_DIRECTION      TRISBbits.TRISB7
#define LED_BLINKING                      RB7


void Timer1_Init(void);


uint8_t msg1[16] = "Enter Password";
uint8_t msg2[16] = " Excellent  you";
uint8_t msg3[16] = "               ";
uint8_t msg4[16] = "Cracked the Code";
uint8_t msg5[16] = "chances exausted";
uint8_t msg6[16] = "Reset the Board";

uint8_t Static_Password[9] = "12121212";
uint8_t Entered_Password[9] = {0};
uint8_t Entered_Password_Index = 0;

uint8_t LED_Blinking_Rate = 20;

uint16_t Timer_Counter = 0; // Global Counter Variable
int8_t Attempt_Number = 5;

void __interrupt() ISR(void) {


    // Check The Flag Bit
    if (TMR1IF) {
        Timer_Counter++;
        if (Timer_Counter == LED_Blinking_Rate) {
            LED_BLINKING = ~LED_BLINKING;
            Timer_Counter = 0;

        }


        TMR1 = 65536 - 50000;
        TMR1IF = 0;
    }



}

void main() {

    LED_BLINKING_DIRECTION = 0;

    LCD_Init();
    KeyPad_Init();
    Timer1_Init();

    LCD_DisStrRC(msg1, 0, 1);

    uint8_t KeyPad_Pressed_Key = 0;
    uint8_t Wrong_Password_Flag = 1;

    while (1) {

        KeyPad_Pressed_Key = KeyPad_GetKey();

        switch (KeyPad_Pressed_Key) {
            case '1':
                LCD_GotoRC(1,3+Entered_Password_Index);
                LCD_DisChar('*');
                Entered_Password[Entered_Password_Index] = '1';
                Entered_Password_Index++;
                break;

            case '2':
                LCD_GotoRC(1,3+Entered_Password_Index);
                LCD_DisChar('*');
                Entered_Password[Entered_Password_Index] = '2';
                Entered_Password_Index++;
                break;
        }

        if (Entered_Password_Index > 7) {
            //compare entered password with the embedded one 
            uint8_t i = 0;
            while (i < 8) {
                if (Entered_Password[i] != Static_Password[i]) {
                    // decrement the attempt with one 
                    // wrong password flag = 1

                    Attempt_Number--;
                    LED_Blinking_Rate -= 3;
                    Entered_Password_Index=0;
                    LCD_DisStrRC(msg3, 1 , 0);
                    Wrong_Password_Flag = 1;
                    break;
                }
                else {
                    if (i == 7) {
                        Wrong_Password_Flag = 0;
                    }
                }
                i++;
            }

        }

        if (Wrong_Password_Flag == 0) {
            // display the embedded massage
            TMR1ON = 0;
            LED_BLINKING=0;
            LCD_SendCMD(CMD_CLEAR_SCREEN);
            LCD_DisStrRC(msg2, 0 , 0);
            LCD_DisStrRC(msg4, 1 , 0);
            while(1);
        }
        
        if(Attempt_Number == 0){
            // infinity loop
            TMR1ON = 0;
            LED_BLINKING=1;
            LCD_SendCMD(CMD_CLEAR_SCREEN);
            LCD_DisStrRC(msg5, 0 , 0);
            LCD_DisStrRC(msg6, 1 , 0);
            while(1);
        }


    }

}

void Timer1_Init(void) {

    TMR1 = 65536 - 50000;
    // Choose the local clock source (timer mode)
    TMR1CS = 0;
    // Choose the desired prescaler ratio (1:1)
    T1CKPS0 = 0;
    T1CKPS1 = 0;

    // Switch ON Timer1 Module!
    TMR1ON = 1;
    // -- [[ Interrupts Configurations ]] --
    TMR1IE = 1; // Timer1 Interrupt Enable Bit
    TMR1IF = 0; // Clear The Interrupt Flag Bit
    PEIE = 1; // Peripherals Interrupts Enable Bit
    GIE = 1; // Global Interrupts Enable Bit
}