#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "sine_table.h"
#include "tone_generators.h"

/*
 *  Samples a sine wavetable go get the amplitude at a given phase
 *
 *  Arguments: freq: The frequency of the tone you want to generate
 *	       amp: The amplitude of the signal
 * 	       sampling_frequency: How often the wavetable is sampled
 *	       phase: Where in the wavetable the last sample was gathererd
 *                    from. Makes the note transitions smooth
 *  Returns: Sample normalized around 2048
 */
uint32_t sine_wave(int freq, int amp, int sampling_frequency,
		   uint32_t * phase)
{
	int temp = (int)floor(freq * 2048 / sampling_frequency);
	*phase = (*phase + temp) % 1024;
	return sine_list[*phase] / 32 + 1024;
}

/*
 *  Samples the wavetable to generate a tone-effect
 *  
 *  Arguments: counter: Where in the wavetable to sample from
 *  Returns: Sample normalized around 2048
 */
uint32_t hit_effect(uint32_t counter)
{
	return sine_list[counter % 1024] / 2 + 1024;
}

/*
 *  Samples the wavetable to generate a metal-effect
 *  
 *  Arguments: counter: Where in the wavetable to sample from
 *  Returns: Sample normalized around 2048
 */
uint32_t metal_effect(uint32_t counter)
{
	int temp_counter = (counter * 20) % 1024;
	return sine_list[temp_counter] / 16 + 1024;
}

/*
 *  Samples the wavetable to generate a win-effect
 *  consisting of two notes (a 4th) played
 *  after eachother
 *  
 *  Arguments: counter: Where in the wavetable to sample from
 *  Returns: Sample normalized around 2048
 */
uint32_t win_effect(uint32_t counter)
{
	if (counter < 1024 * 3) {
		int temp_counter = (counter * 9) % 1024;
		return sine_list[temp_counter] / 16 + 1024;
	} else {
		int temp_counter = (counter * 12) % 1024;
		return sine_list[temp_counter] / 16 + 1024;
	}
}
