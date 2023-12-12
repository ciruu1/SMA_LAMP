#include <xc.h>
#include <proc/pic16f886.h>

/*
 Configure USART
 */

void init_uart(void)
{
    TXSTAbits.BRGH = 0;
    BAUDCTLbits.BRG16 = 0;

    // SPBRGH:SPBRG =
    SPBRGH = 0;
    SPBRG = 32; // 9600 baud rate with 20MHz Clock

    TXSTAbits.SYNC = 0; /* Asynchronous */
    TXSTAbits.TX9 = 0; /* TX 8 data bit */
    RCSTAbits.RX9 = 0; /* RX 8 data bit */

    PIE1bits.TXIE = 0; /* Disable TX interrupt */
    PIE1bits.RCIE = 1; /* Enable RX interrupt */

    RCSTAbits.SPEN = 1; /* Serial port enable */

    TXSTAbits.TXEN = 0; /* Reset transmitter */
    TXSTAbits.TXEN = 1; /* Enable transmitter */
    
    RCSTAbits.CREN = 1;         // Habilita la recepción continua

}

void init_adc(void)
{
    ADCON0bits.ADCS1 = 1;
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.CHS = 0b1100;
    ADCON0bits.GO = 0;
    ADCON0bits.ADON = 1;
    ADCON1bits.ADFM = 1;
    PIE1bits.ADIE = 1; // Activa la interrupcion en el ADC

    ADCON1bits.VCFG1 = 0;
    ADCON1bits.VCFG0 = 0;
    TRISBbits.TRISB0 = 1;
    ANSELHbits.ANS12 = 1;

}

void init_timer()
{
  OPTION_REGbits.T0CS = 0;
  OPTION_REGbits.PSA = 0;
  OPTION_REGbits.PS = 0b111; // Fosc/32
  INTCONbits.T0IE = 1; // Habilita las interrupciones del timer0
  INTCONbits.T0IF = 0;

  TMR0 = 159;  // Cada 5 mseg   256-97  (5*10^6/ 256)*0.005 = 97.6 
}

void init(void)
{
    OSCCON = 0b00001000;
    INTCONbits.GIE = 1;
    init_timer();
    init_uart();
    init_adc();
    
}
