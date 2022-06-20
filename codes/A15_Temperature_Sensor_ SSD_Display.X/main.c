#define _XTAL_FREQ  4000000UL
#include "config.h"
#include <stdint.h>
#include <xc.h>

uint16_t Counter = 9999; // Global Counter Variable
uint8_t SSD1_counter = 0; // Global Counter Variable
uint8_t SSD2_counter = 0; // Global Counter Variable
uint8_t SSD3_counter = 0; // Global Counter Variable
uint8_t SSD4_counter = 0; // Global Counter Variable
uint8_t SSD[11] = {0x3F , 0x06 , 0x5B , 0x4F , 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x39};

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


void main() {

    // -- [[  IO Configurations ]] --

    SSD_Display_Dirction_Port = 0x00; 
    SSD_Enable_Pins_Dirction_Port = 0x01;  // first pin is the input adc pin
    SSD1_EN=0;
    SSD2_EN=0;
    SSD3_EN=0;
    SSD4_EN=0;
    SSD_PORT = SSD[0];
    

    ADCON0 = 0x41;
    ADCON1 = 0x8E;
   __delay_us(30);
    
    volatile uint32_t adc_value=0;
    volatile uint32_t LM35_value=0;
            
    while (1) {
        
        // start conversion
        GO_DONE = 1;
        
        // read adc value
        adc_value = ((ADRESH << 8) | ADRESL);
        // wait until the end of conversion
        while(ADCON0bits.GO_DONE)
        {
            
        }
        
        LM35_value = (adc_value*1500)/1023;
        
       
        
        //SSD2_counter = (LM35_value % 10);
        SSD2_counter = ((LM35_value/10) % 10);
        SSD3_counter = ((LM35_value/100) % 10);
        SSD4_counter = ((LM35_value/1000) % 10);
        
        switch (SSD_status) {
            case SSD_state1:
                SSD1_EN = 0;
                SSD2_EN = 0;
                SSD3_EN = 0;
                SSD4_EN = 1;
                SSD_PORT = SSD[10]; /* initial value */
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
