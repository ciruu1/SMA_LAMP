#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Nov 29 13:16:28 2022

@author: norberto
"""

# =============================================================================
# Para actualizar comportamiento de los leds y el ventilador
# =============================================================================

import serial
#import struct

# Create serial instance

ser = serial.Serial(
port = "/dev/ttyUSB0", 
baudrate = 9600,
bytesize=serial.EIGHTBITS,
parity= serial.PARITY_NONE,
stopbits= serial.STOPBITS_ONE,
timeout=None,
xonxoff=False,
rtscts=False, 
write_timeout=None, 
dsrdtr=False, 
inter_byte_timeout=None,
exclusive=None)

Fin = False

while not Fin:
    
    salir = False
    while not salir:
        print()
        print("Ajustar leds       [1]")
        print("Ajustar ventilador [2]")
        print("Salir              [3]")
        
        resp = input()

        
        if (resp[0] != '1') and (resp[0] != '2') and (resp[0] != '3'):
            print("Respuesta incorrecta")
        else:
            salir = True
            
        
    
    if resp[0] == '1': #Ajustar leds
        R_cod = input("Rojo [0..255]: ")
        G_cod = input("Verde [0..255]: ")
        B_cod = input("Azul [0..255]: ")
        L_cod = input("Luminosidad [0..31]: ")
        
        bytes_writen = ser.write(('R').encode('utf-8'))
        bytes_writen = ser.write(int(R_cod).to_bytes(1, 'big'))
        
        
        bytes_writen = ser.write(('G').encode('utf-8'))
        bytes_writen = ser.write(int(G_cod).to_bytes(1, 'big'))
        
        
        bytes_writen = ser.write(('B').encode('utf-8'))
        bytes_writen = ser.write(int(B_cod).to_bytes(1, 'big'))
       
        
        bytes_writen = ser.write(('L').encode('utf-8'))
        bytes_writen = ser.write(int(L_cod).to_bytes(1, 'big'))
        
        ser.write('\r\n'.encode())
        
    elif resp[0] == '2':
        Vel = input("Velocidad del ventilador [0..100] (%) ")
        bytes_writen = ser.write(('S').encode('utf-8'))
        bytes_writen = ser.write(int(Vel).to_bytes(1, 'big'))
       
        
        ser.write('\r\n'.encode())
        
    else:
        Fin = True
        
    
    
    
