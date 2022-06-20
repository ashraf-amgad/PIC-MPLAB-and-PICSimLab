#define _XTAL_FREQ  8000000UL
#include "Adc.h"

void Adc_Init(void) {

    ADCON0 = 0x41;
    ADCON1 = 0x8E;

    __delay_us(30);

}

uint16_t Adc_Read(void) {
    
    uint16_t adc_value;
    
    // start conversion
    GO_DONE = 1;

    adc_value = ((ADRESH << 8) | ADRESL);
    
    // wait until the end of conversion
    while (ADCON0bits.GO_DONE);
    
    return adc_value;
    
    
}