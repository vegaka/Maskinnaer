#include "effects.h"

/*
 *  Feeds new samples to the DAC.
 *  The melody has priority over all other sounds.
 *  The hit effect has priority over the metal effect and the win effect.
 *  The metal effect has priority over the win effect.
 *  The win effect has the lowest priority.
 *
 *  Arguments: None
 *  Returns: Void	
 */
void play_effects()
{
	/* Melody */
	if (melody_counter < MELODY_NOTES * SAMPLES_PER_NOTE) {
		int note_index = floor(melody_counter / SAMPLES_PER_NOTE);

		new_sample = sine_wave(startup_melody[note_index],
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

/*
 *  Resets the counters for the effects so that they can be played again.
 *
 *  Arguments: buttons_changed: Bitmap of the buttons that changed state 
 *                              with 1 indicating that a button changed state.
 *             current_buttons: Bitmap of the current state of the buttons
 *                              with 1 indicating that the button is pressed.
 *  Returns: Void
 */
void reset_counters(uint16_t buttons_changed, uint16_t current_buttons)
{
	/*  Figure out which button was pressed down.
	 *  buttons_changed & (1 << (button_number)) checks whether button_number
	 *  changed state or not.
	 *  This & current_buttons gives us whether the button was pressed down or released.
	 */ 
	int play_hit_effect = buttons_changed & 1 & current_buttons;
	int play_metal_effect = buttons_changed & (1 << 1) & current_buttons;
	int play_win_effect = buttons_changed & (1 << 2) & current_buttons;
	int play_melody = buttons_changed & (1 << 3) & current_buttons;

	if (play_hit_effect)
		hit_counter = 0;

	if (play_metal_effect)
		metal_counter = 0;

	if (play_win_effect)
		win_counter = 0;

	if (play_melody)
		melody_counter = 0;

}
