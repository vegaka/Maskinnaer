#ifndef CTEST_FUNCTIONS_H
#define CTEST_FUNCTIONS_H

#include <stdint.h>

uint32_t sineWave(int frequency, int time, int amplitude, int samplingFrequency);
uint32_t squareWave(int frequency, int time, int amplitude, int samplingFrequency);
uint32_t triangleWave(int frequency, int time, int amplitude, int samplingFrequency);
int sawtoothWave(int frequency, int time, int amplitude, int samplingFrequency);
#endif //CTEST_FUNCTIONS_H
