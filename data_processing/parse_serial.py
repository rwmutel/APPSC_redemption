import serial
import serial.tools.list_ports
import wave
import numpy as np
import matplotlib.pyplot as plt

# [print(i.device) for i in serial.tools.list_ports.comports()]
serial_port = serial.tools.list_ports.comports()[-1].device
print(serial_port)
baud_rate = 115200
cpu_freq = 72e6
adc_prescaler = 256
sample_time = 61.5
sample_rate = cpu_freq / adc_prescaler / sample_time

ser = serial.Serial(serial_port, baud_rate)
buf_size = 4094
bytes_per_sample = 5

sample_class = "hit"

i = 0
while True:
    print(f"Started Recording Sample #{i}")
    mic_in = bytes.decode(ser.read(buf_size * bytes_per_sample)).split(" ")[:-1]
    mic_in = np.array(mic_in, dtype=np.uint16)
    mic_in = mic_in.astype("<h")
    print(mic_in.shape)

    with open(f"4500_data/raw/{sample_class}/sample_{str(i).zfill(3)}.npy", "wb") as f:
        np.save(f, mic_in)

    with wave.open(f'4500_data/wav/{sample_class}/sample_{str(i).zfill(3)}.wav','w') as obj:
        obj.setnchannels(1)
        obj.setsampwidth(2)
        obj.setframerate(sample_rate)
        obj.writeframes(mic_in)

    # print(f"Plotting {sample_class} Sample #{i}")
    # plt.plot(mic_in[::2], label="mic_1")
    # plt.plot(mic_in[1::2], label="mic_2")
    # plt.legend()
    # plt.show()
    
    print(f"Done {sample_class} Sample #{i}")
    i += 1