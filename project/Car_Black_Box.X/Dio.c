#include "Dio.h"


void Dio_Init(void){
    
    TRISA = 0b00011111;
    TRISB = 0b11111111;
    TRISC = 0b11111111;
    TRISD = 0b11111110;
    TRISE = 0b00000111;
    
    PORTA = 0b00000000;
    PORTB = 0b00000000;
    PORTC = 0b00000000;
    PORTD = 0b00000000;
    PORTE = 0b00000000;
      
}

