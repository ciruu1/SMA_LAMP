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
#include <SPI/spi-master-v1.h>

// I2C
#include <I2C/i2c-v2.h>

// With this library we configure the main settings on the MCU
#include "Init/initall.c"
// I2C lib


// I2C
#define I2C_ADDRESS 0x5A   // Dirección I2C del sensor iAQ-Core (7 bits, sin incluir el bit de lectura/escritura)
#define I2C_READ_CMD 0xB5  // Comando de lectura

// Definiciones para la configuración del I2C y el sensor VEML7700
#define VEML7700_ADDRESS_WRITE 0x20  // Dirección I2C del sensor VEML7700 en modo escritura
#define VEML7700_ADDRESS_READ  0x21  // Dirección I2C del sensor VEML7700 en modo lectura
#define VEML7700_COMMAND_CODE  0x00  // Código de comando para lectura de Lux


int counter;
int counterRuido10ms, counterRuido1seg, counter5seg; // 2 ticks, 200 ticks, 1000 ticks
unsigned int NoiseValue, HumValue, TempValue;
unsigned char noiseValues[10];
int noiseCounterValues; // 0..9
int maxNoise = 0;

//USART
unsigned char receivedString[20];
int currentIndex = 0;
int endString = 0;

// I2C

unsigned char co2Data[3]; // prediction0, prediction1, status

// SPI
unsigned char red = 0xFF;
unsigned char green = 0xFF;
unsigned char blue = 0xFF;
unsigned char alpha = 0xF0;

// FAN
unsigned char fan_speed = 0;


int VEML7700_ReadLux() {
    unsigned int luxValue = 0;

    // Comunicarse con el sensor VEML7700
    i2c_start();                  // Iniciar condición de inicio
    i2c_write(VEML7700_ADDRESS_WRITE);  // Enviar la dirección del dispositivo con el bit de escritura
    i2c_write(VEML7700_COMMAND_CODE);   // Enviar el código de comando para lectura de Lux
    i2c_stop();                   // Enviar condición de parada

    // Leer los dos bytes de Lux del sensor VEML7700
    i2c_start();                  // Iniciar condición de inicio
    if (i2c_write(VEML7700_ADDRESS_READ)) {// Enviar la dirección del dispositivo con el bit de lectura
        luxValue = i2c_read(1) << 8;  // Leer el byte alto de Lux
        //I2C_Ack();                   // Enviar ACK para indicar que se espera otro byte

        luxValue |= i2c_read(0);      // Leer el byte bajo de Lux
        //I2C_Nack();                  // Enviar NACK para indicar que no se esperan más bytes
        i2c_stop();                  // Enviar condición de parada
    }   

   
    return luxValue;
}

int read_co2()
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
    
    
    //printf("%d\r\n", prediccion);
    //printf("%d\r\n", status);
    //printf("%d\r\n", resistencia);
    printf("%d\r\n", Tvoc);
    
    return prediccion;
}

void send_CO2()
{
    if(co2Data[2] == 0x00) // OK
    {
        printf("PPM %u\n", (co2Data[0] << 7) | co2Data[1]);
    }
    else if(co2Data[2] == 0x10) // RUNIN
    {
        printf("WARMUP\n");
    }
    else if(co2Data[2] == 0x01) // BUSY
    {
        printf("BUSY\n");
    }
    else if(co2Data[2] == 0x80) // ERROR
    {
        printf("ERROR\n");
    }
    else // UNKOWN
    {
        printf("UNKOWN\n");
    }
}

void change_color(unsigned char brigthness, unsigned char red, unsigned char green, unsigned char blue, int n) {

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

void putch(char data)
{
    while (!TXIF); // Espera hasta que el registro de transmisión esté listo para enviar
    TXREG = data; // Carga el carácter en el registro de transmisión
}

int readADC(int channel)
{
    ADCON0bits.CHS = channel; // AN8
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ((ADRESH) << 8) | ADRESL;
}

/*
 * 
 * BEHAVIOUR BLOCK IN HIGH LEVEL DIAGRAM
 * 
 */
void __interrupt() int_routine(void)
{
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
            if (noiseCounterValues >= 10)
            {
                noiseCounterValues = 0;
            }
            noiseValues[noiseCounterValues] = NoiseValue; // B-30
            noiseCounterValues++;
            // Leer ruido 10 ms
        }
        if (counterRuido1seg >= 200) /* B-40 */
        {
            counterRuido1seg = 0;
            maxNoise = 0;
            for (int i = 0; i < 10; i++)
            {
                if (noiseValues[i] >= maxNoise)
                {
                    maxNoise = noiseValues[i];
                }
            }
            printf("NOISE %u\n", maxNoise);
        }
        if (counter5seg >= 1000) /* B-50 */
        {
            counter5seg = 0;
            HumValue = readADC(12);
            TempValue = readADC(10);
            //printf("HUMIDITY %u\n", HumValue);
            //printf("TEMPERATURE %u\n", TempValue);
            
            printf("HUMIDITY %f\n", (((((float)HumValue/1024.0) * 5.0) - 0.826) / 0.0315));
            printf("TEMPERATURE %f\n", ((((float)TempValue/1024.0) * 5.0) / 0.01));
            // I2C
            // TODO REWRITE I2C
            printf("PPM %u", read_co2());
            printf("LUX %u\n", VEML7700_ReadLux());
            // Leer Sensores
        }
    }
    //UART
    if (PIR1bits.RCIF)
    {
        PIR1bits.RCIF = 0;
        receivedString[currentIndex] = RCREG; // Lee el carácter recibido
        currentIndex++;
        if (receivedString[currentIndex] == '\n')
        {
            // Se procesan los comandos
            for (int i = 0; i < currentIndex; i++)
            {
                /* B-70 */
                if (receivedString[i] == "L" && currentIndex >= 5) { // We check that the command received has at least 5 chars: "LRGBA" being RGBA the color
                    currentIndex = 0;
                    red = receivedString[i+1];
                    green = receivedString[i+2];
                    blue = receivedString[i+3];
                    alpha = receivedString[i+4];
                    change_color(alpha, red, green, blue, 10); // 10 LEDs
                }
                /* B-80 */
                else if (receivedString[i] == "V" && currentIndex >= 2) { // We check that the command received has at least 2 chars: "VS" being S the speed
                    currentIndex = 0;
                    fan_speed = receivedString[i+1];
                    CCPR1L = (unsigned char) ((float) fan_speed / 100.0 * 167); // Convierte el porcentaje a un valor adecuado para CCPR1L
                    // Ventilador
                }
                else {
                    currentIndex = 0;
                    return; // Be careful with the logic below this, maybe we can supress this statement.
                }
            }
        }
        // Verifica si la cadena está completa y no se recibió '\n'
        if (currentIndex == sizeof (receivedString) - 1 && receivedString[currentIndex - 1] != '\n')
        {
            currentIndex = 0; // Reinicia el índice si se produce un desbordamiento
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
    noiseCounterValues = 0;
    //printf("Bienvenido a Lampara inteligente!!\n"); //this will be send it by UART since putch was redefined.
    while (1); /* B-10 */
    return;
}
