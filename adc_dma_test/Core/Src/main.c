/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd5110.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum state_t {
    START, R_SERVE, L_SERVE, R_WAIT, L_WAIT, R_TURN, L_TURN, UNSURE, END
} state_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile state_t state = START;
volatile state_t server;
volatile uint8_t l_score = 0;
volatile uint8_t r_score = 0;
volatile bool listening = true;
volatile bool deuce = false;
LCD5110_display lcd1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void print_score(LCD5110_display *lcd_conf) {
    LCD5110_clear_scr(lcd_conf);
    LCD5110_set_cursor(1, 22, lcd_conf);
    LCD5110_printf(lcd_conf, BLACK, "L:%u", l_score);
    LCD5110_set_cursor(60, 22, lcd_conf);
    LCD5110_printf(lcd_conf, BLACK, "R:%u", r_score);
    if (deuce) {
        LCD5110_set_cursor(1, 40, lcd_conf);
        LCD5110_print("DEUCE", BLACK, lcd_conf);
    }
    LCD5110_refresh(lcd_conf);
}
void print_score_text(LCD5110_display *lcd_conf, const char *str) {
    print_score(lcd_conf);
    LCD5110_set_cursor(1, 40, lcd_conf);
    LCD5110_print(str, BLACK, lcd_conf);
    LCD5110_refresh(lcd_conf);
}

void update_score(state_t winner) {
    if (winner == L_SERVE) {
        l_score++;
        if (r_score == 10 && l_score == 10) {
            deuce = true;
        } else if ((l_score == 11 && !deuce)
                   || (l_score - r_score > 1 && deuce)) {
            print_score_text(&lcd1, "LEFT WON!");
            state = END;
        } else {
            print_score(&lcd1);
            if (((l_score + r_score) % 2 == 0 && !deuce) || deuce) {
                state = (server == R_SERVE) ? L_SERVE : R_SERVE;
            } else {
                state = server;
            }
            server = state;
        }
    } else if (winner == R_SERVE) {
        r_score++;
        if (r_score == 10 && l_score == 10) {
            deuce = true;
        } else if ((r_score == 11 && !deuce)
                   || (r_score - l_score > 1 && deuce)) {
            print_score_text(&lcd1, "RIGHT WON!");
            state = END;
        } else {
            print_score(&lcd1);
            if (((l_score + r_score) % 2 == 0 && !deuce) || deuce) {
                state = (server == R_SERVE) ? L_SERVE : R_SERVE;
            } else {
                state = server;
            }
            server = state;
        }
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  lcd1.hw_conf.spi_handle = &hspi2;
  lcd1.hw_conf.spi_cs_pin = LCD_CS_Pin;
  lcd1.hw_conf.spi_cs_port = LCD_CS_GPIO_Port;
  lcd1.hw_conf.rst_pin = LCD_RST_Pin;
  lcd1.hw_conf.rst_port = LCD_RST_GPIO_Port;
  lcd1.hw_conf.dc_pin = LCD_DC_Pin;
  lcd1.hw_conf.dc_port = LCD_DC_GPIO_Port;
  lcd1.def_scr = lcd5110_def_scr;
  int res = LCD5110_init(&lcd1.hw_conf, LCD5110_NORMAL_MODE, 0x40, 2, 3);

  print_score_text(&lcd1, "SELECT PLAYER (L/R)\n");
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV256;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (listening) {
        listening = false;
        HAL_TIM_Base_Start_IT(&htim2);
    }
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
//////
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  else {
    LCD5110_set_cursor(0, 0, &lcd1);
    switch (state) {
        case START:
            LCD5110_print("START\n", BLACK, &lcd1);
            break;
        case L_SERVE:
            LCD5110_print("L_SERVE\n", BLACK, &lcd1);
            break;
        case R_SERVE:
            LCD5110_print("R_SERVE\n", BLACK, &lcd1);
            break;
        case L_WAIT:
            LCD5110_print("L_WAIT\n", BLACK, &lcd1);
            break;
        case R_WAIT:
            LCD5110_print("R_WAIT\n", BLACK, &lcd1);
            break;
        case L_TURN:
            LCD5110_print("L_TURN\n", BLACK, &lcd1);
            break;
        case R_TURN:
            LCD5110_print("R_TURN\n", BLACK, &lcd1);
            break;
    }
    // tablehit (left side)
    if (!listening) {
        listening = true;
        if (!HAL_GPIO_ReadPin(TABLEHIT_BTN_GPIO_Port, TABLEHIT_BTN_Pin)) {
            if (state == START) {
                state = L_SERVE;
                server = L_SERVE;
            } else if (state == L_SERVE) {
                state = L_WAIT;
            } else if (state == R_SERVE) {
                update_score(L_SERVE);
            } else if (state == L_WAIT) {
                update_score(R_SERVE);
            } else if (state == R_WAIT) {
                state = L_TURN;
            } else if (state == L_TURN) {
                update_score(R_SERVE);
            } else if (state == R_TURN) {
                state = L_TURN;
            };
            HAL_TIM_Base_Start_IT(&htim2);
        }
            // oppositehit (right side)
        else if (!HAL_GPIO_ReadPin(OPPOSITEHIT_BTN_GPIO_Port,
                                   OPPOSITEHIT_BTN_Pin)) {
            if (state == START) {
                state = R_SERVE;
                server = state;
            } else if (state == R_SERVE) {
                state = R_WAIT;
            } else if (state == L_SERVE) {
                update_score(R_SERVE);
            } else if (state == R_WAIT) {
                update_score(L_SERVE);
            } else if (state == L_WAIT) {
                state = R_TURN;
            } else if (state == R_TURN) {
                update_score(L_SERVE);
            } else if (state == L_TURN) {
                state = R_TURN;
            }
            HAL_TIM_Base_Start_IT(&htim2);
        }
    } else {
        HAL_TIM_Base_Stop_IT(&htim2);
    }
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
