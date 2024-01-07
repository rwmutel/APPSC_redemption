extern "C" {
    #include "classifier.h"
}
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#include "edge-impulse-sdk/dsp/spectral/spectral.hpp"
#include "gpio.h"
#include "usart.h"

volatile uint32_t buf_start_offset = 0;
signal_t audio_data;
ei_impulse_result_t result;


int audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float( dma_buffer + buf_start_offset + offset, out_ptr, length);
    return 0;
}

void classifier_init(){
    run_classifier_init();
}

uint32_t classify_slice(volatile uint32_t offset){
    buf_start_offset = offset;
    audio_data.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    audio_data.get_data = &audio_signal_get_data;
    result = { nullptr };
    HAL_GPIO_TogglePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin);
    auto err = run_classifier_continuous(&audio_data, &result, false, true);
    HAL_GPIO_TogglePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin);

    if (err != EI_IMPULSE_OK) {
        return 2;
    }

//    char buf[256];
//    sprintf(buf, "%s: %f, %s: %f, %s: %f\n", result.classification[0].label, result.classification[0].value, result.classification[1].label, result.classification[1].value, result.classification[2].label, result.classification[2].value);
//    HAL_UART_Transmit(&huart1, (uint8_t*) buf, strlen(buf), 1000);
//    for (size_t i = 0; i < 10; ++i) {
//        sprintf(buf, "%d, %d, %d, %d, %d, %d, %d, %d\n", dma_buffer[buf_start_offset+i], dma_buffer[buf_start_offset+i+1], dma_buffer[buf_start_offset+i+2], dma_buffer[buf_start_offset+i+3], dma_buffer[buf_start_offset+i+4], dma_buffer[buf_start_offset+i+5], dma_buffer[buf_start_offset+i+6], dma_buffer[buf_start_offset+i+7]);
//        HAL_UART_Transmit(&huart1, (uint8_t*) buf, strlen(buf), 1000);
//    }

    uint32_t max_index = 2;
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if (result.classification[ix].value > result.classification[max_index].value) {
            max_index = ix;
        }
    }
    if (result.classification[max_index].value < 0.75) {
        return 2;
    }

    return max_index;
}

