#define _XTAL_FREQ  8000000UL
#include "config.h"
#include "LCD.h"
#include "KeyPad.h"
#include <stdint.h>
#include <string.h>
#include <xc.h>

/* KeyPad Keys
 *   '1' -----> up/left key
 *   '2' -----> down/right
 *   '3' -----> move to the next digit on the left
 *   '4' -----> move to the next digit on the right
 *   '5' -----> start stop key
 *   '6' -----> Edit key
 */ 

uint8_t msg_display_flag = 0;
uint8_t msg1[16] = "Enter New Number";
uint8_t msg2[5] = "msg:";
uint8_t msg3[16] = " Message Marquee";

#define Preset_Mode         0
#define Non_Preset_Mode     1

uint8_t Preset_Non_Preset_Mode = Preset_Mode;

#define Start_Mode          2
#define Stop_Mode           3

uint8_t Start_Stop_Mode = Stop_Mode;

uint8_t Selected_Digits[10] = "000000000";
int8_t Selected_Digit_Offset = 0;
#define left    0
#define right   1
uint8_t Shift = left;

int main() {

    LCD_Init();
    KeyPad_Init();

    uint8_t KeyPad_Pressed_Key = 0;
    int8_t Display_Offset = 0;
    uint8_t Offset_Counter = 0;
    while (1) {


        KeyPad_Pressed_Key = KeyPad_GetKey();

        switch (KeyPad_Pressed_Key) {
            case '1':
                //  up/left
                if (Start_Stop_Mode == Start_Mode) {
                    Shift = left;
                }
                if (Preset_Non_Preset_Mode == Preset_Mode) {
                    Selected_Digits[Selected_Digit_Offset]++;
                    if (Selected_Digits[Selected_Digit_Offset] > '9') {
                        Selected_Digits[Selected_Digit_Offset] = '0';
                    }
                }

                break;

            case '2':
                // down/right
                if (Start_Stop_Mode == Start_Mode) {
                    Shift = right;
                }
                if (Preset_Non_Preset_Mode == Preset_Mode) {
                    Selected_Digits[Selected_Digit_Offset]--;
                    if (Selected_Digits[Selected_Digit_Offset] < '0') {
                        Selected_Digits[Selected_Digit_Offset] = '9';
                    }
                }
                break;

            case '3':
                // move to the next digit on the left
                if (Preset_Non_Preset_Mode == Preset_Mode) {
                    Selected_Digit_Offset--;
                    if (Selected_Digit_Offset < 0) {
                        Selected_Digit_Offset = 0;
                    }
                }
                break;

            case '4':
                // move to the next digit on the right
                if (Preset_Non_Preset_Mode == Preset_Mode) {
                    Selected_Digit_Offset++;
                    Offset_Counter++;
                    if (Selected_Digit_Offset > 9) {
                        Selected_Digit_Offset = 9;
                    }

                }
                break;


            case '5':
                // start stop key
                switch (Start_Stop_Mode) {
                    case Stop_Mode:
                        LCD_SendCMD(CMD_CLEAR_SCREEN);
                        LCD_DisStrRC(msg3, 0, 0);
                        Selected_Digit_Offset = 0;
                        Display_Offset = 0;
                        Preset_Non_Preset_Mode = Non_Preset_Mode;
                        Start_Stop_Mode = Start_Mode;
                        break;

                    case Start_Mode:

                        Start_Stop_Mode = Stop_Mode;
                        break;
                }
                break;

            case '6':
                // Edit key
                LCD_SendCMD(CMD_CLEAR_SCREEN);
                LCD_DisStrRC(msg1, 0, 0);
                LCD_DisStrRC(msg2, 1, 0);
                Selected_Digits[Offset_Counter + 1] = '0';
                Offset_Counter = 0;
                Start_Stop_Mode = Stop_Mode;
                Preset_Non_Preset_Mode = Preset_Mode;
                break;
        }



        if (Preset_Non_Preset_Mode == Preset_Mode) {
            if (msg_display_flag == 0) {
                LCD_DisStrRC(msg1, 0, 0);
                LCD_DisStrRC(msg2, 1, 0);
                msg_display_flag = 1;
            }
            LCD_SendCMD(CMD_CURRSOR_OFF);
            LCD_GotoRC(1, Selected_Digit_Offset + 4);
            LCD_DisChar(Selected_Digits[Selected_Digit_Offset]);
            LCD_GotoRC(1, Selected_Digit_Offset + 4);
            LCD_SendCMD(CMD_CURRSOR_ON);
        }

        if (Start_Stop_Mode == Start_Mode) {

            LCD_SendCMD(CMD_CURRSOR_OFF);
            Selected_Digits[Offset_Counter + 1] = 0;
            LCD_Clear_Column_2();
            LCD_DisStrRC(Selected_Digits, 1, Display_Offset);

            if (Shift == right) {
                Display_Offset++;
                if (Display_Offset > 16) {
                    Display_Offset = 0;
                }

            } else if (Shift == left) {
                Display_Offset--;
                if (Display_Offset < 0) {
                    Display_Offset = 16;
                }
            }

        }

    }
}