#define _XTAL_FREQ  8000000UL
#include "LCD.h"

		/***************************************/
		
void LCD_Init(void)
{
    
    LCD_PORT_DIR = 0;
    LCD_EN_Direction = 0;
    LCD_RS_Direction = 0;
	
    
	LCD_SendCMD(CMD_RETURN_HOME);
	
	#if (LCD_MODE == 4)
		LCD_SendCMD(CMD_2LINE_4BITS);
	#else	
		LCD_voidSendCMD(CMD_2LINE_8BITS);
	#endif
	
	LCD_SendCMD(CMD_CURRSOR_OFF);
	LCD_SendCMD(CMD_CLEAR_SCREEN);
		
}



		/***************************************/
		
void LCD_SendCMD(uint8_t COMMAND)
{

    LCD_RS = 0;
	__delay_ms(1);
	
	
	#if (LCD_MODE == 4)
        LCD_EN = 1;
        __delay_ms(1);
        
        LCD_PORT_VAL = (COMMAND & 0xF0);
		__delay_ms(1);
		
		LCD_EN = 0;
        __delay_ms(1);
        
        
        LCD_EN = 1;
        __delay_ms(1);
		
        LCD_PORT_VAL = ((COMMAND << 4) & 0xF0);
        __delay_ms(1);
					
		LCD_EN = 0;
        __delay_ms(1);
				
	#else
        LCD_EN = 1;
        __delay_ms(1);
        
        LCD_PORT_VAL = COMMAND;
        __delay_ms(1);
		
		
		LCD_EN = 0;
		__delay_ms(1);
		
	#endif
}

		/***************************************/

void LCD_DisChar(uint8_t DATA)
{
    LCD_RS = 1;
	__delay_ms(1);
	
	
	#if (LCD_MODE == 4)
        LCD_EN = 1;
        __delay_ms(1);
        
        LCD_PORT_VAL = (DATA & 0xF0);
		__delay_ms(1);
		
        LCD_EN = 0;
        __delay_ms(1);
		
        LCD_EN = 1;
        __delay_ms(1);
        
		LCD_PORT_VAL = ((DATA << 4) & 0xF0);
		__delay_ms(1);
		
		LCD_EN = 0;
        __delay_ms(1);
				
	#else

        LCD_EN = 1;
        __delay_ms(1);
        
        LCD_PORT_VAL = DATA;
		__delay_ms(1);
		
		LCD_EN = 0;
        __delay_ms(1);
		
	#endif
}


		/***************************************/
		
void LCD_DisStr(uint8_t* str)
{
	uint8_t i=0;
	
	while(str[i] != 0)
	{
		LCD_DisChar(str[i]);
		i++;
	}
	
}
/*		
void LCD_Int2Str(uint16_t Integer ,uint8_t* str)
{
	uint8_t i=0;
	uint8_t j=0;
		
	if (Integer != 0)
	{	
		while(Integer != 0)
		{
			str[i]  = (Integer % 10) + 0x30;
			Integer = Integer / 10; 
			i++; 		
		}
	
		str[i] = '\0';	
	
 		j=i-1;
 		i=0;
 		uint8_t temp=0;
 	
 		while(i < j )
 		{
 			temp   = str[i];
 			str[i] = str[j];
 			str[j] = temp;
 		
 			j--;
 			i++;		
 		}
	}
		
	else
	{
		str[0] = 0x30;
		str[1] = '\0';
		
	}		
			
}		*/
			
		/***************************************/
		
void LCD_GotoRC(uint8_t row ,uint8_t column)
{
		if(row == 0)
			LCD_SendCMD( CMD_SET_CURRSOR_POS | (row + column) );
		
		else
			LCD_SendCMD( CMD_SET_CURRSOR_POS | (column + 0x40) );
}

		/***************************************/
		
void LCD_DisStrRC(uint8_t* str ,uint8_t row ,uint8_t column)
{
	LCD_GotoRC(row ,column);
	LCD_DisStr(str);
}


