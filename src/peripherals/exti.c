/*
 * exti.c
 *
 *  Created on: 25 oct. 2022
 *      Author: Ludo
 */

#include "exti.h"

#include "exti_reg.h"
#include "gpio.h"
#include "mapping.h"
#include "mode.h"
#include "nvic.h"
#include "rcc_reg.h"
#include "syscfg_reg.h"
#include "types.h"

/*** EXTI local macros ***/

#define EXTI_RTSR_FTSR_RESERVED_INDEX	18
#define EXTI_RTSR_FTSR_MAX_INDEX		22

/*** EXTI local functions ***/

/* SET EXTI TRIGGER.
 * @param trigger:	Interrupt edge trigger (see EXTI_trigger_t enum).
 * @param line_idx:	Line index.
 * @return:			None.
 */
static void _EXTI_set_trigger(EXTI_trigger_t trigger, uint8_t line_idx) {
	// Select triggers.
	switch (trigger) {
	// Rising edge only.
	case EXTI_TRIGGER_RISING_EDGE:
		EXTI -> RTSR |= (0b1 << line_idx); // Rising edge enabled.
		EXTI -> FTSR &= ~(0b1 << line_idx); // Falling edge disabled.
		break;
	// Falling edge only.
	case EXTI_TRIGGER_FALLING_EDGE:
		EXTI -> RTSR &= ~(0b1 << line_idx); // Rising edge disabled.
		EXTI -> FTSR |= (0b1 << line_idx); // Falling edge enabled.
		break;
	// Both edges.
	case EXTI_TRIGGER_ANY_EDGE:
		EXTI -> RTSR |= (0b1 << line_idx); // Rising edge enabled.
		EXTI -> FTSR |= (0b1 << line_idx); // Falling edge enabled.
		break;
	// Unknown configuration.
	default:
		break;
	}
	// Clear flag.
	EXTI -> PR |= (0b1 << line_idx);
}

/*** EXTI functions ***/

/* INIT EXTI PERIPHERAL.
 * @param:	None.
 * @return:	None.
 */
void EXTI_init(void) {
	// Enable peripheral clock.
	RCC -> APB2ENR |= (0b1 << 0); // SYSCFEN='1'.
	// Mask all sources by default.
	EXTI -> IMR = 0;
	// Clear all flags.
	EXTI_clear_all_flags();
	// Set interrupts priority.
	NVIC_set_priority(NVIC_INTERRUPT_EXTI_0_1, 3);
	NVIC_set_priority(NVIC_INTERRUPT_EXTI_4_15, 0);
}

/* CONFIGURE A GPIO AS EXTERNAL INTERRUPT SOURCE.
 * @param gpio:		GPIO to be attached to EXTI peripheral.
 * @param trigger:	Interrupt edge trigger (see EXTI_trigger_t enum).
 * @return:			None.
 */
void EXTI_configure_gpio(const GPIO_pin_t* gpio, EXTI_trigger_t trigger) {
	// Select GPIO port.
	SYSCFG -> EXTICR[((gpio -> pin_index) / 4)] &= ~(0b1111 << (4 * ((gpio -> pin_index) % 4)));
	SYSCFG -> EXTICR[((gpio -> pin_index) / 4)] |= ((gpio -> port_index) << (4 * ((gpio -> pin_index) % 4)));
	// Set mask.
	EXTI -> IMR |= (0b1 << ((gpio -> pin_index))); // IMx='1'.
	// Select triggers.
	_EXTI_set_trigger(trigger, (gpio -> pin_index));
}

/* CONFIGURE A LINE AS INTERNAL INTERRUPT SOURCE.
 * @param line:		Line to configure (see EXTI_line_t enum).
 * @param trigger:	Interrupt edge trigger (see EXTI_trigger_t enum).
 * @return:			None.
 */
void EXTI_configure_line(EXTI_line_t line, EXTI_trigger_t trigger) {
	// Set mask.
	EXTI -> IMR |= (0b1 << line); // IMx='1'.
	// Select triggers.
	if ((line != EXTI_RTSR_FTSR_RESERVED_INDEX) || (line <= EXTI_RTSR_FTSR_MAX_INDEX)) {
		_EXTI_set_trigger(trigger, line);
	}
}

/* CLEAR ALL EXTI FLAGS.
 * @param:	None.
 * @return:	None.
 */
void EXTI_clear_all_flags(void) {
	// Clear all flags.
	EXTI -> PR |= 0x007BFFFF; // PIFx='1'.
}
