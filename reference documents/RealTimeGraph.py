import serial
import matplotlib.pyplot as plt
from drawnow import drawnow

ser = serial.Serial('COM6', 9600)

data = []

def update_graph():
    plt.plot(data, label='UART Verisi')
    plt.xlabel('Zaman')
    plt.ylabel('Veri Değeri')
    plt.title('Real-Time Grafik')
    plt.legend()

while True:

    print(ser.readline().decode('utf-8'))
    uart_data = ser.readline().decode('utf-8').strip()

    try:
        data_point = uart_data
        #print(uart_data)
        data.append(data_point)
    except ValueError:
        print("Hatalı veri formatı:", uart_data)
        continue

    drawnow(update_graph)

    if len(data) > 20:
        data.pop(0)
