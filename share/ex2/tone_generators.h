#ifndef CTEST_FUNCTIONS_H
#define CTEST_FUNCTIONS_H

#include <stdint.h>

uint32_t sineWave(int frequency, int time, int amplitude, int sampling_frequency, uint32_t *phase); //Generate sine wave
uint32_t squareWave(int frequency, int time, int amplitude, int sampling_frequency); //Generate square wave
uint32_t triangleWave(int frequency, int time, int amplitude, int sampling_frequency); //Generate triangle wave
int sawtoothWave(int frequency, int time, int amplitude, int sampling_frequency); //Generate sawtotth wave
int hitEffect(uint32_t counter); //Generate 'hit' sound effect
int metal_effect(uint32_t counter); //Generate 'hitting metal' sound effect
int win_effect(uint32_t counter); //Generate 'winning' sound effect

#endif //CTEST_FUNCTIONS_H
