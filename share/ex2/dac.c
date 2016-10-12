#include <stdint.h>
#include <stdbool.h>

#include "dac.h"
#include "efm32gg.h"

void setupDAC()
{
	/*
	   TODO enable and set up the Digital-Analog Converter

	   1. Enable the DAC clock by setting bit 17 in CMU_HFPERCLKEN0

	   2. Prescale DAC clock by writing 0x50010 to DAC0_CTRL
	   3. Enable left and right audio channels by writing 1 to DAC0_CH0CTRL and DAC0_CH1CTRL
	   4. Write a continuous stream of samples to the DAC data registers, DAC0_CH0DATA and DAC0_CH1DATA, for example from a timer interrupt
	 */

	 //Enable the DAC clock by setting bit 17 in CMU_HFPERCLKEN0
	 uint32_t enable_flag = 1 << 17;
	 *CMU_HFPERCLKEN0 = *CMU_HFPERCLKEN0 | enable_flag;

	 //Prescale DAC clock by writing 0x50010 to DAC0_CTRL
	 *DAC0_CTRL = 0x50010;

	 //Enable left and right audio channels by writing 1 to DAC0_CH0CTRL and DAC0_CH1CTRL
	 *DAC0_CH0CTRL = 1;
	 *DAC0_CH1CTRL = 1;


}
