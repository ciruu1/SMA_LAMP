import re
import serial
import customtkinter as ctk
from CTkColorPicker import *
from serial.serialutil import SerialException
from time import sleep

# Default values
color = "#ffffff"
intensity = 255
speed = 0
red = 0xFF
green = 0xFF
blue = 0xFF
'''
port_enabled = True


ser = serial.Serial()
port_enabled = False


def config_serial():
    global ser
    ser = serial.Serial(
        port="/dev/ttyUSB0",
        baudrate=9600,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1,
        xonxoff=False,
        rtscts=False,
        write_timeout=None,
        dsrdtr=False,
        inter_byte_timeout=None,
        exclusive=None
    )


def retry():
    global ser, port_enabled
    try:
        config_serial()
        port_enabled = True
    except SerialException:
        pass
    finally:
        if port_enabled:
            root.quit()


root = ctk.CTk()
root.geometry("210x100")
root.title("SMA_COMP")
root.resizable(False, False)
ctk.CTkLabel(root, text="SMA_COMP\nNOT AVAILABLE", font=("Arial", 25), text_color='yellow').pack(padx=5, pady=5)
ctk.CTkButton(root, text="Retry", command=retry).pack(padx=5, pady=5)
root.mainloop()
'''

ser = serial.Serial(
        port="/dev/ttyUSB0",
        baudrate=9600,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1,
        xonxoff=False,
        rtscts=False,
        write_timeout=None,
        dsrdtr=False,
        inter_byte_timeout=None,
        exclusive=None
    )


def hex_to_rgba(color_hex):
    global red, green, blue
    # Eliminar el carácter '#' si está presente
    if color_hex.startswith('#'):
        color_hex = color_hex[1:]

    # Convertir el código hexadecimal a valores decimales
    color_dec = int(color_hex, 16)

    # Obtener los componentes RGB y A
    red = (color_dec >> 16) & 255
    green = (color_dec >> 8) & 255
    blue = color_dec & 255

    # Devolver los valores
    return red, green, blue


def button_update():
    # TODO Enviar aquí la config por la UART
    #print(color)
    print(hex_to_rgba(color))
    print(f"{red} {green} {blue}")
    #print(intensity)
    t = 0.01
    # LEDs
    # "LRGBI" where 'RGB' is the color and 'I' the intensity of light
    ser.write('R'.encode('utf-8'))
    sleep(t)
    ser.write(red.to_bytes(1))
    sleep(t)
    #ser.write('\r\n'.encode())
    #ser.write('G'.encode('utf-8'))
    ser.write(green.to_bytes(1, 'big'))
    sleep(t)
    #ser.write('\r\n'.encode())
    #ser.write('B'.encode('utf-8'))
    ser.write(blue.to_bytes(1, 'big'))
    sleep(t)
    #ser.write('\r\n'.encode())
    #ser.write('A'.encode('utf-8'))
    ser.write(intensity.to_bytes(1, 'big'))
    sleep(t)
    #ser.write('\r\n'.encode())

    #print(speed)
    # FAN
    # "VS" where 'S' is between 0 and 100
    #ser.write('V'.encode('utf-8'))
    ser.write(speed.to_bytes(1, 'big'))
    sleep(t)
    ser.write('\r'.encode())
    #ser.write('\r\n'.encode())

def process(line):
    for i in range(len(line)):
        if line[i] != '\n':
            if line[i] == 'P' and line[i+1] == 'P' and line[i+2] == ' ':
                print("+++++++++++++++++++")
                print("DECODED: ")
                print(line[i::-1])
                print("+++++++++++++++++++")

