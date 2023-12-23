/*
 * File:   main.c
 * Author: ciruu
 *
 * Created on 23 de noviembre de 2023, 10:22
 */

// TODO cambiar los nombres del diagrama de alto nivel de Espaï¿½ol a Inglï¿½s


#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF

#include <xc.h>
#include <proc/pic16f886.h>
#include <stdio.h>

// SPI
#include "SPI/spi-master-v1.c"

// I2C
#include "I2C/i2c-v2.c"

// With this library we configure the main settings on the MCU
#include "Init/initall.c"

// EEPROM
#include "eeprom.c"
// I2C lib


// I2C
#define I2C_ADDRESS 0x5A   // Dirección I2C del sensor iAQ-Core (7 bits, sin incluir el bit de lectura/escritura)
#define I2C_READ_CMD 0xB5  // Comando de lectura

#define VEML7700_ALS_DATA 0x04        /*!< The light data output */
#define VEML7700_WHITE_DATA 0x05        /*!< The white light data output */

#define VEML7700_GAIN_1_8 0x02          /*!< ALS gain 1/8x */
#define VEML7700_IT_100MS 0x00          /*!< ALS intetgration time 100ms */
#define VEML7700_PERS_1 0x00            /*!< ALS irq persisance 1 sample */
#define VEML7700_POWERSAVE_MODE1 0x00   /*!< Power saving mode 1 */


// Definiciones para la configuración del I2C y el sensor VEML7700
#define VEML7700_ADDRESS_WRITE 0x20  // Dirección I2C del sensor VEML7700 en modo escritura
#define VEML7700_ADDRESS_READ  0x21  // Dirección I2C del sensor VEML7700 en modo lectura
#define VEML7700_COMMAND_CODE  0x00  // Código de comando para lectura de Lux

#define UART_STRING_TAM 7

// EEPROM
#define RED_ADDRESS 0x1000
#define GREEN_ADDRESS 0x1001
#define BLUE_ADDRESS 0x1002
#define INTEN_ADDRESS 0x1003
#define SPEED_ADDRESS 0x1004
#define DATA_ADDRESS 0x1005

int uart_complete = 0;
int counter;
int counterRuido10ms, counterRuido1seg, counter5seg; // 2 ticks, 200 ticks, 1000 ticks
unsigned int NoiseValue, HumValue, TempValue;
unsigned char currentNoiseValue = 0;

//USART
unsigned char receivedString[UART_STRING_TAM];
int currentIndex = 0;
int endString = 0;


// SPI
unsigned char red = 0xFF;
unsigned char green = 0xFF;
unsigned char blue = 0xFF;
unsigned char alpha = 0x0F;
unsigned char color_flag = 0;

// FAN
unsigned char fan_speed;
unsigned char init_fan = 0;

//EEPROM
unsigned char eeprom_save = 0;


void putch(char data)
{
    while (!TXIF); // Espera hasta que el registro de transmisión esté listo para enviar
    TXREG = data; // Carga el carácter en el registro de transmisión
}

void VEML7700_Init() 
{
    i2c_start();
    if (i2c_write(VEML7700_ADDRESS_WRITE))
    {
        //printf("I2C INIT\n");
        i2c_write(VEML7700_COMMAND_CODE);
        i2c_write(0b00000000);
        i2c_write(0b00010000); //0b00010000
        i2c_stop();
    }
}

int VEML7700_ReadLux() 
{
    unsigned int luxValue = 0;
    
    i2c_start();
    if (i2c_write(VEML7700_ADDRESS_WRITE))
    {
        if (i2c_write(VEML7700_WHITE_DATA)) 
        {

            i2c_start();
            if (i2c_write(VEML7700_ADDRESS_READ))
            {
                uint8_t read_data[2];
                read_data[0] = i2c_read(1);  // Leer el byte alto de Lux
                read_data[1] = i2c_read(0);      // Leer el byte bajo de Lux
                luxValue = read_data[1] << 8 | read_data[0];
                //luxValue = read_data[0];
                i2c_stop(); // Enviar condición de parada
            }
        
        }
        
    }
    
                      
   
    return luxValue;
}

void read_co2()
{
    uint8_t data[10];
    uint16_t prediccion;
    uint8_t status;
    int32_t resistencia;
    uint16_t Tvoc;

    i2c_start();

    if(i2c_write(0xB5))            //si devuelve ack
    {
        data[0] = i2c_read(1);
        data[1] = i2c_read(1);
        data[2] = i2c_read(1);
        data[3] = i2c_read(1);
        data[4] = i2c_read(1);
        data[5] = i2c_read(1);
        data[6] = i2c_read(0);
        data[7] = i2c_read(1);
        data[8] = i2c_read(0);

        i2c_stop();
    }
    

    prediccion = (data[0] * 256) + data[1];
    status = data[2];
    resistencia = (data[4] * 65536) + (data[5] * 256) + data[6];
    Tvoc = (data[7] * 256) + data[8];
    
    if(data[2] == 0x00) // OK
    {
        printf("PP %u\n", (data[0] << 7) | data[1]);
    }
    else if(data[2] == 0x10) // RUNIN
    {
        printf("WAR\n");
    }
    else if(data[2] == 0x01) // BUSY
    {
        printf("BUS\n");
    }
    else if(data[2] == 0x80) // ERROR
    {
        printf("ERR\n");
    }
    else // UNKOWN
    {
        printf("UNK\n");
    }
}


