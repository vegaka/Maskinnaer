#include <math.h>
#include <stdint.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"
#include "tone_generators.h"

void play_effects();
void reset_counters(uint16_t flags, uint16_t buttons);

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/* Play the currect effect */
	play_effects();

	/* Clear pending interrupt */
	*TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	handle_gpio_interrupt();
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	handle_gpio_interrupt();
}

/*
 *  Handler for all GPIO interrupts.
 *
 *  Arguments: None
 *  Returns: Void
 */
void handle_gpio_interrupt()
{
	uint16_t interrupt_flags = *GPIO_IF;
	uint16_t current_buttons = ~*GPIO_PC_DIN;

	/* Reset counters based on buttons pressed and the interrupt */
	reset_counters(interrupt_flags, current_buttons);

	/* Cleanup */
	*GPIO_IFC = *GPIO_IF;
}
