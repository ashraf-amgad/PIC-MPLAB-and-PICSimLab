
#include <stdint.h>
#include <xc.h>
#include "LCD_Cfg.h"


#define CMD_CLEAR_SCREEN			(0x01)
#define CMD_RETURN_HOME				(0x02)
#define CMD_SHIFT_DISPLAY_LEFT		(0x05)
#define CMD_SHIFT_DISPLAY_RIGHT		(0x07)
#define CMD_SET_DICTION_LEFT		(0x04)
//#define CMD_SET_DICTION_RIGHT		(0x06)
#define CMD_CURRSOR_OFF				(0x0C)
#define CMD_CURRSOR_ON				(0x0E)
#define CMD_2LINE_4BITS				(0x28)
#define CMD_2LINE_8BITS				(0x38)
#define	CMD_SET_CURRSOR_POS			(0x80)	



void LCD_Init(void);
void LCD_SendCMD(uint8_t COMMAND);
void LCD_DisChar(uint8_t DATA);
void LCD_DisStr(uint8_t* str);
void LCD_Int2Str(uint16_t Integer ,uint8_t* str);
void LCD_GotoRC(uint8_t row ,uint8_t column);
void LCD_DisStrRC(uint8_t* str ,uint8_t row ,uint8_t column);
