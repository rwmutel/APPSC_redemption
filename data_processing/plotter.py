import matplotlib.pyplot as plt
import numpy as np


mic_in = np.load("demo_data/raw/demo_r/sample_001.npy")
# mic_in -= 2**11
# mic_in *= 2**4

# plt.plot(mic_in)

plt.plot(mic_in[::2], label="mic_1")
plt.plot(mic_in[1::2], label="mic_2")
plt.legend()
plt.show()