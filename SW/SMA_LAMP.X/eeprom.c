#include <xc.h>
#include <proc/pic16f886.h>

void EEPROM_Write(uint8_t address, uint8_t data) 
{
    EEADR = address;
    EEDATA = data;
    EECON1bits.EEPGD = 0; // Point to EEPROM Memory
    //EECON1bits.CFGS = 0; // Access to EEPROM
    EECON1bits.WREN = 1; // Enable Write
    //DISABLE_INT();
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1; // Begin Write
    //ENABLE_INT();
    INTCONbits.GIE = 1;
    while (PIR2bits.EEIF == 0); // Wait for Write to get Complete
    PIR2bits.EEIF = 0; // Clear Flag
    EECON1bits.WREN = 0; // Disable Write
}

void EEPROM_Write_Block(uint8_t address, uint8_t *data, uint8_t length) 
{
    uint8_t i = 0;
    for (i = 0; i < length; i++) 
    {
        EEPROM_Write(address + i, *(data + i));
    }
}

uint8_t EEPROM_Read(uint8_t address) 
{
    uint8_t data;
    EEADR = address;
    EECON1bits.EEPGD = 0; // Point to EEPROM
    //EECON1bits.CFGS = 0; // Access to EEPROM
    EECON1bits.RD = 1; // Enable Read
    data = EEDATA;
    __nop();
    __nop();
    __nop();
    return data;
}

void EEPROM_Read_Block(uint8_t address, uint8_t *data, uint8_t length) 
{
    uint8_t i = 0;
    for (i = 0; i < length; i++) 
    {
        *(data + i) = EEPROM_Read(address + i);
    }
}