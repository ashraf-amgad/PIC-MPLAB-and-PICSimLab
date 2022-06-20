#include "RTC_DS1307.h"

void RTC_DS1307_Init(void) {
    I2C_Master_Init();
}

void RTC_DS1307_Write(uint8_t word_Address, uint8_t Data) {
    I2C_Start();
    I2C_Write(Device_Address); // Slave I2C Device Address + Write
    I2C_Write(word_Address); // word address 
    I2C_Write(Data); // Data To Be Sent
    I2C_Stop();
}

uint8_t RTC_DS1307_Read(uint8_t word_Address) {

    uint8_t temp_Device_Address;
    I2C_Start();
    temp_Device_Address = Device_Address & ~(0x01);
    I2C_Write(temp_Device_Address); // Slave I2C Device Address + Write
    I2C_Write(word_Address); // word address 
    I2C_Start(); // I2C Start Sequence
    temp_Device_Address = Device_Address | (0x01);
    I2C_Write(temp_Device_Address); // Slave I2C Device Address + read
    temp_Device_Address = I2C_Read();
    I2C_Stop();
    return (temp_Device_Address);

}

uint8_t RTC_DS1307_BCD_To_Decimal_MSB(uint8_t BCD_Number) {
    uint8_t temp = (BCD_Number >> 4) + '0';

    return (temp);
}

uint8_t RTC_DS1307_BCD_To_Decimal_LSB(uint8_t BCD_Number) {
    uint8_t temp = (BCD_Number & 0x0f) + '0';

    return (temp);
}

uint8_t* RTC_DS1307_DOW_To_String(uint8_t Day_Of_The_Week_Integer) {

    return DOW_To_String[Day_Of_The_Week_Integer];
}



void RTC_DS1307_Get_Date(RTC_Date_Typedef* Date_Structure) {
    RTC_temp_Typedef temp;
    
    temp.Seconds = RTC_DS1307_Read(Word_address_Seconds);
    temp.Minutes = RTC_DS1307_Read(Word_address_Minutes);
    temp.Hours = RTC_DS1307_Read(Word_address_Hours);
    if ((temp.Hours & 0b00100000) != 0) {
        Date_Structure->am_Pm = Time_pm;
    } else {
        Date_Structure->am_Pm = Time_am;
    }
    temp.Hours &= 0x1f;
    temp.DOW = RTC_DS1307_Read(Word_address_DayOfWeek);
    Date_Structure->DOW_String = RTC_DS1307_DOW_To_String(temp.DOW);
    temp.Day = RTC_DS1307_Read(Word_address_Day);
    temp.Month = RTC_DS1307_Read(Word_address_Month);
    temp.Year = RTC_DS1307_Read(Word_address_Year);

    Date_Structure->Seconds[1] = RTC_DS1307_BCD_To_Decimal_LSB(temp.Seconds);
    Date_Structure->Seconds[0] = RTC_DS1307_BCD_To_Decimal_MSB(temp.Seconds);
    Date_Structure->Minutes[1] = RTC_DS1307_BCD_To_Decimal_LSB(temp.Minutes);
    Date_Structure->Minutes[0] = RTC_DS1307_BCD_To_Decimal_MSB(temp.Minutes);
    Date_Structure->Hours[1] = RTC_DS1307_BCD_To_Decimal_LSB(temp.Hours);
    Date_Structure->Hours[0] = RTC_DS1307_BCD_To_Decimal_MSB(temp.Hours);
    Date_Structure->Day[1] = RTC_DS1307_BCD_To_Decimal_LSB(temp.Day);
    Date_Structure->Day[0] = RTC_DS1307_BCD_To_Decimal_MSB(temp.Day);
    Date_Structure->Month[1] = RTC_DS1307_BCD_To_Decimal_LSB(temp.Month);
    Date_Structure->Month[0] = RTC_DS1307_BCD_To_Decimal_MSB(temp.Month);
    Date_Structure->Year[1] = RTC_DS1307_BCD_To_Decimal_LSB(temp.Year);
    Date_Structure->Year[0] = RTC_DS1307_BCD_To_Decimal_MSB(temp.Year);

}
