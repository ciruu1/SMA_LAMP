#include <xc.h>
#include <proc/pic16f886.h>

void I2C_Start() {
    SEN = 1;  // Iniciar condición de inicio
    while (SEN);  // Esperar hasta que la condición de inicio se complete
}

void I2C_Stop() {
    PEN = 1;  // Iniciar condición de parada
    while (PEN);  // Esperar hasta que la condición de parada se complete
}

void I2C_Write(unsigned char data) {
    SSPBUF = data;  // Cargar el byte de datos en el registro de búfer de envío
    while (BF);     // Esperar hasta que se complete la transmisión
}

void I2C_Ack() {
    ACKDT = 0;  // Establecer ACK para la próxima transmisión
    ACKEN = 1;  // Habilitar el bit ACK para enviar ACK
    while (ACKEN);  // Esperar hasta que se complete la transmisión del ACK
}

void I2C_Nack() {
    ACKDT = 1;  // Establecer NACK para la próxima transmisión
    ACKEN = 1;  // Habilitar el bit ACK para enviar NACK
    while (ACKEN);  // Esperar hasta que se complete la transmisión del NACK
}

unsigned char I2C_Read() {
    RCEN = 1;       // Habilitar la recepción
    while (!BF);    // Esperar hasta que se complete la recepción
    return SSPBUF;  // Devolver el byte leído
}
