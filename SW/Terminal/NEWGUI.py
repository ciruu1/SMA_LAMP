import serial
import customtkinter as ctk
from CTkColorPicker import *

# Default values
color = "#ffffff"
intensity = 255
speed = 100


def hex_to_rgba(color_hex):
    global red, green, blue, alpha
    # Eliminar el carácter '#' si está presente
    if color_hex.startswith('#'):
        color_hex = color_hex[1:]

    # Convertir el código hexadecimal a valores decimales
    color_dec = int(color_hex, 16)

    # Obtener los componentes RGB y A
    red = (color_dec >> 16) & 255
    green = (color_dec >> 8) & 255
    blue = color_dec & 255
    alpha = 255  # Establecer el valor alpha predeterminado a 255 (opaco)

    # Devolver los valores
    return red, green, blue, alpha


def button_update():
    # TODO Enviar aquí la config por la UART
    print(color)
    print(hex_to_rgba(color))
    print(intensity)
    print(speed)


def update_text():
    # TODO Actualizar los textos aquí cuando recibimos los valores por la UART
    hum_label.config(text="Humidity: ")
    temp_label.config(text="Temperature: ")
    co2_label.config(text="CO2: ")
    lux_label.config(text="Lux: ")
    noise_label.config(text="Noise: ")


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

intensity_slider = ctk.CTkSlider(app, from_=0, to=255, command=lambda e: update_intensity(e))
intensity_slider.grid(row=1, column=0, padx=0, pady=0)
intensity_slider.set(255)

intensity_label = ctk.CTkLabel(app, text="Intensity: 100%")
intensity_label.grid(row=2, column=0, padx=0, pady=0)

fan_slider = ctk.CTkSlider(app, from_=0, to=100, command=lambda e: update_fan(e))
fan_slider.grid(row=1, column=1, padx=10, pady=10)
fan_slider.set(100)

fan_label = ctk.CTkLabel(app, text="Fan speed: 100")
fan_label.grid(row=2, column=1, padx=0, pady=0)

buttonUpdate = ctk.CTkButton(app, text="Update", command=button_update)
buttonUpdate.grid(row=3, column=1, padx=20, pady=20)

group_label = ctk.CTkLabel(app, text="Sistemas Empotrados CIM41 GRUPO 5 2023 UPM", text_color="gray")
group_label.grid(row=3, column=0, padx=0, pady=0)

text = ctk.CTkFrame(app, width=200, height=240, border_width=2)
text.grid(row=0, column=1, padx=0, pady=10)
text.pack_propagate(False)

hum_label = ctk.CTkLabel(text, text="Humidity: x%", font=("Arial", 20)).pack(expand=True)
temp_label = ctk.CTkLabel(text, text="Temperature: x", font=("Arial", 20)).pack(expand=True)
co2_label = ctk.CTkLabel(text, text="CO2: X", font=("Arial", 20)).pack(expand=True)
lux_label = ctk.CTkLabel(text, text="Lux: X", font=("Arial", 20)).pack(expand=True)
noise_label = ctk.CTkLabel(text, text="Noise: X", font=("Arial", 20)).pack(expand=True)

app.mainloop()