void change_color(unsigned char brigthness, unsigned char red, unsigned char green, unsigned char blue, int n) 
{

    unsigned char init = 0b11100000;
    unsigned char global = init | brigthness;
    // inicio de trama
    for(int i = 0; i < 4; i++)
    {
        spi_write_read((char)0x00);
    }

    // inicio de LED Frame

    for (int i = 0; i < n; i++)
    {
        spi_write_read((char)global);
        spi_write_read((char)blue);// Blue
        spi_write_read((char)green);// Green
        spi_write_read((char)red);// Red
    }

    // fin de trama
    for(int i = 0; i < 4; i++)
    {
        spi_write_read((char)0xFF);
    }

}

int readADC(int channel)
{
    ADCON0bits.CHS = channel; // AN8
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ((ADRESH) << 8) | ADRESL;
}

uint8_t UART_GetC()
{
  while (RCIF == 0) ;  // wait for data receive
  if (OERR)  // if there is overrun error
  {  // clear overrun error bit
    CREN = 0;
    CREN = 1;
  }
  return RCREG;        // read from EUSART receive data register
}

/*
 * 
 * BEHAVIOUR BLOCK IN HIGH LEVEL DIAGRAM
 * 
 */
void __interrupt() int_routine(void)
{
    //UART
    if (PIR1bits.RCIF)
    {
        PIR1bits.RCIF = 0;
        if (currentIndex == UART_STRING_TAM || uart_complete == 1)
        {
            return;
        }
        
        receivedString[currentIndex] = UART_GetC(); // Lee el carácter recibido
        
        if (receivedString[currentIndex] == '\r')
        {
            uart_complete = 1;
        }
        
        currentIndex++;
        
    }
    if (INTCONbits.T0IF)
    {
        counter += 1;
        counterRuido10ms += 1;
        counterRuido1seg += 1;
        counter5seg += 1;
        TMR0 = 159;
        INTCONbits.T0IF = 0;
        if (counterRuido10ms >= 2) /* B-20 */
        {
            counterRuido10ms = 0;
            NoiseValue = readADC(8); // AN8
            if (NoiseValue >= currentNoiseValue)
            {
                currentNoiseValue = NoiseValue;
            } 
        }
        if (counterRuido1seg >= 200) /* B-40 */
        {
            counterRuido1seg = 0;

            printf("NO %u\n", currentNoiseValue);

            currentNoiseValue = 0;
        }
        if (counter5seg >= 1000) /* B-50 */
        {
            counter5seg = 0;
            
            printf("LU %u\n", VEML7700_ReadLux()); // Lux
            HumValue = readADC(12); //Humidity
            TempValue = readADC(10); //Temperature
            printf("HU %u\n", HumValue);
            printf("TE %u\n", TempValue);
            read_co2(); // CO2
        }
        if (uart_complete != 0)
        {
            uart_complete = 0;
            currentIndex = 0;
            if (receivedString[0] = 'R')
            {
                change_color(receivedString[4], receivedString[1], receivedString[2], receivedString[3], 10);
                color_flag = 1;
                red = receivedString[1];
                green = receivedString[2];
                blue = receivedString[3];
                alpha = receivedString[4];
                fan_speed = receivedString[5];
                CCPR1L = fan_speed;
                for (int i = 0; i < UART_STRING_TAM; i++)
                {
                    receivedString[i] = 0;
                }
                eeprom_save = 1;
            }
        }
        if (init_fan != 0)
        {
            init_fan = 0;
            CCPR1L = fan_speed;
        }
    }
    
}

void main(void)
{
    init(); // We call the initialize function in the init library 
    counter = 0;
    counterRuido10ms = 0;
    counterRuido1seg = 0;
    counter5seg = 0;
    if (EEPROM_Read(DATA_ADDRESS) != 0) 
    {
        
        red = EEPROM_Read(RED_ADDRESS);
        green = EEPROM_Read(GREEN_ADDRESS);
        blue = EEPROM_Read(BLUE_ADDRESS);
        alpha = EEPROM_Read(INTEN_ADDRESS);
        fan_speed = EEPROM_Read(SPEED_ADDRESS);
        init_fan = 1;
        change_color(alpha, red, green, blue, 10);
    }
    else 
    {
        printf("NO MEM\n");
        change_color(alpha, red, green, blue, 10);
    }
    
    //init_pwm();
    VEML7700_Init();
    while (1) /* B-10 */
    {
        if (eeprom_save != 0)
        {
            
            eeprom_save = 0;
            EEPROM_Write(RED_ADDRESS, red);
            EEPROM_Write(GREEN_ADDRESS, green);
            EEPROM_Write(BLUE_ADDRESS, blue);
            EEPROM_Write(INTEN_ADDRESS, alpha);
            EEPROM_Write(SPEED_ADDRESS, fan_speed);
            EEPROM_Write(DATA_ADDRESS, 0xFF);
        }
    }
    return;
}
