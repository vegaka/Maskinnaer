#include <math.h>
#include <stdint.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"
#include "tone_generators.h"

void playEffects();
void resetCounters(uint16_t flags, uint16_t buttons);

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	   TODO feed new samples to the DAC
	   remember to clear the pending interrupt by writing 1 to TIMER1_IFC
	 */

	playEffects();

	/* Clear pending interrupt */
	*TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	handle_GPIO_interrupt();
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	handle_GPIO_interrupt();
}

void handle_GPIO_interrupt()
{
	/* TODO handle button pressed event, remember to clear pending interrupt */
	uint16_t interrupt_flags = *GPIO_IF;
	uint16_t current_buttons = ~ *GPIO_PC_DIN;

	resetCounters(interrupt_flags, current_buttons);

	// Cleanup
	*GPIO_IFC = *GPIO_IF;
}
