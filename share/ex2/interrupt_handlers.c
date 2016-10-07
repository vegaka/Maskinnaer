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
		uint32_t sample = sineWave(startUpSoundLeft[index], counter, 4096 / 2, 25000, &phaseLeft);
		*DAC0_CH0DATA = sample;
		*DAC0_CH1DATA = sample;
	}*/

	if (hit_counter < 1024)
	{
		effect_sample = hitEffect(hit_counter);
		*DAC0_CH0DATA = effect_sample;
		*DAC0_CH1DATA = effect_sample;
		hit_counter++;
	} else if (metal_counter < 1024)
	{
		effect_sample = metal_effect(metal_counter);
		*DAC0_CH0DATA = effect_sample;
		*DAC0_CH1DATA = effect_sample;
		metal_counter++;
	} else if (win_counter < 1024 * 10)
	{
		effect_sample = win_effect(win_counter);
		*DAC0_CH0DATA = effect_sample;
		*DAC0_CH1DATA = effect_sample;
		win_counter++;
	}



	counter++;

	*GPIO_PA_DOUT = ~(*GPIO_PA_DOUT);

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

	int play_hit_effect = (interrupt_flags & 1) & current_buttons;
	int play_metal_effect = (interrupt_flags & (1 << 1)) & current_buttons;
	int play_win_effect = (interrupt_flags & (1 << 2)) & current_buttons;

	if (play_hit_effect)
	{
		hit_counter = 0;
	}

	if (play_metal_effect)
	{
		metal_counter = 0;
	}

	if (play_win_effect)
	{
		win_counter = 0;
	}

	// Cleanup
	*GPIO_IFC = *GPIO_IF;	
}