def update_text():
    try:
        if ser.in_waiting > 0:
            #process(ser.readline())
            end_line = b'\r' + b'\n'
            #received_data = ser.read_until(expected=b'\n')
            received_data = ser.readline().decode('utf-8')
            print(f"RAW: {received_data}")
            #print(f"BYTES: {bytes(received_data, 'utf-8')}")
            print("--------------------------------------------")
            match = re.match(r'([A-Z]+) (\d+)', received_data)
            '''
            valor_x = cadena.split(" ")[1].strip()
            print(f"RAW: {valor_x}")
            print(f"BYTES: {bytes(valor_x, 'utf-8')}")
            tipo = cadena.split(" ")[0].strip()
            if tipo == "HU":
                hum_label.configure(text=f"Humidity: {((((float(valor_x) / 1024.0) * 5.0) - 0.826) / 0.0315):.1f}%")
            elif tipo == "TE":
                temp_label.configure(text=f"Temperature: {(((float(valor_x) / 1024.0) * 5.0) / 0.01):.1f}ºC")
            elif tipo == "PP":
                if valor_x.startswith("WAR"):
                    co2_label.configure(text=f"CO2: NO DISPONIBLE", text_color='yellow')
                elif valor_x.startswith("BUS"):
                    co2_label.configure(text=f"CO2: BUSY", text_color='red')
                elif valor_x.startswith("ERR"):
                    co2_label.configure(text=f"CO2: ERROR", text_color='red')
                elif valor_x.startswith("UNK"):
                    co2_label.configure(text=f"CO2: UNKOWN", text_color='red')
                else:
                    co2_label.configure(text=f"CO2: {valor_x} ppm")
            elif tipo == "LU":
                lux_label.configure(text=f"Lux: {valor_x} lx")
            elif tipo == "NO":
                if 0 <= int(valor_x) <= 400:
                    noise_label.configure(text=f"Ruido bajo", text_color='green')
                elif 400 < int(valor_x) <= 900:
                    noise_label.configure(text=f"Ruido intermedio", text_color='yellow')
                elif int(valor_x) > 900:
                    noise_label.configure(text=f"Ruido alto", text_color='red')
                else:
                    pass
            else:  # Maybe here load config on the GUI
                pass
            '''

            if match:
                # Si encontramos un tipo y un número, los imprimimos
                tipo = match.group(1)
                valor_x = match.group(2)
                if tipo == "HU":
                    hum_label.configure(text=f"Humidity: {abs(((((float(valor_x) / 1024.0) * 5.0) - 0.826) / 0.0315)):.1f}%")
                elif tipo == "TE":
                    far = (((float(valor_x) / 1024.0) * 5.0) / 0.01)
                    temp_label.configure(text=f"Temperature: {((far - 32) * 5/9)-10:.1f}ºC")
                elif tipo == "PP":
                    if tipo.__contains__("WAR"):
                        co2_label.configure(text=f"CO2: NO DISPONIBLE", text_color='yellow')
                    elif valor_x.startswith("BUS"):
                        co2_label.configure(text=f"CO2: BUSY", text_color='red')
                    elif valor_x.startswith("ERR"):
                        co2_label.configure(text=f"CO2: ERROR", text_color='red')
                    elif valor_x.startswith("UNK"):
                        co2_label.configure(text=f"CO2: UNKOWN", text_color='red')
                    else:
                        co2_label.configure(text=f"CO2: {valor_x} ppm")
                elif tipo == "LU":
                    lux_label.configure(text=f"Lux: {valor_x} lx")
                elif tipo == "NO":
                    if 0 <= int(valor_x) <= 400:
                        noise_label.configure(text=f"Ruido bajo", text_color='green')
                    elif 400 < int(valor_x) <= 900:
                        noise_label.configure(text=f"Ruido intermedio", text_color='yellow')
                    elif int(valor_x) > 900:
                        noise_label.configure(text=f"Ruido alto", text_color='red')
                    else:
                        pass
                else:  # Maybe here load config on the GUI
                    pass
            else:
                print(f"Formato no válido {received_data}")
                

    finally:
        # Schedule the next read after 100 ms
        text.after(150, update_text)


def update_color(col):
    global color
    color = col


def update_intensity(inten):
    global intensity
    intensity = int(inten)
    intensity_label.configure(text=str(f"Intensity: {int((intensity * 100) / 255)}%"))


def update_fan(spd):
    global speed
    speed = int(spd)
    fan_label.configure(text=str(f"Fan speed: {int(speed)}"))




app = ctk.CTk()
app.geometry("535x380")
app.title("SMA_COMP")
app.resizable(False, False)

colorpicker = CTkColorPicker(app, width=300, command=lambda e: update_color(e))
colorpicker.grid(row=0, column=0, padx=10, pady=10)

intensity_slider = ctk.CTkSlider(app, from_=0, to=31, command=lambda e: update_intensity(e))
intensity_slider.grid(row=1, column=0, padx=0, pady=0)
intensity_slider.set(31)

intensity_label = ctk.CTkLabel(app, text="Intensity: 31")
intensity_label.grid(row=2, column=0, padx=0, pady=0)

fan_slider = ctk.CTkSlider(app, from_=0, to=100, command=lambda e: update_fan(e))
fan_slider.grid(row=1, column=1, padx=10, pady=10)
fan_slider.set(0)

fan_label = ctk.CTkLabel(app, text="Fan speed: 0")
fan_label.grid(row=2, column=1, padx=0, pady=0)

buttonUpdate = ctk.CTkButton(app, text="Update", command=button_update)
buttonUpdate.grid(row=3, column=1, padx=20, pady=20)

group_label = ctk.CTkLabel(app, text="Sistemas Empotrados CIM41 GRUPO 5 2023 UPM", text_color="gray")
group_label.grid(row=3, column=0, padx=0, pady=0)

text = ctk.CTkFrame(app, width=200, height=240, border_width=2)
text.grid(row=0, column=1, padx=0, pady=10)
text.pack_propagate(False)


hum_label = ctk.CTkLabel(text, text="Humidity: x%", font=("Arial", 20))
hum_label.pack(expand=True)
temp_label = ctk.CTkLabel(text, text="Temperature: x", font=("Arial", 20))
temp_label.pack(expand=True)
co2_label = ctk.CTkLabel(text, text="CO2: X", font=("Arial", 20))
co2_label.pack(expand=True)
lux_label = ctk.CTkLabel(text, text="Lux: X", font=("Arial", 20))
lux_label.pack(expand=True)
noise_label = ctk.CTkLabel(text, text="Noise: X", font=("Arial", 20))
noise_label.pack(expand=True)

text.after(100, update_text())

app.mainloop()



