/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"

/* External variables --------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc;

/******************************************************************************/
/* Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1) {}
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/**
  * @brief This function handles RTC alarm interrupt through EXTI line 17.
  * Required for the Peak Hour Alert mentioned in the report.
  */
void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&hrtc);
}
