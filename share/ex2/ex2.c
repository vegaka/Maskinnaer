#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "dac.h"
#include "tone_generators.h"
#include "timer.h"

/* The period between sound samples, in clock cycles */
#define   SAMPLE_PERIOD   560

/* Declaration of peripheral setup functions */
void setup_gpio();
void setup_timer(uint32_t period);
void setup_dac();
void setup_nvic();

#ifdef BASELINE
void play_effects();
void reset_counters(uint16_t flags, uint16_t buttons);
#endif

int main(void)
{
	/* Call the peripheral setup functions */
	setup_gpio();
	setup_dac();
	setup_timer(SAMPLE_PERIOD);

	/* Turn off the lights */
	*GPIO_PA_DOUT = *GPIO_PA_DOUT | 0xff00;

#ifdef BASELINE

	uint16_t old_buttons = 0xffff;
	uint16_t old_time_value = 0x0000;

	while (1) {

		/* Get timer value */
		uint16_t time_value = *TIMER1_CNT;
		if (time_value < old_time_value)
			play_effects();

		old_time_value = time_value;

		/* Get currently pressed buttons */
		uint16_t buttons = *GPIO_PC_DIN;

		/* Figure out which buttons have changed */
		uint16_t changed_buttons = old_buttons ^ buttons;

		/* Figure out which buttons have been pressed down sice the last poll */
		uint16_t buttons_pressed = changed_buttons & (~buttons);
		
		reset_counters(buttons_pressed, ~buttons);

		old_buttons = buttons;
	}
#else
	
	/* Enable interrupt handling */
	setup_nvic();

	/* Enter Energy Mode 1 (EM1) by writing 0b0xx0 to EMU_CTRL; x is don't care bits */
	*EMU_CTRL = 0;
	
	/* Enable sleep and sleep on return from ISR by writing 2 to SystemControlRegister */
	*SCR = 2;
	
	/* Assembly instruction for 'wait for interrupt' */
	__asm__("wfi");

#endif

	return 0;
}

/*
 *  Set up the NVIC for GPIO interrupts and TIMER1 interrupt.
 *
 *  Arguments: None
 *  Returns: Void
 */
void setup_nvic()
{
	*ISER0 = 0b1100000000010;
}
