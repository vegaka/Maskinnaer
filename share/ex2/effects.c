#include "effects.h"

void play_effects()
{
	/* Melody */
	if (melody_counter < MELODY_NOTES * SAMPLES_PER_NOTE) {
		int note_index = floor(melody_counter / SAMPLES_PER_NOTE);

		new_sample = sine_wave(startup_melody[note_index],
					    melody_counter,
					    4096 / 2, 25000,
					    &phase);

		melody_counter++;
	} else if (hit_counter < 1024) {
		new_sample = hit_effect(hit_counter);
		hit_counter++;
	} else if (metal_counter < 1024) {
		new_sample = metal_effect(metal_counter);
		metal_counter++;
	} else if (win_counter < 1024 * 10) {
		new_sample = win_effect(win_counter);
		win_counter++;
	}

	*DAC0_CH0DATA = new_sample;
	*DAC0_CH1DATA = new_sample;

}

void reset_counters(uint16_t interrupt_flags, uint16_t current_buttons)
{
	int play_hit_effect = interrupt_flags & 1 & current_buttons;
	int play_metal_effect = interrupt_flags & (1 << 1) & current_buttons;
	int play_win_effect = interrupt_flags & (1 << 2) & current_buttons;
	int play_melody = interrupt_flags & (1 << 3) & current_buttons;

	if (play_hit_effect)
		hit_counter = 0;

	if (play_metal_effect)
		metal_counter = 0;

	if (play_win_effect)
		win_counter = 0;

	if (play_melody)
		melody_counter = 0;

}
