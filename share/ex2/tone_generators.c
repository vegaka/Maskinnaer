#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "tone_generators.h"

#define PI 3.14159265

int sineWave(int freq, int time, int amp, int samplingFreq) {
    //printf("%f", amp * sin(2 * PI * time/samplingFreq * freq) / 2);
    return amp/2 * sin(2 * PI * time/samplingFreq * freq) + (amp / 2);
}

int squareWave(int freq, int time, int amp, int samplingFreq) {
    //printf("%f", amp * (sin(2 * PI * time/samplingFreq * freq) > 0 ? 1 : -1 ) / 2);
    return amp * (sin(2 * PI * time/samplingFreq * freq) > 0 ? 1 : 0 );
}

int triangleWave(int freq, int time, int amp, int samplingFreq) {
    double progress = (double)time*freq/(samplingFreq) - floor(time*freq/(samplingFreq));
    //printf("%f", amp * ((progress < 0.5 ? progress : 1- progress)*2-0.5));
    return amp * ((progress < 0.5 ? progress : 1- progress)*2-0.5);
}

int sawtoothWave(int freq, int time, int amp, int samplingFreq) {
    double progress = (double)time*freq/(samplingFreq) - floor(time*freq/(samplingFreq));
    //printf("%f", amp * ((progress) -0.5));
    return amp * (progress - 0.5);
}
