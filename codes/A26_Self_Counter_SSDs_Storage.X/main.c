#define _XTAL_FREQ  8000000UL
#include "config.h"
#include "E2PROM.h"
#include <stdint.h>
#include <xc.h>

uint8_t Seconds_Counter = 0; // Global Counter Variable
uint8_t Minute_Counter = 0; // Global Counter Variable
uint8_t SSD1_counter = 0; // Global Counter Variable
uint8_t SSD2_counter = 0; // Global Counter Variable
uint8_t SSD3_counter = 0; // Global Counter Variable
uint8_t SSD4_counter = 0; // Global Counter Variable
uint8_t SSD[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

#define SSD_state1  1
#define SSD_state2  2
#define SSD_state3  3
#define SSD_state4  4
uint8_t SSD_status = SSD_state1;


#define SSD1_EN     RA2
#define SSD2_EN     RA3
#define SSD3_EN     RA4
#define SSD4_EN     RA5
#define LED_BLINK   RD7
#define SSD_PORT    PORTD

#define SSD_Display_Dirction_Port       TRISD
#define SSD_Enable_Pins_Dirction_Port   TRISA

#define Store_Key_Direction     TRISBbits.TRISB4
#define Store_Key                     RB4

void main() {


    SSD_Display_Dirction_Port = 0x00; /* enable pins first 4-pins of portc */
    SSD_Enable_Pins_Dirction_Port = 0; /* quad 7-seg */
    SSD1_EN = 0;
    SSD2_EN = 0;
    SSD3_EN = 0;
    SSD4_EN = 0;
    SSD_PORT = SSD[0];

    Store_Key_Direction = 1;

    uint8_t EEPROM_Value = EEPROM_Read(0x00);


    if (EEPROM_Value == 1) {
        EEPROM_Value = EEPROM_Read(0x01);
        Seconds_Counter = EEPROM_Value; // Global Counter Variable

        EEPROM_Value = EEPROM_Read(0x02);
        Minute_Counter = EEPROM_Value; // Global Counter Variable
    }
    else{
        Seconds_Counter = 0; // Global Counter Variable
        Minute_Counter = 0; // Global Counter Variable
    }


    uint16_t Time_Counter = 0;
    while (1) {

        if (Time_Counter == 400) {
            Seconds_Counter++;
            Time_Counter = 0;
        }

        if (Seconds_Counter == 99) {
            Minute_Counter++;
            Seconds_Counter = 0;
        }

        if (Minute_Counter == 99) {
            Minute_Counter = 0;
        }

        if (Store_Key == 0) {
            while (Store_Key == 0);
            EEPROM_Write(0, 0x01);
            EEPROM_Write(1, Seconds_Counter);
            EEPROM_Write(2, Minute_Counter);
        }
        
        

        SSD1_counter = (Seconds_Counter % 10);
        SSD2_counter = (Seconds_Counter / 10);
        SSD3_counter = (Minute_Counter % 10);
        SSD4_counter = (Minute_Counter / 10);


        switch (SSD_status) {
            case SSD_state1:
                SSD1_EN = 0;
                SSD2_EN = 0;
                SSD3_EN = 0;
                SSD4_EN = 1;
                SSD_PORT = SSD[SSD1_counter];
                SSD_status = SSD_state2;
                break;

            case SSD_state2:
                SSD1_EN = 0;
                SSD2_EN = 0;
                SSD3_EN = 1;
                SSD4_EN = 0;
                SSD_PORT = SSD[SSD2_counter];
                SSD_status = SSD_state3;
                //LED_BLINK = 0;
                break;

            case SSD_state3:
                SSD1_EN = 0;
                SSD2_EN = 1;
                SSD3_EN = 0;
                SSD4_EN = 0;
                SSD_PORT = SSD[SSD3_counter];
                SSD_status = SSD_state4;
                break;

            case SSD_state4:
                SSD1_EN = 1;
                SSD2_EN = 0;
                SSD3_EN = 0;
                SSD4_EN = 0;
                SSD_PORT = SSD[SSD4_counter];
                SSD_status = SSD_state1;
                break;
        }


        __delay_ms(1);
        Time_Counter++;


    }
}
