#ifndef CTEST_FUNCTIONS_H
#define CTEST_FUNCTIONS_H

#include <stdint.h>

uint32_t sineWave(int frequency, int time, int amplitude, int sampling_frequency, uint32_t *phase);
uint32_t squareWave(int frequency, int time, int amplitude, int sampling_frequency);
uint32_t triangleWave(int frequency, int time, int amplitude, int sampling_frequency);
int sawtoothWave(int frequency, int time, int amplitude, int sampling_frequency);
int hitEffect(uint32_t counter);
int metal_effect(uint32_t counter);
int win_effect(uint32_t counter);

#endif //CTEST_FUNCTIONS_H
