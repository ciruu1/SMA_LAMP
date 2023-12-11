/*
 * In this file we describe the functions that monitor the sensors on the MCU. 
 * REF. MO-10, MO-20, MO-30, MO-40.
 * 
 * recibir_comandos_pc()
 * leer_sensores()
 */

#include <xc.h>
#include <stdio.h>

void recibir_comandos_pc() {
    if (PIR1bits.RCIF) { 
        char received_char = RCREG;  

        if (received_char == 'R' || received_char == 'G' || received_char == 'B' || received_char == 'L' || received_char == 'S') {
        }//this only check if some commands come by UART.
        
        PIR1bits.RCIF = 0;
    }
}
