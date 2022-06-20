#include "Adc.h"
#include "Button.h"
#include "E2PROM_24C04.h"
#include "LCD.h"
#include "RTC_DS1307.h"
#include "config.h"
#include <stdint.h>
#include <xc.h>

#define _XTAL_FREQ 8000000 // Fosc


uint8_t LCD_Timemsg[5] = "Time";
uint8_t LCD_Eventmsg[2] = "E";
uint8_t LCD_Speedmsg[3] = "SP";
uint8_t LCD_Time[9] = "hh:mm:ss";
uint8_t Passwordmsg[15] = "ENTER PASSWORD";
uint8_t Password_Trails_msg[17] = "3 Attempt remain";
uint8_t Blocked_Sreen_Line1_msg[16] = "You are Blocked";
uint8_t Blocked_Sreen_Line2_msg[15] = "Wait... 15 Min";
uint8_t Main_Menu[5][13] = {"View Log", "Clear Log", "Download Log", "Set Time", "Change pwd"};
uint8_t Clear_Events_msg1[16] = "Logs Cleared...";
uint8_t Clear_Events_msg2[13] = "Successfully";
uint8_t LCD_Logsmsg[5] = "Logs";
uint8_t LCD_NoLogsmsg[3] = "No";
uint8_t passwordChangedSuccessfullymsg[8] = "Changed";


uint8_t Embedded_Password[5] = "0000";
uint8_t Input_Password[5] = {0};
uint8_t New_Password_Offset = 0;

uint8_t Waiting_Minutes = 15;
uint8_t Set_Time_Blinking_flag = 1;

#define Event_ON         1
#define Event_G1         2
#define Event_G2         3
#define Event_G3         4
#define Event_G4         5
#define Event_GR         6
#define Event_GN         7
#define Event_C          8

uint8_t Current_Event = Event_ON;
uint8_t Events_Strings[10][3] = {" ", "ON", "G1", "G2", "G3", "G4", "GR", "GN", "C "};

uint8_t Gear_Counter = 0;
uint8_t Password_Waiting_Time = 0;
int8_t Main_Menu_Offset = 0;

#define Second_Selection_Part   1
#define Minutes_Selection_Part  2
#define Hours_Selection_Part    3

uint8_t Selection_Part = Second_Selection_Part;
uint16_t Selection_Part_flag = 0;

#define Default_Screen          8
#define Login_Screen            9
#define Main_Menu_Screen        10
#define View_Log_Screen         11
#define Clear_Log_Screen        12
#define Download_Log_Screen     13
#define Set_Time_Screen         14
#define Change_Pwd_Screen_1     15
#define Change_Pwd_Screen_2     16
#define Wait_15_Minutes_Screen  30

uint8_t Screen_Status = Default_Screen;

void LCD_Display_Current_Event(uint8_t Current_Event) {

    LCD_DisStrRC(Events_Strings[Current_Event], 1, 11);
}

uint8_t Check_Password(void) {
    uint8_t i = 0;
    uint8_t temp = 0;

    for (i = 0; i < 4; i++) {
        temp = E2PROM_24C04_Read(0xA0, i + 5);
        if (Input_Password[i] == temp) {
            continue;
        } else {
            break;
        }
    }

    if (i > 3) {
        return 0;
    } else {
        return 1;
    }

}

uint16_t Timer_Counter = 0;

void __interrupt() ISR(void) {


    // Check The Flag Bit
    if (TMR1IF) {

        Timer_Counter++;
        Password_Waiting_Time++;
        /* delay 1m */
        if (Timer_Counter == 275) {
            Waiting_Minutes--;
            Timer_Counter = 0;
        }

        if (Timer_Counter % 2 == 0) {
            Set_Time_Blinking_flag = ~Set_Time_Blinking_flag;
        }

        TMR1 = 65536 - 50000;
        TMR1IF = 0; // Clear The Flag Bit
    }
}

