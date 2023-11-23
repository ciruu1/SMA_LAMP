/*
 * In this file we describe the functions that take control on the actuators
 * on the system. REF. AC-10, AC-20.
 * 
 * enviar_info_pc()
 * modificar_velocidad()
 * modificar_LEDs()
 */

#include <xc.h>
#include <proc/pic16f886.h>

void sendchar(char data)
{
    while (!TRMT) // check buffer
        continue; // wait till ready
    TXREG = data; // send data
}

// enviar_info_pc()
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