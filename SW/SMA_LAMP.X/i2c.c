#include <xc.h>
#include <proc/pic16f886.h>

void I2C_Start() {
    SEN = 1;  // Iniciar condici�n de inicio
    while (SEN);  // Esperar hasta que la condici�n de inicio se complete
}

void I2C_Stop() {
    PEN = 1;  // Iniciar condici�n de parada
    while (PEN);  // Esperar hasta que la condici�n de parada se complete
}

void I2C_Write(unsigned char data) {
    SSPBUF = data;  // Cargar el byte de datos en el registro de b�fer de env�o
    while (BF);     // Esperar hasta que se complete la transmisi�n
}

void I2C_Ack() {
    ACKDT = 0;  // Establecer ACK para la pr�xima transmisi�n
    ACKEN = 1;  // Habilitar el bit ACK para enviar ACK
    while (ACKEN);  // Esperar hasta que se complete la transmisi�n del ACK
}

void I2C_Nack() {
    ACKDT = 1;  // Establecer NACK para la pr�xima transmisi�n
    ACKEN = 1;  // Habilitar el bit ACK para enviar NACK
    while (ACKEN);  // Esperar hasta que se complete la transmisi�n del NACK
}

unsigned char I2C_Read() {
    RCEN = 1;       // Habilitar la recepci�n
    while (!BF);    // Esperar hasta que se complete la recepci�n
    return SSPBUF;  // Devolver el byte le�do
}
