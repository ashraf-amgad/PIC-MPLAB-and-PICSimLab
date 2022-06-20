#define _XTAL_FREQ  8000000UL
#include "config.h"
#include "LCD.h"
#include <stdint.h>
#include <string.h>
#include <xc.h>

void Timer1_Init(void);

uint8_t Massage1[11] = "Stop Watch";
uint8_t Massage2[16] = "Press  Start key";
uint8_t Massage3[7] = "SW:";
uint8_t Massage4[7] = "Ln:";


#define Start_State    1
#define Stop_State     0
uint8_t Start_Stop_Status = Stop_State;

#define Button_Start_Stop_Driction        TRISBbits.TRISB3
#define Button_Start_Stop                 RB3 

#define Button_Lap_Reset_Driction      TRISBbits.TRISB4
#define Button_Lap_Reset               RB4
#define Button_Scroll_up_Driction        TRISBbits.TRISB3
#define Button_Scroll_up                 RB5 
#define Button_Scroll_down_Driction      TRISBbits.TRISB4
#define Button_Scroll_down               RB0

typedef struct {
    uint8_t Lap_1_MilliSeconds[3];
    uint8_t Lap_1_Seconds[3];
    uint8_t Lap_1_Minutes[3];
    uint8_t Lap_1_Hours[3];
} Lap_t;

typedef struct {
    Lap_t Laps[6];
} Laps_t;


uint8_t Lap_MilliSeconds = 0;
uint8_t Lap_Seconds = 0;
uint8_t Lap_Minutes = 0;
uint8_t Lap_Hours = 0;

uint8_t Lap_counter = 0;
int8_t scroll_counter = 0;


uint8_t Reset_flag = 1;
uint8_t LAP_flag = 0;
uint8_t first_lap = 0;

#define TMR1_Load   10000


uint16_t Timer_Counter = 0; // Global Counter Variable


uint8_t INIT_DONE = 0;

void __interrupt() ISR(void) {


    // Check The Flag Bit
    if (TMR1IF) {

        Lap_MilliSeconds++;

        TMR1 = 65536 - TMR1_Load;
        TMR1IF = 0; // Clear The Flag Bit
    }



}

