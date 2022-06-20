#define _XTAL_FREQ  4000000UL
#include "config.h"
#include <stdint.h>
#include <xc.h>

uint16_t Counter = 9999; // Global Counter Variable
uint8_t SSD1_counter = 0; // Global Counter Variable
uint8_t SSD2_counter = 0; // Global Counter Variable
uint8_t SSD3_counter = 0; // Global Counter Variable
uint8_t SSD4_counter = 0; // Global Counter Variable
uint8_t SSD[10] = {0x3F , 0x06 , 0x5B , 0x4F , 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

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

uint16_t Display_Rate = 50000;
// Interrupt Service Routine - ISR

void __interrupt() ISR(void) {


    // Check The Flag Bit
    if (TMR1IF) {
        Counter--;
        TMR1 = 65535-Display_Rate;
        TMR1IF = 0; // Clear The Flag Bit
    }
}

void main() {

    // -- [[  IO Configurations ]] --

    SSD_Display_Dirction_Port = 0x00; 
    SSD_Enable_Pins_Dirction_Port = 0x01; 
    SSD1_EN=0;
    SSD2_EN=0;
    SSD3_EN=0;
    SSD4_EN=0;
    SSD_PORT = SSD[0];
    // -- [[ Configure Timer1 To Operate In Timer Mode&nbsp; ]] --
    // Clear The Timer1 Register. To start counting from 0
    //TMR1 = 0;
    TMR1 = 65535-Display_Rate;
    // Choose the local clock source (timer mode)
    TMR1CS = 0;
    // Choose the desired prescaler ratio (1:1)
    T1CKPS0 = 1;
    T1CKPS1 = 1;
    // Event1 = LED ON
    RB0 = 1;
    // Switch ON Timer1 Module!
    TMR1ON = 1;
    // -- [[ Interrupts Configurations ]] --
    TMR1IE = 1; // Timer1 Interrupt Enable Bit
    TMR1IF = 0; // Clear The Interrupt Flag Bit
    PEIE = 1; // Peripherals Interrupts Enable Bit
    GIE = 1; // Global Interrupts Enable Bit

    
    
    ADCON0 = 0x41;
    ADCON1 = 0x8E;
   __delay_us(30);
    
    volatile uint16_t adc_value=0;
            
    while (1) {
        
        // start conversion
        GO_DONE = 1;
        // read adc value
        adc_value = ((ADRESH << 8) | ADRESL);
        // wait until the end of conversion
        while(ADCON0bits.GO_DONE)
        {
            
        }
        PORTB = (adc_value>>2);
        
        // change the rate according to the conversion value
         if (adc_value <= 100)
               Display_Rate = 5000;
          else if ((adc_value > 100) && (adc_value <= 200))
               Display_Rate = 10000;
          else if ((adc_value > 200) && (adc_value <= 300))
               Display_Rate = 15000;
          else if ((adc_value > 300) && (adc_value <= 400))
               Display_Rate = 20000;
          else if ((adc_value > 400) && (adc_value <= 500))
               Display_Rate = 25000;
          else if ((adc_value > 500) && (adc_value <= 600))
               Display_Rate = 30000;
          else if ((adc_value > 600) && (adc_value <= 700))
               Display_Rate = 35000;
          else if ((adc_value > 700) && (adc_value <= 800))
               Display_Rate = 40000;
          else if ((adc_value > 800) && (adc_value <= 900))
               Display_Rate = 45000;
          else
               Display_Rate = 50000;
         

        SSD1_counter = (Counter % 10);
        SSD2_counter = ((Counter/10) % 10);
        SSD3_counter = ((Counter/100) % 10);
        SSD4_counter = ((Counter/1000) % 10);
        
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
                SSD_PORT &= ~0x7F;
                SSD_PORT |= SSD[SSD2_counter]; /* initial value */
                SSD_status = SSD_state3;
                //LED_BLINK = 0;
                break;

            case SSD_state3:
                SSD1_EN = 0;
                SSD2_EN = 1;
                SSD3_EN = 0;
                SSD4_EN = 0;
                SSD_PORT &= ~0x7F;
                SSD_PORT |= SSD[SSD3_counter]; /* initial value */
                //LED_BLINK = 1;
                
                SSD_status = SSD_state4;
                break;

            case SSD_state4:
                SSD1_EN = 1;
                SSD2_EN = 0;
                SSD3_EN = 0;
                SSD4_EN = 0;
                SSD_PORT &= ~0x7F;
                SSD_PORT |= SSD[SSD4_counter]; /* initial value */
                SSD_status = SSD_state1;
                //LED_BLINK = 0;
                break;
        }
       

    }
}
