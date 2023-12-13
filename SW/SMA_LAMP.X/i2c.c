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

unsigned char I2C_Read() {
    RCEN = 1;       // Habilitar la recepción
    while (!BF);    // Esperar hasta que se complete la recepción
    return SSPBUF;  // Devolver el byte leído
}
