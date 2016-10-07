#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"
#include "tone_generators.h"
#include "sounds.h"

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	   TODO feed new samples to the DAC
	   remember to clear the pending interrupt by writing 1 to TIMER1_IFC
	 */

	//Sound
	/*uint32_t samplesPerNote = 7250;
	int noteArrayIndex = floor(counter / samplesPerNote);
	int index = noteArrayIndex;

	if (noteArrayIndex < 23)
	{
		*DAC0_CH0DATA = sineWave(startUpSoundLeft[index], counter, 4096 / 2, 25000, &phaseLeft);
		*DAC0_CH1DATA = sineWave(startUpSoundRight[index] * 2, counter, 4096 / 2, 25000, &phaseRight);
	}*/

	if (counter < 1024 * 10)
	{
		*DAC0_CH0DATA = win_effect(counter);
		*DAC0_CH1DATA = win_effect(counter);
	}



	counter++;

	*GPIO_PA_DOUT = ~(*GPIO_PA_DOUT);

	/* Clear pending interrupt */
	*TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	/* TODO handle button pressed event, remember to clear pending interrupt */
	*GPIO_PA_DOUT = ~(*GPIO_PA_DOUT);
	counter = 0;

	// Cleanup
	*GPIO_IFC = *GPIO_IF;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	/* TODO handle button pressed event, remember to clear pending interrupt */
	*GPIO_PA_DOUT = ~(*GPIO_PA_DOUT);
	counter = 0;

	// Cleanup
	*GPIO_IFC = *GPIO_IF;
}
