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
// I2C lib


// I2C
#define I2C_ADDRESS 0x5A   // Dirección I2C del sensor iAQ-Core (7 bits, sin incluir el bit de lectura/escritura)
#define I2C_READ_CMD 0xB5  // Comando de lectura

// Definiciones para la configuración del I2C y el sensor VEML7700
#define VEML7700_ADDRESS_WRITE 0x20  // Dirección I2C del sensor VEML7700 en modo escritura
#define VEML7700_ADDRESS_READ  0x21  // Dirección I2C del sensor VEML7700 en modo lectura
#define VEML7700_COMMAND_CODE  0x00  // Código de comando para lectura de Lux

#define UART_STRING_TAM 7

int uart_complete = 0;
int counter;
int counterRuido10ms, counterRuido1seg, counter5seg; // 2 ticks, 200 ticks, 1000 ticks
unsigned int NoiseValue, HumValue, TempValue;
//unsigned char noiseValues[10];
unsigned char currentNoiseValue = 0;
//int noiseCounterValues; // 0..9
//int maxNoise = 0;

//USART
unsigned char receivedString[UART_STRING_TAM];
int currentIndex = 0;
int endString = 0;

// I2C

//unsigned char co2Data[3]; // prediction0, prediction1, status

// SPI
unsigned char red = 0xFF;
unsigned char green = 0xFF;
unsigned char blue = 0xFF;
unsigned char alpha = 0xF0;

// FAN
unsigned char fan_speed = 0;