void Save_Event_to_24c04(RTC_Date_Typedef RTC_Date, uint8_t Event, uint8_t Speed[2], uint8_t Memory_Offset) {

    E2PROM_24C04_Write(0xA0, 0x27 + Memory_Offset, Speed[0]);
    E2PROM_24C04_Write(0xA0, 0x28 + Memory_Offset, Speed[1]);

    E2PROM_24C04_Write(0xA0, 0x26 + Memory_Offset, Event);

    E2PROM_24C04_Write(0xA0, 0x24 + Memory_Offset, RTC_Date.Seconds[0]);
    E2PROM_24C04_Write(0xA0, 0x25 + Memory_Offset, RTC_Date.Seconds[1]);

    E2PROM_24C04_Write(0xA0, 0x22 + Memory_Offset, RTC_Date.Minutes[0]);
    E2PROM_24C04_Write(0xA0, 0x23 + Memory_Offset, RTC_Date.Minutes[1]);

    E2PROM_24C04_Write(0xA0, 0x20 + Memory_Offset, RTC_Date.Hours[0]);
    E2PROM_24C04_Write(0xA0, 0x21 + Memory_Offset, RTC_Date.Hours[1]);

}

void Clear_Events(void) {
    uint8_t i = 0;
    for (i = 0; i < 160; i++) {
        E2PROM_24C04_Write(0xA0, 0x20 + i, 0);
    }

}

void UART_TX_Init(void) {
    BRGH = 1; // Set For High-Speed Baud Rate
    SPBRG = 51; // Set The Baud Rate To Be 9600 bps 8MHz
    //--[ Enable The Ascynchronous Serial Port ]--
    SYNC = 0;
    SPEN = 1;
    //--[ Set The RX-TX Pins to be in UART mode (not io) ]--
    TRISC6 = 1; // As stated in the datasheet
    TRISC7 = 1; // As stated in the datasheet
    TXEN = 1; // Enable UART Transmission
}

void UART_Write(uint8_t data) {
    while (!TRMT);
    TXREG = data;
}

void UART_WriteStr(uint8_t* String) {
    uint8_t i = 0;
    while (String[i] != 0) {
        UART_Write(String[i]);
        i++;
    }
}

void UART_Send_Current_Event(uint8_t Current_Event) {
    UART_WriteStr(Events_Strings[Current_Event]);
}

