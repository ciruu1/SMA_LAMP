/*
 * File:   pruebas_led.c
 * Author: ciruu
 *
 * Created on 20 de diciembre de 2023, 11:53
 */



#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF

#include <xc.h>
#include <proc/pic16f886.h>
#include <stdio.h>
#include "SPI/spi-master-v1.c"

#pragma intrinsic(_delay)
#define _XTAL_FREQ 20000000 // necessary for __delay_us

char salida_B = 0;
int contador = 0;
char continuar = 0;

char red = 0;
char green = 0;
char blue = 0;

char brillo = 30;



void actualizar_leds(uint8_t brillo, uint8_t r, uint8_t g, uint8_t b)
{
    spi_write_read(0);
    spi_write_read(0);
    spi_write_read(0);
    spi_write_read(0);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);
    

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);

    spi_write_read((char)(brillo + 224));           //224 = 1110 0000
    spi_write_read((char)b);
    spi_write_read((char)g);
    spi_write_read((char)r);


    spi_write_read(0xFF);
    spi_write_read(0xFF);
    spi_write_read(0xFF);
    spi_write_read(0xFF);

}

void __interrupt() treat_int(void)
{
    
    if(INTCONbits.T0IF)
    {
        TMR0 = 60;
        contador += 1;
        
        if(contador >= 400)
        {
            actualizar_leds(brillo, red, green, blue);
            continuar = 1;
            contador = 0;
        }
        
        INTCONbits.T0IF = 0; 
    }
}

void init_TMR0(void)
{
    OPTION_REGbits.T0CS= 0b0;
    OPTION_REGbits.PSA = 0b0;
    OPTION_REGbits.PS = 0b110;
    TMR0 = 60;
    INTCONbits.T0IE = 1; 
}

void init_uart(void)
{  
  TXSTAbits.BRGH = 0;
  BAUDCTLbits.BRG16 = 0;

  // SPBRGH:SPBRG = 
  SPBRGH = 0;
  SPBRG = 32;  // 9600 baud rate with 20MHz Clock
  
  TXSTAbits.SYNC = 0; /* Asynchronous */
  TXSTAbits.TX9 = 0; /* TX 8 data bit */
  RCSTAbits.RX9 = 0; /* RX 8 data bit */

  PIE1bits.TXIE = 0; /* Disable TX interrupt */
  PIE1bits.RCIE = 0; /* Disable RX interrupt */

  RCSTAbits.SPEN = 1; /* Serial port enable */

  TXSTAbits.TXEN = 0; /* Reset transmitter */
  TXSTAbits.TXEN = 1; /* Enable transmitter */
 }




void init_Ports(void)
{
    //TRISBbits.TRISB1 = 1;           //Configurar el pin 1 del puerto B como Input
    //ANSELbits.ANS2 = 1;             //Configurar el pin 1 del puerto B como Input Analogico
    //TRISB = 0;                      //Configurar el puerto B como Output
    
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISAbits.TRISA5 = 1;
}

void putch(char c)
{ 
  while (!TXSTAbits.TRMT);
  TXREG = c;
} 



void main(void)
{ 
  OSCCONbits.OSTS = 1; // External cristal
  

  //falta configurar los puertos correspondientes como salidas

  init_uart();
  
  init_TMR0();
  init_Ports();
  
  INTCONbits.PEIE = 1;
  INTCONbits.GIE = 1;
  
  continuar = 0;
  
  
  
  while(1)
  {
    while(!continuar);
    
    if(red >= 240)
    {
        red = 0;
    }else red += 20;
    
    
    //red == 255 ? red=0 : red=255; 
    //green == 255 ? green=0 : green++;
    //blue == 255 ? blue=0 : blue++;

    
    continuar = 0;
    //printf("%d\r\n", red);
    //printf("%d\r\n", green);
    //printf("%d\r\n", blue);
  } 
  
  //actualizar_leds(brillo, red, green, blue);
  
}