void putch(char data)
{
    while (!TXIF); // Espera hasta que el registro de transmisión esté listo para enviar
    TXREG = data; // Carga el carácter en el registro de transmisión
}
/*
void debug(char num)
{
    putch(num);
    putch('\n');
}


void send_string(const char *str)
{
    // Verifica que el puntero no sea nulo
    if (str == NULL) {
        //printf("Error: El puntero al string es nulo.\n");
        return;
    }

    // Itera sobre cada caracter del string
    while (*str != '\0') {
        // Llama a la función printchar con el caracter actual
        sendchar(*str);
        // Incrementa el puntero al siguiente caracter
        str++;
    }
}

*/
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
    
    //send_string("PPM");
    //putch('P');
    //putch(' ');
    //INTCONbits.GIE = 0;
    if(data[2] == 0x00) // OK
    {
        printf("PP %u\n", (data[0] << 7) | data[1]);
        //putch(data[0]);
        //putch(data[1]);
    }
    else if(data[2] == 0x10) // RUNIN
    {
        printf("PP WAR\n");
        //send_string("WAR");
        //putch('W');
        //putch('A');
        //putch('R');
    }
    else if(data[2] == 0x01) // BUSY
    {
        printf("PP BUS\n");
        //send_string("BUS");
        //putch('B');
        //putch('U');
        //putch('S');
    }
    else if(data[2] == 0x80) // ERROR
    {
        printf("PP ERR\n");
        //send_string("ERR");
        //putch('E');
        //putch('R');
        //putch('R');
    }
    else // UNKOWN
    {
        printf("PP UNK\n");
        //send_string("UNK");
        //putch('U');
        //putch('N');
        //putch('K');
    }
    //INTCONbits.GIE = 1;
    //putch('\n');
    
    
    //printf("%d\r\n", prediccion);
    //printf("%d\r\n", status);
    //printf("%d\r\n", resistencia);
    //printf("%d\r\n", Tvoc);
    
    //return prediccion;
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
/*

void change_color(uint8_t brillo, uint8_t r, uint8_t g, uint8_t b)
{
    spi_write_read(0);
    spi_write_read(0);
    spi_write_read(0);
    spi_write_read(0);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);
    

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 155));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);


    spi_write_read(0xFF);
    spi_write_read(0xFF);
    spi_write_read(0xFF);
    spi_write_read(0xFF);

}
*/

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
            //debug('2'); // DEBUG
            counterRuido10ms = 0;
            NoiseValue = readADC(8); // AN8
            if (NoiseValue >= currentNoiseValue)
            {
                currentNoiseValue = NoiseValue;
            }
            //noiseValues[noiseCounterValues] = NoiseValue; // B-30
            //noiseCounterValues++;
            // Leer ruido 10 ms
            if (uart_complete != 0)
            {
                uart_complete = 0;
                currentIndex = 0;
                if (receivedString[0] = 'R')
                {
                    change_color(receivedString[4], receivedString[1], receivedString[2], receivedString[3], 10);
                    CCPR1L = (receivedString[5] * 167) / 100;
                    for (int i = 0; i < UART_STRING_TAM; i++)
                    {
                        putch(receivedString[i]);
                        receivedString[i] = 0;
                    }
                    putch('\n');
                }
                
                
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
            printf("LU %u\n", VEML7700_ReadLux());

            HumValue = readADC(12);
            TempValue = readADC(10);
            printf("HU %u\n", HumValue);
            printf("TE %u\n", TempValue);
            

            read_co2();
            
            // Leer Sensores
        }
    }
    //UART
    if (PIR1bits.RCIF)
    {
        
        
        receivedString[currentIndex] = UART_GetC(); // Lee el carácter recibido
        PIR1bits.RCIF = 0;
        
        
        //putch('X');
        //putch(receivedString[currentIndex]);
        //putch('\r');
        //putch('\n');
        /*
        if (receivedString[currentIndex] == '\r')
        {
            //change_color(receivedString[4], receivedString[1], receivedString[2], receivedString[3], 10);
            //printf("%c%c%c%c%c", receivedString[0], receivedString[1], receivedString[2], receivedString[3], receivedString[4]);
            //putch('\r');
            for (int i = 0; i < 6; i++)
            {
                putch(receivedString[i]);
            }
            
            putch('\n');
            receivedString[currentIndex] = 'D';
            //change_color(31, receivedString[1], 0, 0); // 10 LEDs
            
            
            //putch('T');
            
            // Se procesan los comandos
            if (receivedString[0] == 'R')
            {
                red = receivedString[1];
                //change_color(alpha, red, green, blue); // 10 LEDs
            }
            if (receivedString[0] == 'G')
            {
                green = receivedString[1];
                //change_color(alpha, red, green, blue); // 10 LEDs
            }
            if (receivedString[0] == 'B')
            {
                blue = receivedString[1];
                //change_color(alpha, red, green, blue); // 10 LEDs
            }
            if (receivedString[0] == 'I')
            {
                alpha = receivedString[1];
                //change_color(alpha, red, green, blue); // 10 LEDs
            }
            if (receivedString[0] == 'V')
            {
                CCPR1L = (receivedString[1] * 166) / 100;
                //change_color(alpha, red, green, blue);
            }
            
            
                
            
            currentIndex = 0;
            //debug('5'); // DEBUG
        }
        */
        if (receivedString[currentIndex] == '\r')
        {
            uart_complete = 1;
            //putch('C');
            //putch('\n');
        }
        
        currentIndex++;
        
        // Verifica si la cadena está completa y no se recibió '\n'
        
        /*if (currentIndex == UART_STRING_TAM - 1 && receivedString[currentIndex - 1] != '\r')
        {
            currentIndex = 0; // Reinicia el índice si se produce un desbordamiento
        }*/
        
    }
}

void main(void)
{
    init(); // We call the initialize function in the init library 
    counter = 0;
    counterRuido10ms = 0;
    counterRuido1seg = 0;
    counter5seg = 0;
    CCPR1L = 0;
    //change_color(31, 255, 127, 0, 10);
    //change_color(31, 255, 0, 255);
    //debug('1'); // DEBUG
    //noiseCounterValues = 0;
    //printf("Bienvenido a Lampara inteligente!!\n"); //this will be send it by UART since putch was redefined.
    while (1); /* B-10 */
    return;
}
