
#define _XTAL_FREQ 20000000 // necessary for __delay_us
#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF
#pragma intrinsic(_delay)

#include <xc.h>
#include <stdio.h>
#include <proc/pic16f886.h>



void init_timer2(){
    
    T2CONbits.TMR2ON=1;
    T2CONbits.TOUTPS = 0;
    T2CONbits.T2CKPS = 0;
    PIE1bits.TMR2IE = 0; 
    PIR1bits.TMR2IF = 0;

    
    PR2 = 166;   
}


void initPWM(void){

    CCP1CONbits.P1M=0b00;
    CCP1CONbits.DC1B=0b0;
    CCP1CONbits.CCP1M=0b1100;
    CCPR1L=0; 
    CCP2CONbits.DC2B0 = 0b1;
    CCP2CONbits.DC2B1 = 0b1;
    CCP2CONbits.CCP2M = 0b1100;
   
}


void cambioPotencia(int potencia){
    
    int estado = (potencia/100)*166;
    CCPR1L = estado;
}

void main(void) {
  
  INTCONbits.GIE = 1;
  INTCONbits.PEIE = 1;
  OSCCON = 0b00001000; // External cristal
  TRISC = 0; 
  

  
  init_timer2();
  initPWM();

  cambioPotencia(100);
  __delay_us(1000);
  //cambioPotencia(20);
  //__delay_us(10000);
  cambioPotencia(0);
    while(1);
}
