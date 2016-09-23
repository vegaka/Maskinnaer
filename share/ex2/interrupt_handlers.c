#include <stdint.h>
#include <stdbool.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"
#include "tone_generators.h"

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	   TODO feed new samples to the DAC
	   remember to clear the pending interrupt by writing 1 to TIMER1_IFC
	 */

	//Sound
	//int sample = squareWave(440, counter, 4096 / 16, 50000);
	//*DAC0_CH0DATA = squareWave(440, counter, 4096 / 2, 1000000);
	//*DAC0_CH1DATA = squareWave(440, counter, 4096 / 2, 1000000);
	//*DAC0_CH0DATA = sample;
	//*DAC0_CH1DATA = sample;

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

	// Cleanup
	*GPIO_IFC = *GPIO_IF;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	/* TODO handle button pressed event, remember to clear pending interrupt */
	*GPIO_PA_DOUT = ~(*GPIO_PA_DOUT);

	// Cleanup
	*GPIO_IFC = *GPIO_IF;
}
