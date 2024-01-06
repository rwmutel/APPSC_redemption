extern "C" {
    #include "classifier.h"
}
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#include "edge-impulse-sdk/dsp/spectral/spectral.hpp"
#include "gpio.h"

uint32_t buf_start_offset = 0;
signal_t audio_data;
ei_impulse_result_t result;


int audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float((int16_t*) dma_buffer + buf_start_offset + offset, out_ptr, length);
    return 0;
}

void classifier_init(){
    run_classifier_init();
}

uint32_t classify_slice(uint32_t offset){
    buf_start_offset = offset;
    audio_data.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    audio_data.get_data = &audio_signal_get_data;
    result = { nullptr };
    auto err = run_classifier_continuous(&audio_data, &result, true);
    HAL_GPIO_TogglePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin);

    if (err != EI_IMPULSE_OK) {
        return 2;
    }

    uint32_t max_index = 2;
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if (result.classification[ix].value > result.classification[max_index].value) {
            max_index = ix;
        }
    }
    if (result.classification[max_index].value < 0.85) {
        return 2;
    }

    return max_index;
}

