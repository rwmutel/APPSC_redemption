import numpy as np
import wave

# sample_rate = 48000000 / 256 / 19.5
sample_rate = 48000000 / 256 / 19.5
sample_rate /= 3
buf_size = 8096

mic_in = np.load("tmp.npy")
# mic_in -= 2**11
# mic_in *= 2**4
mic_in = np.concatenate([mic_in[::2], mic_in[1::2]])

with wave.open('tmp.wav','w') as obj:
    obj.setnchannels(1)
    obj.setsampwidth(2)
    obj.setframerate(sample_rate)
    obj.writeframes(mic_in)