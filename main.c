/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @author         : AHMED MD SHAKIL
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;
UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
void UpdateParkingStatus(void);
void SetRTCAlarm(uint8_t hours, uint8_t minutes);

/**
  * @brief  The application entry point.
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();

  /* Set RTC Alarm for Peak Hour (e.g., 18:00) as per report [cite: 135] */
  SetRTCAlarm(18, 00);

  /* Infinite loop */
  while (1)
  {
    UpdateParkingStatus();
    HAL_Delay(1000); // 1 second delay
  }
}

/**
  * @brief  Checks sensor status, updates LEDs, and sends data to PC.
  * Logic derived from report section 4.2 [cite: 89]
  */
void UpdateParkingStatus(void) {
    uint8_t availableSlots = 0;
    char msg[50];

    // Check all 4 parking slots
    for (int i = 0; i < 4; i++) {
        // Read Sensor State (PA0-PA3)
        // Note: Using bit shifting to check pins 0, 1, 2, 3
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, (GPIO_PIN_0 << i));

        if (state == GPIO_PIN_SET) {
            // Vehicle Detected -> LED OFF (Occupied)
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 << i), GPIO_PIN_RESET);
        } else {
            // No Vehicle -> LED ON (Available)
            HAL_GPIO_WritePin(GPIOB, (GPIO_PIN_0 << i), GPIO_PIN_SET);
            availableSlots++;
        }
    }

    // Toggle Status LED (PC13) if parking is full [cite: 106]
    if (availableSlots == 0) {
        HAL_GPIO_WritePin(GPIOC, STATUS_LED_Pin, GPIO_PIN_SET); 
    } else {
        HAL_GPIO_WritePin(GPIOC, STATUS_LED_Pin, GPIO_PIN_RESET);
    }

    // Send status to PC via USART [cite: 119]
    sprintf(msg, "Available Spaces: %d / 4\r\n", availableSlots);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

/**
  * @brief  Sets an RTC Alarm at specific HH:MM.
  * Logic derived from report section 4.3 [cite: 135]
  */
void SetRTCAlarm(uint8_t hours, uint8_t minutes) {
    RTC_AlarmTypeDef sAlarm = {0};

    sAlarm.AlarmTime.Hours = hours;
    sAlarm.AlarmTime.Minutes = minutes;
    sAlarm.AlarmTime.Seconds = 0;
    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
    sAlarm.Alarm = RTC_ALARM_A;
    
    HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

/**
  * @brief  Callback for RTC Alarm Interrupt.
  * Logic derived from report section 4.3 [cite: 159]
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
    char msg[] = "RTC Alarm Triggered: Peak Hour Alert!\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

/**
  * @brief USART2 Initialization Function
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief RTC Initialization Function
  */
static void MX_RTC_Init(void)
{
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* Configure Sensors (PA0-PA3) */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Configure LEDs (PB0-PB3) */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* Configure Status LED (PC13) */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
  
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}