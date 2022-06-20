#include "config.h"
#include <stdint.h>
#include <xc.h>
#include <pic16f877a.h>

uint16_t Timer_Counter = 0; // Global Counter Variable
uint8_t Seconds_Counter = 0; // Global Counter Variable
uint8_t Minute_Counter = 0; // Global Counter Variable
uint8_t Hours_Counter = 12; // Global Counter Variable
uint8_t SSD1_counter = 0; // Global Counter Variable
uint8_t SSD2_counter = 0; // Global Counter Variable
uint8_t SSD3_counter = 0; // Global Counter Variable
uint8_t SSD4_counter = 0; // Global Counter Variable
uint8_t LED_BLINK_FLAG = 0;
uint8_t SSD[12] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00};

#define SSD_state1  1
#define SSD_state2  2
#define SSD_state3  3
#define SSD_state4  4
uint8_t SSD_status = SSD_state1;

#define Clock_init_state    5
#define Clock_run_state     6
#define Clock_config_state  7
uint8_t Clock_status = Clock_init_state;


#define Edit_state    8
#define Set_state     9
uint8_t Set_Edit_status = Set_state;

#define LED_BLINK_STATE_1     10
#define LED_BLINK_STATE_2     11
uint8_t LED_BLINK_STATUS = LED_BLINK_STATE_2;

#define SSD_BLINK_DISPLY_ON      12
#define SSD_BLINK_DISPLY_OFF     13
uint8_t SSD_BLINK_STATUS = SSD_BLINK_DISPLY_ON;


#define SSD1_EN     RA2
#define SSD2_EN     RA3
#define SSD3_EN     RA4
#define SSD4_EN     RA5
#define LED_BLINK   RD7
#define SSD_PORT    PORTD

#define SSD_Display_Dirction_Port       TRISD
#define SSD_Enable_Pins_Dirction_Port   TRISA
#define Button_Set_Edit_Driction        TRISBbits.TRISB3
#define Button_Set_Edit                 RB3 
#define Button_Select_Field_Driction    TRISBbits.TRISB4
#define Button_Select_Field             RB4 
#define Button_Increment_Driction       TRISBbits.TRISB0
#define Button_Decrement_Driction       TRISBbits.TRISB5

#define Select_Field_Minute_state 14
#define Select_Field_Hours_state  15 
uint8_t Select_Field_status = Select_Field_Minute_state;
// Interrupt Service Routine - ISR

void __interrupt() ISR(void) {


    // Check The Flag Bit
    if (TMR1IF) {

        Timer_Counter++;
        /* delay 1s */
        if (Timer_Counter == 20) {
            if (Set_Edit_status == Set_state)
                Seconds_Counter++;
            Timer_Counter = 0;
            //LED_BLINK_FLAG = 1;   //for debugging 
        }
        /* delay 0.5s */
        if (Timer_Counter % 10 == 0) {
            switch (LED_BLINK_STATUS) {
                case (LED_BLINK_STATE_1):
                    LED_BLINK_STATUS = LED_BLINK_STATE_2;
                    break;

                case (LED_BLINK_STATE_2):
                    LED_BLINK_STATUS = LED_BLINK_STATE_1;
                    break;
            }

        }

        TMR1 = 65536 - 50000;
        TMR1IF = 0; // Clear The Flag Bit
    }

    if (INTF) {
        switch (Set_Edit_status) {
            case (Edit_state):
                switch (Select_Field_status) {
                    case (Select_Field_Minute_state):
                        Minute_Counter++;
                        break;

                    case (Select_Field_Hours_state):
                        Hours_Counter++;
                        break;

                        break;
                }

        }

        INTF = 0;
    }


}

