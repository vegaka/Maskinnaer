#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "sineTables.h"

#define PI 3.14159265


uint32_t sineWave(int freq, int time, int amp, int samplingFreq) {
    //uint32_t temp = amp/2 * sin(2 * PI * time/samplingFreq * freq) + 2048;
    int temp = (int)floor(freq * 2048 / samplingFreq);
    int temp2 = (time * temp) % 1024;
    return sineList[temp2] / 2 + 1024;
}

uint32_t squareWave(int freq, int time, int amp, int samplingFreq) {
    uint32_t temp = amp/2 * (sin(2 * PI * time/samplingFreq * freq) > 0 ? 1 : -1 ) + 2048;
    return temp;
}

uint32_t triangleWave(int freq, int time, int amp, int samplingFreq) {
    double progress = (double)time*freq/(samplingFreq) - floor(time*freq/(samplingFreq));
    uint32_t temp = amp/2 * (progress < 0.5 ? 2*progress : 2 - 2*progress) + 2048;
    return temp;
}

int sawtoothWave(int freq, int time, int amp, int samplingFreq) {
    double progress = (double)time*freq/(samplingFreq) - floor(time*freq/(samplingFreq));
    return amp * (progress - 0.5);
}
