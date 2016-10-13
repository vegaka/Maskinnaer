#ifndef TONE_GENERATORS_H
#define TONE_GENERATORS_H

#include <stdint.h>

#define PI 3.14159265

uint32_t sine_wave(int frequency, int time, int amplitude, int sampling_frequency, uint32_t * phase);	//Generate sine wave
int hit_effect(uint32_t counter);	//Generate 'hit' sound effect
int metal_effect(uint32_t counter);	//Generate 'hitting metal' sound effect
int win_effect(uint32_t counter);	//Generate 'winning' sound effect

#endif
