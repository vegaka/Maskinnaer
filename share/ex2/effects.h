#ifndef EFFECTS_H
#define EFFECTS_H

#include <stdint.h>
#include <math.h>

#include "efm32gg.h"
#include "tone_generators.h"
#include "sounds.h"

#define SAMPLES_PER_NOTE 7250
#define MELODY_NOTES 23

uint32_t phase = 0;

/* Initialize counters to values that prevents them from playing at startup */
uint32_t hit_counter = 1024;
uint32_t metal_counter = 1024;
uint32_t win_counter = 1024 * 10;
uint32_t melody_counter = SAMPLES_PER_NOTE * MELODY_NOTES;

uint32_t new_sample = 0;

void play_effects();
void reset_counters(uint16_t flags, uint16_t buttons);

#endif
