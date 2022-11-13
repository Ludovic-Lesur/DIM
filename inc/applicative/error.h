/*
 * error.h
 *
 *  Created on: 25 oct. 2022
 *      Author: Ludo
 */

#ifndef __ERROR_H__
#define __ERROR_H__

// Peripherals.
#include "adc.h"
#include "flash.h"
#include "iwdg.h"
#include "lptim.h"
#include "lpuart.h"
#include "rcc.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
// Utils.
#include "math.h"
#include "parser.h"
#include "string.h"
#include "types.h"
// Components.
#include "rs485.h"

/*** ERROR macros ***/

#define ERROR_STACK_DEPTH	6

/*** ERROR structures ***/

typedef enum {
	SUCCESS = 0,
	ERROR_BUSY_SPY_RUNNING,
	ERROR_TX_DISABLED,
	ERROR_RS485_ADDRESS_DISABLED,
	// Peripherals.
	ERROR_BASE_ADC1 = 0x0100,
	ERROR_BASE_FLASH = (ERROR_BASE_ADC1 + ADC_ERROR_BASE_LAST),
	ERROR_BASE_IWDG = (ERROR_BASE_FLASH + FLASH_ERROR_BASE_LAST),
	ERROR_BASE_LPTIM1 = (ERROR_BASE_IWDG + IWDG_ERROR_BASE_LAST),
	ERROR_BASE_LPUART1 = (ERROR_BASE_LPTIM1 + LPTIM_ERROR_BASE_LAST),
	ERROR_BASE_RCC = (ERROR_BASE_LPUART1 + LPUART_ERROR_BASE_LAST),
	ERROR_BASE_RTC = (ERROR_BASE_RCC + RCC_ERROR_BASE_LAST),
	ERROR_BASE_TIM21 = (ERROR_BASE_RTC + RTC_ERROR_BASE_LAST),
	ERROR_BASE_USART = (ERROR_BASE_TIM21 + TIM_ERROR_BASE_LAST),
	// Utils.
	ERROR_BASE_MATH = (ERROR_BASE_USART + USART_ERROR_BASE_LAST),
	ERROR_BASE_PARSER = (ERROR_BASE_MATH + MATH_ERROR_BASE_LAST),
	ERROR_BASE_STRING = (ERROR_BASE_PARSER + PARSER_ERROR_BASE_LAST),
	// Components.
	ERROR_BASE_RS485 = (ERROR_BASE_STRING + STRING_ERROR_BASE_LAST),
	// Last index.
	ERROR_BASE_LAST = (ERROR_BASE_RS485 + RS485_ERROR_BASE_LAST)
} ERROR_t;

/*** ERROR functions ***/

void ERROR_stack_init(void);
void ERROR_stack_add(ERROR_t status);
void ERROR_stack_read(ERROR_t* error_stack);
uint8_t ERROR_stack_is_empty(void);

#define ERROR_status_check(status, success, error_base) { \
	if (status != success) { \
		ERROR_stack_add(error_base + status); \
	} \
}

#define ERROR_status_check_print(status, success, error_base) { \
	if (status != success) { \
		ERROR_stack_add(error_base + status); \
		_AT_print_status(error_base + status); \
		goto errors; \
	} \
}

#endif /* __ERROR_H__ */
