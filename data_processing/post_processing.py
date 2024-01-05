import numpy as np
import random


samples = list(range(125))

for sample in samples:
    try:
        fname = f"sample_{str(sample).zfill(3)}.npy"
        mic_in = np.load(f"4500_data/raw/hit/{fname}")
        peak = np.argmax(mic_in[0::2])
        print(f"{sample=}, {peak=}")
        a = random.randint(16, 48)
        b = random.randint(16, 48)
        with open(f"4500_data/cropped/l-hit/{fname}", "wb") as f:
            np.save(f, np.reshape(np.reshape(mic_in, (-1, 2))[peak - a:peak + 512 + b, :], (-1)))
        with open(f"4500_data/cropped/r-hit/{fname}", "wb") as f:
            np.save(f, np.reshape(np.reshape(mic_in, (-1, 2))[peak - a:peak + 512 + b, :][:, ::-1], (-1)))
        with open(f"4500_data/cropped/silence/{fname}", "wb") as f:
            if peak + 1024 < mic_in.shape[0] // 2:
                np.save(f, np.reshape(np.reshape(mic_in, (-1, 2))[peak + 512:peak + 1024, :], (-1)))
            elif peak - 512 > 0:
                np.save(f, np.reshape(np.reshape(mic_in, (-1, 2))[peak + 512:peak + 1024, :], (-1)))
            else:
                print("No silence possible to crop")

    except Exception as e:
        print(e)
        print(f"No sample {sample}")

# mic_in = np.load(f"demo_data/raw/demo_hit/sample_092.npy")
# print(np.min(mic_in))