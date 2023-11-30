/*
 * classifier.h
 *
 *  Created on: Nov 27, 2023
 *      Author: vepbxer
 */

#ifndef INC_CLASSIFIER_H_
#define INC_CLASSIFIER_H_
#include "model-parameters/model_metadata.h"

extern uint16_t dma_buffer[EI_CLASSIFIER_SLICE_SIZE*2];
void classifier_init();
uint32_t classify_slice(uint32_t offset);

#endif /* INC_CLASSIFIER_H_ */
