import matplotlib.pyplot as plt
import numpy as np
import sys

# mic_in -= 2**11
# mic_in *= 2**4

# plt.plot(mic_in)

fig, axs = plt.subplots(4,4, figsize=(10,10))
plt.tight_layout(pad=1.2)

start = int(sys.argv[1])

for i in range(4):
    for j in range(4):
        try:
            # mic_in = np.load(f"4500_data/raw/hit/sample_{str(start + i*4+j).zfill(3)}.npy")
            mic_in = np.load(f"4500_data/cropped/silence/sample_{str(start + i*4+j).zfill(3)}.npy")[10:]
            peak = np.argmax(mic_in[0::2])
            axs[i, j].set_title(f"sample_{str(start + i*4+j).zfill(3)}")
            axs[i, j].plot(mic_in[0::2], label="mic_1")
            axs[i, j].plot(mic_in[1::2], label="mic_2")
            # axs[i, j].axvline(peak, c="g
            # axs[i, j].axvline(peak+512, c="g")
            print(i, j)
        except:
            print(f"No sample_{str(start + i*4+j).zfill(3)}.npy")
        
plt.legend()
plt.show()

# mic_in = np.load(f"demo_data/raw/demo_hit/sample_042.npy")
# print(mic_in[mic_in>= 4000])