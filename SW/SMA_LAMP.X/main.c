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

// With this library we configure the main settings on the MCU
#include "Init/initall.c"
//#include "action.c"

int counter;
int counterRuido10ms, counterRuido1seg, counter5seg; // 2 ticks, 200 ticks, 1000 ticks
int uart_num;
int NoiseValue, HumValue, TempValue;
char noiseValues[10];
int noiseCounterValues; // 0..9
int maxNoise = 0;

//USART
char receivedString[20];
int currentIndex = 0;
int endString = 0;

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
        if (counterRuido10ms >= 2) // NOISE
        {
            counterRuido10ms = 0;
            NoiseValue = readADC(8); // AN8
            if (noiseCounterValues >= 10)
            {
                noiseCounterValues = 0;
            }
            noiseValues[noiseCounterValues] = NoiseValue;
            noiseCounterValues++;
            // Leer ruido 10 ms
        }
        if (counterRuido1seg >= 200)
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
            printf("NOISE %u", maxNoise);
        }
        if (counter5seg >= 1000)
        {
            counter5seg = 0;
            HumValue = readADC(12);
            TempValue = readADC(10);
            printf("HUMIDITY %u", HumValue);
            printf("TEMPERATURE %u", TempValue);
            // I2C

            // Leer Sensores
        }
    }
    //UART
    if (PIR1bits.RCIF)
    {
        receivedString[currentIndex] = RCREG; // Lee el carácter recibido
        currentIndex++;
        if (receivedString[currentIndex] == '\n')
        {
            // Se procesan los comandos
            for (int i = 0; i < currentIndex; i++)
            {
                if (receivedString[i] == "L") {
                    // LEDS
                }
                else if (receivedString[i] == "V") {
                    // Ventilador
                }
                else {
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
    printf("Bienvenido a Lampara inteligente!!"); //this will be send it by UART since putch was redefined.
    while (1)
    {
        
    }
    return;
}
