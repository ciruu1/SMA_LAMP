# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

# =============================================================================
# Py serial basic test
# =============================================================================

import serial

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


end_line = b'\r' + b'\n'
    
while True:
    
    line = ser.read_until(expected=end_line)
    print(line)
    


