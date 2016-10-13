#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "dac.h"
#include "tone_generators.h"
#include "timer.h"

/*
  TODO calculate the appropriate sample period for the sound wave(s)
  you want to generate. The core clock (which the timer clock is derived
  from) runs at 14 MHz by default. Also remember that the timer counter
  registers are 16 bits.
*/
/* The period between sound samples, in clock cycles */
#define   SAMPLE_PERIOD   560

/* Declaration of peripheral setup functions */
void setup_gpio();
void setup_timer(uint32_t period);
void setup_dac();
void setup_nvic();

void play_effects();
void reset_counters(uint16_t flags, uint16_t buttons);

/* Your code will start executing here */
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

		// Reset lights
		*GPIO_PA_DOUT = *GPIO_PA_DOUT | 0xff00;

		*GPIO_PA_DOUT = *GPIO_PA_DOUT & (*GPIO_PC_DIN << 8);

		// Get timer value
		uint16_t time_value = *TIMER1_CNT;
		if (time_value < old_time_value)
			play_effects();

		old_time_value = time_value;

		// Get currently pressed buttons
		uint16_t buttons = *GPIO_PC_DIN;
		uint16_t changed_buttons = old_buttons ^ buttons;
		uint16_t buttons_pressed = changed_buttons & (~buttons);
		reset_counters(buttons_pressed, ~buttons);

		old_buttons = buttons;
	}
#else
	
	/* Enable interrupt handling */
	setup_nvic();

	// Kan vi gå lavere enn EM1?
	//Enter Energy Mode 1 (EM1) by writing 0b0xx0 to EMU_CTRL
	*EMU_CTRL = 0;
	//Enable sleep by writing 0 to bit 2 in SystemControlRegister
	*SCR = 2;
	//Assmebly instruction for 'wait for interrupt'
	__asm__("wfi");

#endif

	return 0;
}

void setup_nvic()
{
	/* TODO use the NVIC ISERx registers to enable handling of interrupt(s)
	   remember two things are necessary for interrupt handling:
	   - the peripheral must generate an interrupt signal
	   - the NVIC must be configured to make the CPU handle the signal
	   You will need TIMER1, GPIO odd and GPIO even interrupt handling for this
	   assignment.
	 */
	*GPIO_EXTIPSELL = 0x22222222;
	*GPIO_EXTIRISE = 0xff;
	*GPIO_EXTIFALL = 0xff;
	*GPIO_IEN = 0xff;
	*ISER0 = 0b1100000000010;

}
