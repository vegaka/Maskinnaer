#include "effects.h"


void playEffects()
{
	// Melody
	if (melody_counter < MELODY_NOTES * SAMPLES_PER_NOTE)
	{
		int note_array_index = floor(melody_counter / SAMPLES_PER_NOTE);

		uint32_t sample = sineWave(start_up_sound_left[note_array_index], melody_counter, 4096 / 2, 25000, &phase_left);
		*DAC0_CH0DATA = sample;
		*DAC0_CH1DATA = sample;

		melody_counter++;
	} else if (hit_counter < 1024)
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
}

void resetCounters(uint16_t interrupt_flags, uint16_t current_buttons)
{
	int play_hit_effect = (interrupt_flags & 1) & current_buttons;
	int play_metal_effect = (interrupt_flags & (1 << 1)) & current_buttons;
	int play_win_effect = (interrupt_flags & (1 << 2)) & current_buttons;
	play_melody = (interrupt_flags & (1 << 3)) & current_buttons;

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

	if (play_melody)
	{
		melody_counter = 0;
	}
}
