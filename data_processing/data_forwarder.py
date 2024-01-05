import json
import time, hmac, hashlib
import requests
import numpy as np
from tqdm import tqdm

empty_sign = ''.join(['0'] * 64)
HMAC_KEY = "d6480aa74ac5563c3367981bf8369b1d"
API_KEY = "ei_05310a869f70a6ce26505422ae4af9ef3374fc949358d391cdbe7ea6611ef0cc"

samples = list(range(125))

def format_data(array):
    array = array[:array.shape[0] - array.shape[0] % 2]
    data = {
        "protected": { "ver": "v1", "alg": "HS256", "iat": time.time() },
        "signature": empty_sign,
        "payload": {
            "device_type": "STM32F3Discovery",
            "interval_ms": 1000/4573,
            "sensors": [
                { "name": "left_mic", "units": "ms" },
                { "name": "right_mic", "units": "ms" }
            ],
            "values": np.reshape(array, (-1, 2)).tolist()
        }
    }
    encoded = json.dumps(data)
    # sign message
    signature = hmac.new(bytes(HMAC_KEY, 'utf-8'), msg = encoded.encode('utf-8'), digestmod = hashlib.sha256).hexdigest()
    # set the signature again in the message, and encode again
    data['signature'] = signature
    encoded = json.dumps(data)
    return encoded


if __name__ == "__main__":
    for sample in tqdm(samples):
        try:
            fname = f"sample_{str(sample).zfill(3)}.npy"
            mic_in = np.load(f"4500_data/cropped/r-hit/{fname}")
        except:
            print(f"No sample {fname}")
            continue

        encoded = format_data(mic_in)
        res = requests.post(url='https://ingestion.edgeimpulse.com/api/training/data',
                        data=encoded,
                        headers={
                            'Content-Type': 'application/json',
                            'x-file-name': f"sample_{str(sample).zfill(3)}",
                            'x-api-key': API_KEY,
                            'x-label': "r-hit"
                        })
        if (res.status_code != 200):
            print('Failed to upload file to Edge Impulse', res.status_code, res.content)