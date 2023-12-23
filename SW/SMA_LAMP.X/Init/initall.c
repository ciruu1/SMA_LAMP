#include <xc.h>
#include <proc/pic16f886.h>

#define _XTAL_FREQ 20000000

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

    RCSTAbits.CREN = 1; // Habilita la recepción continua

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

void init_timer0()
{
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111; // Fosc/32
    INTCONbits.T0IE = 1; // Habilita las interrupciones del timer0
    INTCONbits.T0IF = 0;

    TMR0 = 159; // Cada 5 mseg   256-97  (5*10^6/ 256)*0.005 = 97.6 
}

void init_timer2(void) {
    T2CON = 0;
    T2CONbits.TOUTPS = 0b0000;
    T2CONbits.TMR2ON = 1;
    T2CONbits.T2CKPS = 0b00;
 
    PIE1bits.TMR2IE = 1;


}

void init_pwm(void) {
    CCP1CONbits.P1M = 0b00; // Salida única; P1A modulado; P1B, P1C, P1D asignados como pines de puerto
    CCP1CONbits.DC1B = 0b11; // Modo PWM
    CCP1CONbits.CCP1M = 0b1100; // Modo PWM; P1A, P1C activos en alto; P1B, P1D activos en alto
    CCPR1L = 0; // El trabajo comienza en 0 %
    PR2 = 167; // Para que el periodo del PWM sea 30Khz o 0.033 ms
    TRISC2 = 0; // Puerto C configurado como salida
}

void init_I2C()
{
    // Configuración del módulo I2C
    SSPCON = 0b00101000; // Habilitar el modo I2C Master, velocidad estándar (100kHz)
    SSPADD = ((_XTAL_FREQ / 4) / 100000) - 1; // Calcular el valor del registro de divisor para 100kHz
    SSPSTAT = 0;
    
    TRISC3 = 1;  // Configurar el pin RC3 como entrada para SDA
    TRISC4 = 1;  // Configurar el pin RC4 como entrada para SCL
}

void init_SPI() {
    //TRISC0 = 0; // SCK como salida (reloj)
    //TRISC1 = 0; // SDO como salida (Maestro manda esclavo)
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISAbits.TRISA5 = 1;
}

void init(void)
{
    OSCCON = 0b00001000;
    INTCONbits.GIE = 1;
    //INTCONbits.PEIE = 1;
    init_timer0();
    init_timer2();
    init_pwm();
    init_uart();
    init_adc();
    init_I2C();
    init_SPI();
}