void main(void) {

    uint32_t Adc_Return_Value;
    uint32_t Car_Speed;
    uint8_t Adc_Str[5];
    RTC_Date_Typedef RTC_Date;
    uint8_t Pass_Offset = 0;
    uint8_t Password_Trails = 3;
    uint16_t Button_4_counter = 0;
    uint16_t Button_5_counter = 0;
    uint8_t Memory_Offset = 0;
    int8_t View_Log_Counter = 0;
    uint8_t Download_Log_Counter = 0;

    Adc_Init();
    Dio_Init();
    Button_Init();
    E2PROM_24C04_Init();
    LCD_Init();
    RTC_DS1307_Init();
    UART_TX_Init();



    TMR1 = 65536 - 50000;
    // Choose the local clock source (timer mode)
    TMR1CS = 0;
    // Choose the desired PreScaler ratio (1:1)
    T1CKPS0 = 1;
    T1CKPS1 = 1;
    // Switch ON Timer1 Module!
    TMR1ON = 0;
    // -- [[ Interrupts Configurations ]] --
    TMR1IE = 1; // Timer1 Interrupt Enable Bit
    TMR1IF = 0; // Clear The Interrupt Flag Bit
    PEIE = 1; // Peripherals Interrupts Enable Bit
    GIE = 1; // Global Interrupts Enable Bit

    if (E2PROM_24C04_Read(0xA0, 0x00) != 0xAA) {
        E2PROM_24C04_Write(0xA0, 0x05, Embedded_Password[0]);
        E2PROM_24C04_Write(0xA0, 0x06, Embedded_Password[1]);
        E2PROM_24C04_Write(0xA0, 0x07, Embedded_Password[2]);
        E2PROM_24C04_Write(0xA0, 0x08, Embedded_Password[3]);
        E2PROM_24C04_Write(0xA0, 0x00, 0xAA);
    }



    uint8_t temp_;
    temp_ = RTC_DS1307_Read(Word_address_Hours);
    temp_ &= ~0b01000000;
    RTC_DS1307_Write(Word_address_Hours, temp_);

    if (E2PROM_24C04_Read(0xA0, 0x02) != 0xAA) {
        E2PROM_24C04_Write(0xA0, 0x01, 0x00);
        E2PROM_24C04_Write(0xA0, 0x02, 0xAA);
    }

    Memory_Offset = E2PROM_24C04_Read(0xA0, 0x01);

    while (1) {


        if (Button_Read(Button_1) == 0) {
            while (Button_Read(Button_1) == 0);
            Current_Event = 8;
            // save event to external memory
            Adc_Return_Value = Adc_Read();
            Car_Speed = (Adc_Return_Value * 99) / 1023;
            LCD_2DigitIntToStr(Car_Speed, Adc_Str);
            RTC_DS1307_Get_Date(&RTC_Date);
            Save_Event_to_24c04(RTC_Date, Current_Event, Adc_Str, Memory_Offset);
            Memory_Offset += 16;
            if (Memory_Offset / 16 > 10) {
                Memory_Offset = 0;
            }
            // save memory offset
            E2PROM_24C04_Write(0xA0, 0x01, Memory_Offset);
        }

        if (Button_Read(Button_2) == 0) {
            while (Button_Read(Button_2) == 0);
            Current_Event++;
            if (Current_Event > 6) {
                Current_Event = 2;
            }
            // save event to external memory
            Adc_Return_Value = Adc_Read();
            Car_Speed = (Adc_Return_Value * 99) / 1023;
            LCD_2DigitIntToStr(Car_Speed, Adc_Str);
            RTC_DS1307_Get_Date(&RTC_Date);
            Save_Event_to_24c04(RTC_Date, Current_Event, Adc_Str, Memory_Offset);
            Memory_Offset += 16;
            if (Memory_Offset / 16 > 10) {
                Memory_Offset = 0;
            }
            // save memory offset
            E2PROM_24C04_Write(0xA0, 0x01, Memory_Offset);
        }

        if (Button_Read(Button_3) == 0) {
            while (Button_Read(Button_3) == 0);
            Current_Event--;
            if (Current_Event < 1) {
                Current_Event = 7;
            }
            // save event to external memory
            Adc_Return_Value = Adc_Read();
            Car_Speed = (Adc_Return_Value * 99) / 1023;
            LCD_2DigitIntToStr(Car_Speed, Adc_Str);
            RTC_DS1307_Get_Date(&RTC_Date);
            Save_Event_to_24c04(RTC_Date, Current_Event, Adc_Str, Memory_Offset);
            Memory_Offset += 16;
            if (Memory_Offset / 16 > 10) {
                Memory_Offset = 0;
            }
            // save memory offset
            E2PROM_24C04_Write(0xA0, 0x01, Memory_Offset);
        }


        if (Password_Trails == 0) {
            Screen_Status = Wait_15_Minutes_Screen;
        }

        if (Button_Read(Button_4) == 0) {

            if (Screen_Status == Default_Screen) {
                while (Button_Read(Button_4) == 0);
                LCD_SendCMD(CMD_CLEAR_SCREEN);
                LCD_DisStrRC(Passwordmsg, 0, 1);
                Screen_Status = Login_Screen;
            } else if (Screen_Status == Login_Screen) {
                while (Button_Read(Button_4) == 0);
                // save input password
                Input_Password[Pass_Offset] = '1';
                Pass_Offset++;
                LCD_DisChar('*');
                Password_Waiting_Time = 0;
            } else if (Screen_Status == Main_Menu_Screen) {
                while (Button_Read(Button_4) == 0) {
                    Button_4_counter++;
                }
                if (Button_4_counter < 500) {
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    Main_Menu_Offset--;
                    if (Main_Menu_Offset < 0) {
                        Main_Menu_Offset = 0;
                    }
                } else {
                    // select the element Main_Menu [0+Main_Menu_Offset]
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    Screen_Status = View_Log_Screen + Main_Menu_Offset;
                    if (Screen_Status == Change_Pwd_Screen_1) {
                        LCD_SendCMD(CMD_CURRSOR_OFF);
                        LCD_DisStrRC(Passwordmsg, 0, 0);
                    }
                    Button_4_counter = 0;
                }

            } else if (Screen_Status == View_Log_Screen) {
                while (Button_Read(Button_4) == 0);

                if (Memory_Offset > 0) {
                    View_Log_Counter += 16;
                    if (View_Log_Counter > Memory_Offset) {
                        View_Log_Counter = 0;
                    }
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                }
            } else if (Screen_Status == Change_Pwd_Screen_1) {
                while (Button_Read(Button_4) == 0);
                Input_Password[New_Password_Offset] = '1';
                New_Password_Offset++;
                LCD_DisChar('*');
                Password_Waiting_Time = 0;
            } else if (Screen_Status == Set_Time_Screen) {
                while (Button_Read(Button_4) == 0) {
                    Button_4_counter++;
                }
                if (Button_4_counter < 500) {
                    if (Selection_Part == Second_Selection_Part) {
                        RTC_Date.Seconds[1]++;
                        if (RTC_Date.Seconds[1] == 10 + '0') {
                            RTC_Date.Seconds[1] = '0';
                            RTC_Date.Seconds[0]++;
                        }
                        if (RTC_Date.Seconds[0] == 6 + '0') {
                            RTC_Date.Seconds[0] = '0';
                        }
                    } else if (Selection_Part == Minutes_Selection_Part) {
                        RTC_Date.Minutes[1]++;
                        if (RTC_Date.Minutes[1] == 10 + '0') {
                            RTC_Date.Minutes[1] = '0';
                            RTC_Date.Minutes[0]++;
                        }
                        if (RTC_Date.Minutes[0] == 6 + '0') {
                            RTC_Date.Minutes[0] = '0';
                        }
                    }
                    if (Selection_Part == Hours_Selection_Part) {
                        RTC_Date.Hours[1]++;
                        if (RTC_Date.Hours[1] == 10 + '0') {
                            RTC_Date.Hours[1] = '0';
                            RTC_Date.Hours[0]++;
                        }
                        if ((RTC_Date.Hours[0] == '2') && (RTC_Date.Hours[1] == '4')) {
                            RTC_Date.Hours[0] = '0';
                            RTC_Date.Hours[1] = '0';
                        }
                    }
                } else {
                    // set time 
                    temp_ = ((RTC_Date.Seconds[0] - '0') << 4) | (RTC_Date.Seconds[1] - '0');
                    RTC_DS1307_Write(Word_address_Seconds, temp_);

                    temp_ = ((RTC_Date.Minutes[0] - '0') << 4) | (RTC_Date.Minutes[1] - '0');
                    RTC_DS1307_Write(Word_address_Minutes, temp_);

                    temp_ = ((RTC_Date.Hours[0] - '0') << 4) | (RTC_Date.Hours[1] - '0');
                    RTC_DS1307_Write(Word_address_Hours, temp_);
                    // display saving massage 
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    LCD_GotoRC(0, 0);
                    LCD_DisChar('T');
                    LCD_DisChar('I');
                    LCD_DisChar('M');
                    LCD_DisChar('E');
                    LCD_DisChar(' ');
                    LCD_DisChar('I');
                    LCD_DisChar('S');
                    LCD_DisChar(' ');
                    LCD_DisChar('S');
                    LCD_DisChar('E');
                    LCD_DisChar('T');
                    LCD_GotoRC(1, 0);
                    LCD_DisChar('S');
                    LCD_DisChar('U');
                    LCD_DisChar('C');
                    LCD_DisChar('C');
                    LCD_DisChar('E');
                    LCD_DisChar('S');
                    LCD_DisChar('S');
                    LCD_DisChar('F');
                    LCD_DisChar('U');
                    LCD_DisChar('L');
                    LCD_DisChar('Y');
                    __delay_ms(2000);
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    TMR1ON = 0;
                    Screen_Status = Main_Menu_Screen;
                }
            }

        }

        if (Button_Read(Button_5) == 0) {
            if (Screen_Status == Default_Screen) {
                while (Button_Read(Button_5) == 0);
                LCD_SendCMD(CMD_CLEAR_SCREEN);
                LCD_DisStrRC(Passwordmsg, 0, 1);
                Screen_Status = Login_Screen;
            } else if (Screen_Status == Login_Screen) {
                while (Button_Read(Button_5) == 0);
                // save input password
                Input_Password[Pass_Offset] = '0';
                Pass_Offset++;
                LCD_DisChar('*');
            } else if (Screen_Status == Main_Menu_Screen) {
                while (Button_Read(Button_5) == 0) {
                    Button_5_counter++;
                }
                if (Button_5_counter < 500) {
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    Main_Menu_Offset++;
                    if (Main_Menu_Offset > 4) {
                        Main_Menu_Offset = 4;
                    }
                } else {
                    // long press on down button
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    LCD_SendCMD(CMD_CURRSOR_OFF);
                    Screen_Status = Default_Screen;
                    Button_5_counter = 0;

                }
            } else if (Screen_Status == View_Log_Screen) {
                while (Button_Read(Button_5) == 0) {
                    Button_5_counter++;
                }
                if (Button_5_counter < 500) {
                    View_Log_Counter -= 16;
                    if (View_Log_Counter < 0) {
                        View_Log_Counter = 0;
                    }
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                } else {
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    Screen_Status = Main_Menu_Screen;
                    Button_5_counter = 0;
                }
            } else if (Screen_Status == Change_Pwd_Screen_1) {
                while (Button_Read(Button_5) == 0);
                Input_Password[New_Password_Offset] = '0';
                New_Password_Offset++;
                LCD_DisChar('*');
                Password_Waiting_Time = 0;
            } else if (Screen_Status == Set_Time_Screen) {
                switch (Selection_Part) {
                    case Second_Selection_Part:
                        Selection_Part = Minutes_Selection_Part;
                        break;
                    case Minutes_Selection_Part:
                        Selection_Part = Hours_Selection_Part;
                        break;
                    case Hours_Selection_Part:
                        Selection_Part = Second_Selection_Part;
                        break;
                }
            }
        }

        switch (Screen_Status) {
            case (Default_Screen):
                TMR1ON = 0;
                LCD_SendCMD(CMD_CURRSOR_OFF);
                LCD_DisStrRC(LCD_Timemsg, 0, 2);
                LCD_DisStrRC(LCD_Eventmsg, 0, 11);
                LCD_DisStrRC(LCD_Speedmsg, 0, 14);

                LCD_Display_Current_Event(Current_Event);

                Adc_Return_Value = Adc_Read();
                Car_Speed = (Adc_Return_Value * 99) / 1023;
                LCD_2DigitIntToStr(Car_Speed, Adc_Str);
                LCD_DisStrRC(Adc_Str, 1, 14);


                RTC_DS1307_Get_Date(&RTC_Date);
                LCD_Time[7] = RTC_Date.Seconds[1];
                LCD_Time[6] = RTC_Date.Seconds[0];
                LCD_Time[4] = RTC_Date.Minutes[1];
                LCD_Time[3] = RTC_Date.Minutes[0];
                LCD_Time[1] = RTC_Date.Hours[1];
                LCD_Time[0] = RTC_Date.Hours[0];

                LCD_DisStrRC(LCD_Time, 1, 2);
                break;


            case (Login_Screen):
                TMR1ON = 1;
                if (Pass_Offset == 4) {
                    // compare password
                    if (Check_Password() == 0) {
                        LCD_SendCMD(CMD_CLEAR_SCREEN);
                        Screen_Status = Main_Menu_Screen;
                    } else {
                        Password_Trails--;
                        Password_Trails_msg[0] = Password_Trails + '0';
                        LCD_DisStrRC(Password_Trails_msg, 1, 0);
                        __delay_ms(500);
                        LCD_SendCMD(CMD_CLEAR_SCREEN);
                        LCD_DisStrRC(Passwordmsg, 0, 1);

                    }
                    Pass_Offset = 0;
                } else if (Password_Waiting_Time == 50) {
                    // wait 2 sec for next digit in the password 
                    Pass_Offset = 0;
                    Password_Waiting_Time = 0;
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    Screen_Status = Default_Screen;
                }
                LCD_GotoRC(1, Pass_Offset + 6);
                LCD_SendCMD(CMD_CURRSOR_ON);
                break;

            case (Wait_15_Minutes_Screen):
                LCD_SendCMD(CMD_CURRSOR_OFF);
                if (Waiting_Minutes == 0) {
                    Screen_Status = Login_Screen;
                    Waiting_Minutes = 15;
                    TMR1ON = 0;
                } else {
                    TMR1ON = 1;
                    Blocked_Sreen_Line2_msg[8] = (Waiting_Minutes / 10) + '0';
                    Blocked_Sreen_Line2_msg[9] = (Waiting_Minutes % 10) + '0';
                    LCD_DisStrRC(Blocked_Sreen_Line1_msg, 0, 0);
                    LCD_DisStrRC(Blocked_Sreen_Line2_msg, 1, 0);
                }

                break;

            case (Main_Menu_Screen):
                TMR1ON = 0;
                LCD_SendCMD(CMD_CURRSOR_OFF);
                LCD_GotoRC(0, 0);
                LCD_DisChar('*');
                if (Main_Menu_Offset < 4) {
                    LCD_DisStrRC(Main_Menu[0 + Main_Menu_Offset], 0, 2);
                    LCD_DisStrRC(Main_Menu[1 + Main_Menu_Offset], 1, 2);
                } else {
                    LCD_DisStrRC(Main_Menu[0 + Main_Menu_Offset], 0, 2);
                }
                break;

            case (View_Log_Screen):
                TMR1ON = 0;
                LCD_SendCMD(CMD_CURRSOR_OFF);
                LCD_DisStrRC(LCD_Logsmsg, 0, 0);

                LCD_GotoRC(1, 0);
                temp_ = Memory_Offset / 16;
                if (temp_ == 0) {
                    LCD_DisStrRC(LCD_NoLogsmsg, 1, 0);
                    LCD_DisStrRC(LCD_Logsmsg, 1, 3);
                    LCD_DisStrRC(&Clear_Events_msg1[12], 1, 9);
                } else {
                    temp_ = View_Log_Counter / 16;
                    LCD_DisChar(temp_ + '0');

                    temp_ = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x26);
                    LCD_Display_Current_Event(temp_);

                    Adc_Str[0] = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x27);
                    Adc_Str[1] = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x28);
                    Adc_Str[2] = 0;
                    LCD_DisStrRC(Adc_Str, 1, 14);

                    LCD_Time[7] = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x25);
                    LCD_Time[6] = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x24);
                    LCD_Time[4] = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x23);
                    LCD_Time[3] = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x22);
                    LCD_Time[1] = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x21);
                    LCD_Time[0] = E2PROM_24C04_Read(0xA1, View_Log_Counter + 0x20);

                    LCD_DisStrRC(LCD_Time, 1, 2);
                }
                break;

            case (Clear_Log_Screen):
                Memory_Offset = 0;
                // save memory offset
                E2PROM_24C04_Write(0xA0, 0x01, Memory_Offset);
                View_Log_Counter = 0;
                Clear_Events();
                LCD_SendCMD(CMD_CLEAR_SCREEN);
                LCD_DisStrRC(Clear_Events_msg1, 0, 0);
                LCD_DisStrRC(Clear_Events_msg2, 1, 2);
                __delay_ms(1000);
                LCD_SendCMD(CMD_CLEAR_SCREEN);
                Screen_Status = Default_Screen;
                break;

            case(Download_Log_Screen):
                temp_ = E2PROM_24C04_Read(0xA1, 0x01); // Memory_Offset 
                if (temp_ / 16 == 0) {
                    // Send No Logs Available
                    UART_Write('N');
                    UART_Write('o');
                    UART_Write(' ');
                    UART_Write('L');
                    UART_Write('o');
                    UART_Write('g');
                    UART_Write('s');
                    UART_Write(' ');
                    UART_Write('A');
                    UART_Write('v');
                    UART_Write('a');
                    UART_Write('i');
                    UART_Write('L');
                    UART_Write('a');
                    UART_Write('b');
                    UART_Write('l');
                    UART_Write('e');
                    UART_Write('.');
                    UART_Write('.');
                    UART_Write('.');
                    UART_Write('.');
                    UART_Write(10);

                } else {
                    Download_Log_Counter = 0;
                    while (Download_Log_Counter / 16 < 10) {
                        // temp_ is event
                        temp_ = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x26);
                        if ((temp_ == 0x00) || (temp_ == 0xFF)) {
                            break;
                        }

                        UART_Write((Download_Log_Counter / 16) + '0');
                        UART_Write(' ');
                        LCD_Time[7] = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x25);
                        LCD_Time[6] = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x24);
                        LCD_Time[4] = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x23);
                        LCD_Time[3] = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x22);
                        LCD_Time[1] = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x21);
                        LCD_Time[0] = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x20);
                        UART_Write(LCD_Time[0]);
                        UART_Write(LCD_Time[1]);
                        UART_Write(':');
                        UART_Write(LCD_Time[3]);
                        UART_Write(LCD_Time[4]);
                        UART_Write(':');
                        UART_Write(LCD_Time[6]);
                        UART_Write(LCD_Time[7]);
                        UART_Write(' ');


                        UART_Send_Current_Event(temp_); // temp_ is event
                        UART_Write(' ');

                        Adc_Str[0] = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x27);
                        Adc_Str[1] = E2PROM_24C04_Read(0xA1, Download_Log_Counter + 0x28);
                        UART_Write(Adc_Str[0]);
                        UART_Write(Adc_Str[1]);
                        UART_Write(10);

                        Download_Log_Counter += 16;
                    }
                }
                // LCD Display Download Logs Successful
                LCD_SendCMD(CMD_CLEAR_SCREEN);
                LCD_DisStrRC(LCD_Logsmsg, 0, 0);
                LCD_DisChar(' ');
                LCD_DisChar('D');
                LCD_DisChar('o');
                LCD_DisChar('w');
                LCD_DisChar('n');
                LCD_DisChar('l');
                LCD_DisChar('o');
                LCD_DisChar('a');
                LCD_DisChar('d');
                LCD_DisChar('e');
                LCD_DisChar('d');
                LCD_DisStrRC(Clear_Events_msg2, 1, 0);
                __delay_ms(2000);
                LCD_SendCMD(CMD_CLEAR_SCREEN);
                Screen_Status = Main_Menu_Screen;
                break;

            case(Set_Time_Screen):
                TMR1ON = 1;
                LCD_GotoRC(0, 0);
                LCD_DisChar('T');
                LCD_DisChar('I');
                LCD_DisChar('M');
                LCD_DisChar('E');
                LCD_DisChar(' ');
                LCD_DisChar('(');
                LCD_DisChar('H');
                LCD_DisChar('H');
                LCD_DisChar(':');
                LCD_DisChar('M');
                LCD_DisChar('M');
                LCD_DisChar(':');
                LCD_DisChar('S');
                LCD_DisChar('S');
                LCD_DisChar(')');


                if (Selection_Part == Second_Selection_Part) {
                    if (Set_Time_Blinking_flag == 1) {
                        LCD_Time[7] = RTC_Date.Seconds[1];
                        LCD_Time[6] = RTC_Date.Seconds[0];
                        LCD_Time[4] = RTC_Date.Minutes[1];
                        LCD_Time[3] = RTC_Date.Minutes[0];
                        LCD_Time[1] = RTC_Date.Hours[1];
                        LCD_Time[0] = RTC_Date.Hours[0];
                    } else {
                        LCD_Time[7] = ' ';
                        LCD_Time[6] = ' ';
                        LCD_Time[4] = RTC_Date.Minutes[1];
                        LCD_Time[3] = RTC_Date.Minutes[0];
                        LCD_Time[1] = RTC_Date.Hours[1];
                        LCD_Time[0] = RTC_Date.Hours[0];
                    }

                } else if (Selection_Part == Minutes_Selection_Part) {
                    if (Set_Time_Blinking_flag == 1) {
                        LCD_Time[7] = RTC_Date.Seconds[1];
                        LCD_Time[6] = RTC_Date.Seconds[0];
                        LCD_Time[4] = RTC_Date.Minutes[1];
                        LCD_Time[3] = RTC_Date.Minutes[0];
                        LCD_Time[1] = RTC_Date.Hours[1];
                        LCD_Time[0] = RTC_Date.Hours[0];
                    } else {
                        LCD_Time[7] = RTC_Date.Seconds[1];
                        LCD_Time[6] = RTC_Date.Seconds[0];
                        LCD_Time[4] = ' ';
                        LCD_Time[3] = ' ';
                        LCD_Time[1] = RTC_Date.Hours[1];
                        LCD_Time[0] = RTC_Date.Hours[0];
                    }

                } else if (Selection_Part == Hours_Selection_Part) {
                    if (Set_Time_Blinking_flag == 1) {
                        LCD_Time[7] = RTC_Date.Seconds[1];
                        LCD_Time[6] = RTC_Date.Seconds[0];
                        LCD_Time[4] = RTC_Date.Minutes[1];
                        LCD_Time[3] = RTC_Date.Minutes[0];
                        LCD_Time[1] = RTC_Date.Hours[1];
                        LCD_Time[0] = RTC_Date.Hours[0];
                    } else {
                        LCD_Time[7] = RTC_Date.Seconds[1];
                        LCD_Time[6] = RTC_Date.Seconds[0];
                        LCD_Time[4] = RTC_Date.Minutes[1];
                        LCD_Time[3] = RTC_Date.Minutes[0];
                        LCD_Time[1] = ' ';
                        LCD_Time[0] = ' ';
                    }

                }

                LCD_DisStrRC(LCD_Time, 1, 0);
                break;

            case(Change_Pwd_Screen_1):
                TMR1ON = 1;
                if (Password_Waiting_Time == 50) {
                    // wait 2 sec for next digit in the password 
                    New_Password_Offset = 0;
                    Password_Waiting_Time = 0;
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    Screen_Status = Main_Menu_Screen;
                }
                LCD_GotoRC(1, New_Password_Offset + 0);
                LCD_SendCMD(CMD_CURRSOR_ON);

                if (New_Password_Offset == 4) {
                    LCD_SendCMD(CMD_CURRSOR_OFF);
                    // save password
                    E2PROM_24C04_Write(0xA0, 0x05, Input_Password[0]);
                    E2PROM_24C04_Write(0xA0, 0x06, Input_Password[1]);
                    E2PROM_24C04_Write(0xA0, 0x07, Input_Password[2]);
                    E2PROM_24C04_Write(0xA0, 0x08, Input_Password[3]);
                    New_Password_Offset = 0;
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    LCD_DisStrRC(&Passwordmsg[6], 0, 0);
                    LCD_DisStrRC(passwordChangedSuccessfullymsg, 0, 9);
                    LCD_DisStrRC(Clear_Events_msg2, 1, 2);
                    __delay_ms(2000);
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    Screen_Status = Main_Menu_Screen;
                }
                break;

        }

    }
}
