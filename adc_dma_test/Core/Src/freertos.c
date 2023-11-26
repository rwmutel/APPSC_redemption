/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint16_t dma_buffer[100];
const uint16_t buf_start_pointer = 0;
const uint16_t buf_half_pointer = 500;
QueueHandle_t xQueue1;
/* USER CODE END Variables */
osThreadId samplingTaskHandle;
osThreadId inferencingTaskHandle;
osMessageQId inferenceTaskQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartSamplingTask(void const * argument);
void StartInferencingTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of inferenceTaskQueue */
  osMessageQDef(inferenceTaskQueue, 16, uint32_t);
  inferenceTaskQueueHandle = osMessageCreate(osMessageQ(inferenceTaskQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	xQueue1 = xQueueCreate(10,sizeof(char*));

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of samplingTask */
  osThreadDef(samplingTask, StartSamplingTask, osPriorityHigh, 0, 128);
  samplingTaskHandle = osThreadCreate(osThread(samplingTask), NULL);

  /* definition and creation of inferencingTask */
  osThreadDef(inferencingTask, StartInferencingTask, osPriorityIdle, 0, 128);
  inferencingTaskHandle = osThreadCreate(osThread(inferencingTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartSamplingTask */
/**
  * @brief  Function implementing the samplingTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartSamplingTask */
void StartSamplingTask(void const * argument)
{
  /* USER CODE BEGIN StartSamplingTask */
  HAL_ADC_Start_DMA(&hadc1, dma_buffer, 100);
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin);
    osDelay(1000);
  }
  /* USER CODE END StartSamplingTask */
}

/* USER CODE BEGIN Header_StartInferencingTask */
/**
* @brief Function implementing the inferencingTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartInferencingTask */
void StartInferencingTask(void const * argument)
{
  /* USER CODE BEGIN StartInferencingTask */
  /* Infinite loop */
	uint16_t buf_offset;
	for(;;)
	{
		xQueueReceive(xQueue1, &buf_offset, 100);
		osDelay(1);
	}
  /* USER CODE END StartInferencingTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
	xQueueSendFromISR(xQueue1, &buf_start_pointer, 100);
	HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	xQueueSendFromISR(xQueue1, &buf_half_pointer, 100);
	HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
}
/* USER CODE END Application */