void main() {


    Button_Start_Stop_Driction = 1;
    Button_Lap_Reset_Driction = 1;
    Button_Scroll_up_Driction = 1;
    Button_Scroll_down_Driction = 1;


    LCD_Init();
    Timer1_Init();

    Laps_t Laps;
    Lap_t temp;



    LCD_DisStrRC(Massage1, 0, 3);
    LCD_DisStrRC(Massage2, 1, 0);



    INIT_DONE = 0;
    uint8_t save_counter = 0;

    while (1) {
        if (Lap_MilliSeconds > 100) {
            Lap_Seconds++;
            Lap_MilliSeconds = 0;
        }

        if (Lap_Seconds == 60) {
            Lap_Minutes++;
            Lap_Seconds = 0;
        }

        if (Lap_Minutes == 60) {
            Lap_Hours++;
            Lap_Minutes = 0;
        }

        if (Lap_Hours == 60) {
            Lap_Hours = 0;
        }

        if (Button_Start_Stop == 0) {
            while (Button_Start_Stop == 0);
            switch (Start_Stop_Status) {
                case Start_State:
                    TMR1ON = 0;
                    Start_Stop_Status = Stop_State;
                    break;

                case Stop_State:
                    LCD_SendCMD(CMD_CLEAR_SCREEN);
                    LCD_DisStrRC(Massage3, 0, 0);
                    TMR1ON = 1;
                    Start_Stop_Status = Start_State;

                    if (INIT_DONE == 0) {
                        /* LCD_GotoRC(0, 7);
                         LCD_DisChar(':');
                         LCD_GotoRC(0, 10);
                         LCD_DisChar(':');
                         LCD_GotoRC(0, 13);
                         LCD_DisChar(':');*/
                    }
                    INIT_DONE = 1;
                    break;
            }
        }


        if (Button_Scroll_up == 0) {
            while (Button_Scroll_up == 0);
            if (Start_Stop_Status == Stop_State) {
                scroll_counter++;
                if (scroll_counter > 4) {
                    scroll_counter = 0;
                }
            }
        }

        if (Button_Scroll_down == 0) {
            while (Button_Scroll_down == 0);
            if (Start_Stop_Status == Stop_State) {
                scroll_counter--;
                if (scroll_counter < 0) {
                    scroll_counter = 4;
                }
            }
        }


        if (Button_Lap_Reset == 0) {
            while (Button_Lap_Reset == 0);
            switch (Start_Stop_Status) {
                case Start_State:
                    Lap_counter++;
                    save_counter++;
                    if (Lap_counter > 5) {
                        Lap_counter = 1;
                    }

                    LAP_flag = 1;
                    first_lap = 1;
                    break;

                case Stop_State:
                    Reset_flag = 1;
                    break;
            }
        }


        if (Reset_flag == 1) {
            // reset the laps
            uint8_t i = 0;
            for (i = 0; i < 5; i++) {
                Laps.Laps[i].Lap_1_MilliSeconds[0] = '0';
                Laps.Laps[1].Lap_1_Seconds[0] = '0';
                Laps.Laps[i].Lap_1_Minutes[0] = '0';
                Laps.Laps[i].Lap_1_Hours[0] = '0';
                Laps.Laps[i].Lap_1_MilliSeconds[1] = ' ';
                Laps.Laps[1].Lap_1_Seconds[1] = ' ';
                Laps.Laps[i].Lap_1_Minutes[1] = ' ';
                Laps.Laps[i].Lap_1_Hours[1] = ' ';
            }
            Reset_flag = 0;
        }


        if (LAP_flag == 1) {
            // save the lap
            LCD_Int2Str(Lap_Hours, Laps.Laps[Lap_counter - 1].Lap_1_MilliSeconds);
            LCD_Int2Str(Lap_Minutes, Laps.Laps[Lap_counter - 1].Lap_1_Seconds);
            LCD_Int2Str(Lap_Seconds, Laps.Laps[Lap_counter - 1].Lap_1_Minutes);
            LCD_Int2Str(Lap_MilliSeconds, Laps.Laps[Lap_counter - 1].Lap_1_Hours);
            LAP_flag = 0;
        }






        switch (Start_Stop_Status) {
            case Start_State:
                LCD_Int2Str(Lap_Hours, temp.Lap_1_MilliSeconds);
                LCD_DisStrRC(temp.Lap_1_MilliSeconds, 0, 5);

                LCD_Int2Str(Lap_Minutes, temp.Lap_1_Seconds);
                LCD_DisStrRC(temp.Lap_1_Seconds, 0, 8);

                LCD_Int2Str(Lap_Seconds, temp.Lap_1_Minutes);
                LCD_DisStrRC(temp.Lap_1_Minutes, 0, 11);

                LCD_Int2Str(Lap_MilliSeconds, temp.Lap_1_Hours);
                LCD_DisStrRC(temp.Lap_1_Hours, 0, 14);

                if (first_lap == 1) {
                    LCD_DisStrRC(Laps.Laps[Lap_counter - 1].Lap_1_MilliSeconds, 1, 5);
                    LCD_DisStrRC(Laps.Laps[Lap_counter - 1].Lap_1_Seconds, 1, 8);
                    LCD_DisStrRC(Laps.Laps[Lap_counter - 1].Lap_1_Minutes, 1, 11);
                    LCD_DisStrRC(Laps.Laps[Lap_counter - 1].Lap_1_Hours, 1, 14);

                    Massage4[1] = Lap_counter - 1 + '0';
                    LCD_DisStrRC(Massage4, 1, 0);
                }
                break;

            case Stop_State:
                if (INIT_DONE == 1) {
                    LCD_DisStrRC(Laps.Laps[scroll_counter].Lap_1_MilliSeconds, 1, 5);
                    LCD_DisStrRC(Laps.Laps[scroll_counter].Lap_1_Seconds, 1, 8);
                    LCD_DisStrRC(Laps.Laps[scroll_counter].Lap_1_Minutes, 1, 11);
                    LCD_DisStrRC(Laps.Laps[scroll_counter].Lap_1_Hours, 1, 14);
                    
                    Massage4[1] = scroll_counter + '0';
                    LCD_DisStrRC(Massage4, 1, 0);
                    break;
                }
        }


    }

}

void Timer1_Init(void) {

    TMR1 = 65536 - TMR1_Load;
    // Choose the local clock source (timer mode)
    TMR1CS = 0;
    // Choose the desired prescaler ratio (1:1)
    T1CKPS0 = 0;
    T1CKPS1 = 0;

    // Switch ON Timer1 Module!
    TMR1ON = 0;
    // -- [[ Interrupts Configurations ]] --
    TMR1IE = 1; // Timer1 Interrupt Enable Bit
    TMR1IF = 0; // Clear The Interrupt Flag Bit
    PEIE = 1; // Peripherals Interrupts Enable Bit
    GIE = 1; // Global Interrupts Enable Bit
}