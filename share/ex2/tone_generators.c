#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "sineTables.h"
#include "tone_generators.h"

#define PI 3.14159265

uint32_t sineWave(int freq, int time, int amp, int sampling_frequency, uint32_t *phase) {
    //uint32_t temp = amp/2 * sin(2 * PI * time/sampling_frequency * freq) + 2048;
    int temp = (int)floor(freq * 2048 / sampling_frequency);
    *phase = (*phase + temp) % 1024;
    return sine_list[*phase] / 2 + 1024;
}

uint32_t squareWave(int freq, int time, int amp, int sampling_frequency) {
    uint32_t temp = amp/2 * (sin(2 * PI * time/sampling_frequency * freq) > 0 ? 1 : -1 ) + 2048;
    return temp;
}

uint32_t triangleWave(int freq, int time, int amp, int sampling_frequency) {
    double progress = (double)time*freq/(sampling_frequency) - floor(time*freq/(sampling_frequency));
    uint32_t temp = amp/2 * (progress < 0.5 ? 2*progress : 2 - 2*progress) + 2048;
    return temp;
}

int sawtoothWave(int freq, int time, int amp, int sampling_frequency) {
    double progress = (double)time*freq/(sampling_frequency) - floor(time*freq/(sampling_frequency));
    return amp * (progress - 0.5);
}

int hitEffect(uint32_t counter)
{
  return sine_list[counter % 1024] / 2 + 1024;
}

int metal_effect(uint32_t counter)
{
    int temp_counter = (counter * 20) % 1024;
    return sine_list[temp_counter] / 2 + 1024;
}

int win_effect(uint32_t counter)
{
    if (counter < 1024 * 3)
    {
        int temp_counter = (counter * 9) % 1024;
        return sine_list[temp_counter] / 2 + 1024;
    } else {
        int temp_counter = (counter * 12) % 1024;
        return sine_list[temp_counter] / 2 + 1024;
    }
}
