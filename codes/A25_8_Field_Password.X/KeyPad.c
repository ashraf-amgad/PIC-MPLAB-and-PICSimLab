
#define _XTAL_FREQ  8000000UL
#include "LCD.h"
#include "KeyPad.h"

void KeyPad_Init(void) {

    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;

    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;

}

uint8_t KeyPad_GetKey(void) {
    RD0 = 0;
    RD1 = 1;
    RD2 = 1;
    RD3 = 1;


    if ((RB0 == 0) && ((PORTD & 0x01) == 0)) {
        while ((RB0 == 0) && ((PORTD & 0x01) == 0));
        return('1');
    } else if ((RB1 == 0) && ((PORTD & 0x01) == 0)) {
        while ((RB1 == 0) && ((PORTD & 0x01) == 0));
        return('2');
    } else if ((RB2 == 0) && ((PORTD & 0x01) == 0)) {
        while ((RB2 == 0) && ((PORTD & 0x01) == 0));
        return('3');
    }
    
    __delay_ms(20);
    RD0 = 1;
    RD1 = 0;
    RD2 = 1;
    RD3 = 1;


    if ((RB0 == 0) && ((PORTD & 0x02) == 0)) {
        while ((RB0 == 0) && ((PORTD & 0x02) == 0));
        return('*');
    } else if ((RB1 == 0) && ((PORTD & 0x02) == 0)) {
        while ((RB1 == 0) && ((PORTD & 0x02) == 0));
        return('0');
    } else if ((RB2 == 0) && ((PORTD & 0x02) == 0)) {
        while ((RB2 == 0) && ((PORTD & 0x02) == 0));
        return('#');
    }
    __delay_ms(20);
    RD0 = 1;
    RD1 = 1;
    RD2 = 0;
    RD3 = 1;


    if ((RB0 == 0) && ((PORTD & 0x04) == 0)) {
        while ((RB0 == 0) && ((PORTD & 0x04) == 0));
        return('7');
    } else if ((RB1 == 0) && ((PORTD & 0x04) == 0)) {
        while ((RB1 == 0) && ((PORTD & 0x04) == 0));
        return('8');
    } else if ((RB2 == 0) && ((PORTD & 0x04) == 0)) {
        while ((RB1 == 0) && ((PORTD & 0x04) == 0));
        return('9');
    }
    __delay_ms(20);

    RD0 = 1;
    RD1 = 1;
    RD2 = 1;
    RD3 = 0;


    if ((RB0 == 0) && ((PORTD & 0x08) == 0)) {
        while ((RB0 == 0) && ((PORTD & 0x08) == 0));
        return('4');
    } else if ((RB1 == 0) && ((PORTD & 0x08) == 0)) {
        while ((RB1 == 0) && ((PORTD & 0x08) == 0));
        return('5');
    } else if ((RB2 == 0) && ((PORTD & 0x08) == 0)) {
        while ((RB2 == 0) && ((PORTD & 0x08) == 0));
        return('6');
    }
    __delay_ms(20);
    return(0);
}
