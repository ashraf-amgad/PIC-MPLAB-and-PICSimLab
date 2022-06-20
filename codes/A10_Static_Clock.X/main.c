#include "config.h"
#include <stdint.h>
#include <xc.h>

uint16_t Timer_Counter = 0; // Global Counter Variable
uint8_t Seconds_Counter = 0; // Global Counter Variable
uint8_t Minute_Counter = 0; // Global Counter Variable
uint8_t Hours_Counter = 12; // Global Counter Variable
uint8_t SSD1_counter = 0; // Global Counter Variable
uint8_t SSD2_counter = 0; // Global Counter Variable
uint8_t SSD3_counter = 0; // Global Counter Variable
uint8_t SSD4_counter = 0; // Global Counter Variable
uint8_t LED_BLINK_FLAG = 0;
uint8_t SSD[10] = {0x3F , 0x06 , 0x5B , 0x4F , 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

#define SSD_state1  1
#define SSD_state2  2
#define SSD_state3  3
#define SSD_state4  4
uint8_t SSD_status = SSD_state1;

#define LED_BLINK_STATE_1     0
#define LED_BLINK_STATE_2     1
uint8_t LED_BLINK_STATUS = LED_BLINK_STATE_2;


#define SSD1_EN     RA2
#define SSD2_EN     RA3
#define SSD3_EN     RA4
#define SSD4_EN     RA5
#define LED_BLINK   RD7
#define SSD_PORT    PORTD

#define SSD_Display_Dirction_Port       TRISD
#define SSD_Enable_Pins_Dirction_Port   TRISA

// Interrupt Service Routine - ISR

void __interrupt() ISR(void) {


    // Check The Flag Bit
    if (TMR1IF) {

        Timer_Counter++;
        /* delay 1s */
        if (Timer_Counter == 20) {
            Seconds_Counter++;
            Timer_Counter = 0;
            
        }
        /* delay 0.5s */
        if (Timer_Counter%10 == 0) {
           switch (LED_BLINK_STATUS) {
                case (LED_BLINK_STATE_1):
                    LED_BLINK_STATUS = LED_BLINK_STATE_2;
                    break;

                case (LED_BLINK_STATE_2):
                    LED_BLINK_STATUS = LED_BLINK_STATE_1;
                    break;
            }
        }

        TMR1 = 65536-50000;
        TMR1IF = 0; // Clear The Flag Bit
    }
}

void main() {

    // -- [[  IO Configurations ]] --

    SSD_Display_Dirction_Port = 0x00; /* enable pins first 4-pins of portc */
    SSD_Enable_Pins_Dirction_Port = 0; /* quad 7-seg */
    SSD1_EN=0;
    SSD2_EN=0;
    SSD3_EN=0;
    SSD4_EN=0;
    SSD_PORT = SSD[0];
    // -- [[ Configure Timer1 To Operate In Timer Mode&nbsp; ]] --
    // Clear The Timer1 Register. To start counting from 0
    //TMR1 = 0;
    TMR1 = 65536-50000;
    // Choose the local clock source (timer mode)
    TMR1CS = 0;
    // Choose the desired prescaler ratio (1:1)
    T1CKPS0 = 1;
    T1CKPS1 = 0;
    // Event1 = LED ON
    RB0 = 1;
    // Switch ON Timer1 Module!
    TMR1ON = 1;
    // -- [[ Interrupts Configurations ]] --
    TMR1IE = 1; // Timer1 Interrupt Enable Bit
    TMR1IF = 0; // Clear The Interrupt Flag Bit
    PEIE = 1; // Peripherals Interrupts Enable Bit
    GIE = 1; // Global Interrupts Enable Bit

    while (1) {
        
        if (Seconds_Counter == 60) {
            Minute_Counter++;
            Seconds_Counter = 0;
        }

        if (Minute_Counter == 60) {
            Hours_Counter++;
            Minute_Counter = 0;
        }

        if (Hours_Counter == 24) {
            Hours_Counter = 0;
        }
        
        //SSD1_counter = (Seconds_Counter % 10);
        //SSD2_counter = (Seconds_Counter / 10);
        //SSD3_counter = (Minute_Counter % 10);
        //SSD4_counter = (Minute_Counter / 10);

        SSD1_counter = (Minute_Counter % 10);
        SSD2_counter = (Minute_Counter / 10);
        SSD3_counter = (Hours_Counter % 10);
        SSD4_counter = (Hours_Counter / 10);
        switch (SSD_status) {
            case SSD_state1:
                SSD1_EN = 0;
                SSD2_EN = 0;
                SSD3_EN = 0;
                SSD4_EN = 1;
                SSD_PORT &= ~0x7F;
                SSD_PORT |= SSD[SSD1_counter]; /* initial value */
                SSD_status = SSD_state2;
                //LED_BLINK = 0;
                break;

            case SSD_state2:
                SSD1_EN = 0;
                SSD2_EN = 0;
                SSD3_EN = 1;
                SSD4_EN = 0;
                SSD_PORT = SSD[SSD2_counter]; /* initial value */
                SSD_status = SSD_state3;
                //LED_BLINK = 0;
                break;

            case SSD_state3:
                SSD1_EN = 0;
                SSD2_EN = 1;
                SSD3_EN = 0;
                SSD4_EN = 0;
                SSD_PORT = SSD[SSD3_counter]; /* initial value */
                //LED_BLINK = 1;
                switch (LED_BLINK_STATUS) {
                    case (LED_BLINK_STATE_1):
                        LED_BLINK = 1;
                        break;

                    case (LED_BLINK_STATE_2):
                        LED_BLINK = 0;
                        break;
                }
                SSD_status = SSD_state4;
                break;

            case SSD_state4:
                SSD1_EN = 1;
                SSD2_EN = 0;
                SSD3_EN = 0;
                SSD4_EN = 0;
                SSD_PORT = SSD[SSD4_counter]; /* initial value */
                SSD_status = SSD_state1;
                //LED_BLINK = 0;
                break;
        }
       

    }
}
