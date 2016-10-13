#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "sine_table.h"
#include "tone_generators.h"

uint32_t sine_wave(int freq, int time, int amp, int sampling_frequency,
		   uint32_t * phase)
{
	int temp = (int)floor(freq * 2048 / sampling_frequency);
	*phase = (*phase + temp) % 1024;
	return sine_list[*phase] / 32 + 1024;
}

int hit_effect(uint32_t counter)
{
	return sine_list[counter % 1024] / 2 + 1024;
}

int metal_effect(uint32_t counter)
{
	int temp_counter = (counter * 20) % 1024;
	return sine_list[temp_counter] / 16 + 1024;
}

int win_effect(uint32_t counter)
{
	if (counter < 1024 * 3) {
		int temp_counter = (counter * 9) % 1024;
		return sine_list[temp_counter] / 16 + 1024;
	} else {
		int temp_counter = (counter * 12) % 1024;
		return sine_list[temp_counter] / 16 + 1024;
	}
}