void main() {

    // -- [[  IO Configurations ]] --

    SSD_Display_Dirction_Port = 0;
    SSD_Enable_Pins_Dirction_Port = 0;
    Button_Set_Edit_Driction = 1;
    Button_Increment_Driction = 1;
    Button_Decrement_Driction = 1;
    Button_Select_Field_Driction = 1;
    Button_Increment_Driction = 1; // for INT0 
    Button_Decrement_Driction = 1; // for INT1 
    SSD1_EN = 0;
    SSD2_EN = 0;
    SSD3_EN = 0;
    SSD4_EN = 0;
    SSD_PORT = SSD[0];



    INTE = 1;
    INTEDG = 0;

    TMR1 = 65536 - 50000;
    // Choose the local clock source (timer mode)
    TMR1CS = 0;
    // Choose the desired prescaler ratio (1:1)
    T1CKPS0 = 1;
    T1CKPS1 = 0;
    // Switch ON Timer1 Module!
    TMR1ON = 0;
    // -- [[ Interrupts Configurations ]] --
    TMR1IE = 1; // Timer1 Interrupt Enable Bit
    TMR1IF = 0; // Clear The Interrupt Flag Bit
    PEIE = 1; // Peripherals Interrupts Enable Bit
    GIE = 1; // Global Interrupts Enable Bit

    uint8_t SSD_temp = 0;
    while (1) {




        if (Button_Set_Edit == 0) {
            while (Button_Set_Edit == 0);

            switch (Set_Edit_status) {
                case (Set_state):
                    Set_Edit_status = Edit_state;
                    break;

                case (Edit_state):
                    Set_Edit_status = Set_state;
                    break;
            }
        }

        switch (Set_Edit_status) {
            case (Edit_state):
                // stop timer
                //TMR1ON = 0;
                if (Button_Select_Field == 0) {
                    while (Button_Select_Field == 0);

                    switch (Select_Field_status) {
                        case (Select_Field_Minute_state):
                            Select_Field_status = Select_Field_Hours_state;
                            break;

                        case (Select_Field_Hours_state):
                            Select_Field_status = Select_Field_Minute_state;
                            break;

                    }
                }

                // inside the ISR of increment and decrement buttons 
                // increment and decrement if you are in edit state

                break;

            case (Set_state):
                // rest timer count and run it
                TMR1ON = 1;
                break;

        }


        // INT 1
        if (RB5 == 0) {
            while (RB5 == 0);
            switch (Set_Edit_status) {
                case (Edit_state):
                    switch (Select_Field_status) {
                        case (Select_Field_Minute_state):
                            Minute_Counter--;
                            break;

                        case (Select_Field_Hours_state):
                            Hours_Counter--;
                            break;
                    }
                    break;
            }
        }



        if (Seconds_Counter == 60) {
            Minute_Counter++;
            Seconds_Counter = 0;
        } else if (Seconds_Counter > 60) {
            Minute_Counter--;
            Seconds_Counter = 59;
        }

        if (Minute_Counter == 60) {
            Hours_Counter++;
            Minute_Counter = 0;
        } else if (Minute_Counter > 60) {
            Hours_Counter--;
            Minute_Counter = 59;
        }

        if (Hours_Counter == 24) {
            Hours_Counter = 0;
        } else if (Hours_Counter > 24) {
            Hours_Counter = 23;
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
                if (Set_Edit_status == Set_state) {
                    //SSD1_counter = (Minute_Counter % 10);

                } else {
                    if (Select_Field_status == Select_Field_Minute_state) {
                        switch (LED_BLINK_STATUS) {
                            case (LED_BLINK_STATE_1):
                                //SSD1_counter = (Minute_Counter % 10);
                                break;

                            case (LED_BLINK_STATE_2):
                                SSD1_counter = 11;
                                break;
                        }
                    }
                }

                SSD_PORT = SSD[SSD1_counter]; /* initial value */
                SSD_status = SSD_state2;
                //LED_BLINK = 0;
                break;

            case SSD_state2:
                SSD1_EN = 0;
                SSD2_EN = 0;
                SSD3_EN = 1;
                SSD4_EN = 0;
                if (Set_Edit_status == Set_state) {
                    //SSD2_counter = (Minute_Counter / 10);

                } else {
                    if (Select_Field_status == Select_Field_Minute_state) {
                        switch (LED_BLINK_STATUS) {
                            case (LED_BLINK_STATE_1):
                                //SSD2_counter = (Minute_Counter / 10);
                                break;

                            case (LED_BLINK_STATE_2):
                                SSD2_counter = 11;
                                break;
                        }
                    }
                }

                SSD_PORT = SSD[SSD2_counter]; /* initial value */
                

                SSD_status = SSD_state3;
                //LED_BLINK = 0;
                break;

            case SSD_state3:
                SSD1_EN = 0;
                SSD2_EN = 1;
                SSD3_EN = 0;
                SSD4_EN = 0;
                if (Set_Edit_status == Set_state) {
                    //SSD3_counter = (Hours_Counter % 10);
                    switch (LED_BLINK_STATUS) {
                        case (LED_BLINK_STATE_1):
                            LED_BLINK = 1;
                            break;

                        case (LED_BLINK_STATE_2):
                            LED_BLINK = 0;
                            break;
                    }

                } else {
                    if (Select_Field_status == Select_Field_Hours_state) {
                        switch (LED_BLINK_STATUS) {
                            case (LED_BLINK_STATE_1):
                                //SSD3_counter = (Hours_Counter % 10);
                                break;

                            case (LED_BLINK_STATE_2):
                                SSD3_counter = 11;
                                break;
                        }
                    }
                }
                
                SSD_PORT &= ~0x7f;
                SSD_PORT |= SSD[SSD3_counter]; /* initial value */
                SSD_status = SSD_state4;
                break;

            case SSD_state4:
                SSD1_EN = 1;
                SSD2_EN = 0;
                SSD3_EN = 0;
                SSD4_EN = 0;
                if (Set_Edit_status == Set_state) {
                    //SSD4_counter = (Hours_Counter / 10);

                } else {
                    if (Select_Field_status == Select_Field_Hours_state) {
                        switch (LED_BLINK_STATUS) {
                            case (LED_BLINK_STATE_1):
                                //SSD4_counter = (Hours_Counter / 10);
                                break;

                            case (LED_BLINK_STATE_2):
                                SSD4_counter = 11;
                                break;
                        }
                    }
                }

                SSD_PORT = SSD[SSD4_counter]; /* initial value */
                SSD_status = SSD_state1;
                //LED_BLINK = 0;
                break;
        }
    }

}

