import serial
import tkinter as tk
from tkinter import ttk, simpledialog

class SerialInterfaceApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Serial Interface")


        # Create serial instance
        self.ser = serial.Serial(
            port="COM6",  
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

        # GUI elements
        self.create_widgets()

    def create_widgets(self):

        # Entry widgets for command input
        self.leds_frame = ttk.Frame(self.root, padding="10")
        self.leds_frame.grid(row=0, column=0, padx=10, pady=10)
        self.leds_R_label = ttk.Label(self.leds_frame, text="Rojo:")
        self.leds_R_label.grid(row=0, column=0, pady=5, sticky=tk.E)
        self.leds_R_entry = ttk.Entry(self.leds_frame, width=10)
        self.leds_R_entry.grid(row=0, column=1, pady=5)
        self.leds_G_label = ttk.Label(self.leds_frame, text="Verde:")
        self.leds_G_label.grid(row=0, column=2, pady=5, sticky=tk.E)
        self.leds_G_entry = ttk.Entry(self.leds_frame, width=10)
        self.leds_G_entry.grid(row=0, column=3, pady=5)
        self.leds_B_label = ttk.Label(self.leds_frame, text="Azul:")
        self.leds_B_label.grid(row=0, column=4, pady=5, sticky=tk.E)
        self.leds_B_entry = ttk.Entry(self.leds_frame, width=10)
        self.leds_B_entry.grid(row=0, column=5, pady=5)
        self.leds_L_label = ttk.Label(self.leds_frame, text="Luminosidad:")
        self.leds_L_label.grid(row=0, column=6, pady=5, sticky=tk.E)
        self.leds_L_entry = ttk.Entry(self.leds_frame, width=10)
        self.leds_L_entry.grid(row=0, column=7, pady=5)
        self.send_leds_button = ttk.Button(self.leds_frame, text="Ajustar LEDs", command=self.send_leds_command)
        self.send_leds_button.grid(row=0, column=8, pady=5, padx=10)
        self.ventilador_frame = ttk.Frame(self.root, padding="10")
        self.ventilador_frame.grid(row=1, column=0, padx=10, pady=10)
        self.ventilador_Vel_label = ttk.Label(self.ventilador_frame, text="Velocidad del Ventilador:")
        self.ventilador_Vel_label.grid(row=0, column=0, pady=5, sticky=tk.E)
        self.ventilador_Vel_entry = ttk.Entry(self.ventilador_frame, width=10)
        self.ventilador_Vel_entry.grid(row=0, column=1, pady=5)
        self.send_ventilador_button = ttk.Button(self.ventilador_frame, text="Ajustar Ventilador", command=self.send_ventilador_command)
        self.send_ventilador_button.grid(row=0, column=2, pady=5, padx=10)
        self.exit_button = ttk.Button(self.root, text="Salir", command=self.finish_program)
        self.exit_button.grid(row=2, column=0, pady=10)
        self.receive_text = tk.Text(self.root, height=6, width=40, wrap=tk.WORD)
        self.receive_text.grid(row=0, column=1, rowspan=3, padx=10, pady=10)


    def send_leds_command(self):
        R_cod = self.leds_R_entry.get()
        G_cod = self.leds_G_entry.get()
        B_cod = self.leds_B_entry.get()
        L_cod = self.leds_L_entry.get()

        self.ser.write(('R').encode('utf-8'))
        self.ser.write(int(R_cod).to_bytes(1, 'big'))

        self.ser.write(('G').encode('utf-8'))
        self.ser.write(int(G_cod).to_bytes(1, 'big'))

        self.ser.write(('B').encode('utf-8'))
        self.ser.write(int(B_cod).to_bytes(1, 'big'))

        self.ser.write(('L').encode('utf-8'))
        self.ser.write(int(L_cod).to_bytes(1, 'big'))

        self.ser.write('\r\n'.encode())

        self.root.after(100, self.receive_serial_data)

    def send_ventilador_command(self):
        Vel = self.ventilador_Vel_entry.get()

        self.ser.write(('S').encode('utf-8'))
        self.ser.write(int(Vel).to_bytes(1, 'big'))
        self.ser.write('\r\n'.encode())
        self.root.after(100, self.receive_serial_data)

    def finish_program(self):
        self.root.destroy()

    def receive_serial_data(self):
        try:
            if self.ser.in_waiting > 0:
                received_data = self.ser.readline().decode()
                self.receive_text.insert(tk.END, received_data)
        finally:
            # Schedule the next read after 100 ms
            self.root.after(100, self.receive_serial_data)

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialInterfaceApp(root)
    root.mainloop()
