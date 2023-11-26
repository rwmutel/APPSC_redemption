import serial
import serial.tools.list_ports
import wave
import numpy as np

# [print(i.device) for i in serial.tools.list_ports.comports()]
serial_port = serial.tools.list_ports.comports()[-1].device
print(serial_port)
baud_rate = 115200
# sample_rate = 30000.0
sample_rate = 48000000 / 256 / 19.5
ser = serial.Serial(serial_port, baud_rate)
buf_size = 8096
bytes_per_sample = 5

mic_in = bytes.decode(ser.read(buf_size * bytes_per_sample)).split(" ")[:-1]
mic_in = np.array(mic_in, dtype=np.uint16)
mic_in = mic_in.astype("<h")
print(mic_in.shape)

with open("tmp.npy", "wb") as f:
    np.save(f, mic_in)

sample_rate /= 3 # kostyl
mic_in -= 2**11
mic_in *= 2**4

with wave.open('tmp.wav','w') as obj:
    obj.setnchannels(2)
    obj.setsampwidth(2)
    obj.setframerate(sample_rate)
    obj.writeframes(mic_in)