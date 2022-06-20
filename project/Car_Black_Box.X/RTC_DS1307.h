#include <stdint.h>
#include <xc.h>
#include "I2C.h"



#define Device_Address      0xD0

#define Word_address_Seconds        0x00
#define Word_address_Minutes        0x01
#define Word_address_Hours          0x02
#define Word_address_DayOfWeek      0x03
#define Word_address_Day            0x04
#define Word_address_Month          0x05
#define Word_address_Year           0x06


uint8_t DOW_To_String[8][3] = {"Su" , "Mo" , "Tu" , "We" , "Th" , "Fr" , "Sa"};


#define Time_am     0
#define Time_pm     1

typedef struct{
    uint8_t Seconds[2];
    uint8_t Minutes[2];
    uint8_t Hours[2];
    uint8_t am_Pm;
    uint8_t* DOW_String;
    uint8_t Day[2];
    uint8_t Month[2];
    uint8_t Year[2];
}RTC_Date_Typedef;

typedef struct{
    uint8_t Seconds;
    uint8_t Minutes;
    uint8_t Hours;
    uint8_t am_Pm;
    uint8_t DOW;
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;
}RTC_temp_Typedef;



void RTC_DS1307_Init(void);

void RTC_DS1307_Write(uint8_t word_Address , uint8_t Data);

uint8_t RTC_DS1307_Read(uint8_t word_Address);

uint8_t RTC_DS1307_BCD_To_Decimal_MSB(uint8_t BCD_Number);

uint8_t RTC_DS1307_BCD_To_Decimal_LSB(uint8_t BCD_Number);

uint8_t* RTC_DS1307_DOW_To_String(uint8_t Day_Of_The_Week_Integer);

void RTC_DS1307_Get_Date(RTC_Date_Typedef* Date_Structure);