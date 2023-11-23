/*
 * File:   main.c
 * Author: ciruu
 *
 * Created on 23 de noviembre de 2023, 10:22
 */

// TODO cambiar los nombres del diagrama de alto nivel de Español a Inglés

#include <xc.h>
#include <proc/pic16f886.h>

// With this library we configure the main settings on the MCU
#include "Init/initall.h"

void main(void)
{
    init(); // We call the initialize function in the init library 
    return;
}
