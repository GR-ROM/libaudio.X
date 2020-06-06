#include "compressor.h"
#include <math.h>
#include <xc.h>
#include <stdbool.h>

float inline log10f(float n) { return(log(n)/log(10)); }

float ratio2db(float ratio) {
    return 20 * log10f(ratio);
}

float db2ratio(float db) {
    return pow(10, db / 20);
}

void computeDecibelsFullScale(int16_t *samples, float *dBFS, int length){
    for (int i=0;i!=length;i++){
        dBFS[i]=20*log10f((float)abs(samples[i])+1);
    }
}

void computeRMS(float *dBFS, int length, int windowSize){
    float rms=0.0f;
    float dBFSt;
    for (int i=0;i!=windowSize;i++){
        dBFSt=dBFS[i];
        rms+=dBFSt*dBFSt;
    }
    for (int i=0;i!=windowSize;i++){
        dBFSt=dBFS[i];
        rms-=dBFSt*dBFSt;
        dBFSt=dBFS[i+windowSize];
        rms+=dBFSt*dBFSt;
        dBFS[i]=sqrt(rms/windowSize);
    }
}

void smoothGainReduction(float newGain, float &lastGain, float& targetGain, float& gainDelta, float& gain, bool& attackFlag, bool& releaseFlag, int& samplesCounter, float attackTime, float releaseTime) {
    const int SAMPLING_RATE = 48000;
    float sampleTime = 1.0f / SAMPLING_RATE;
    float samplesCountAttack = attackTime / (sampleTime*1000); // all times in milliseconds
    float samplesCountRelease = releaseTime / (sampleTime*1000);
    if (newGain < gain && !releaseFlag) {
        attackFlag = true;
        releaseFlag = false;
        targetGain = newGain;
    }
    if (newGain > gain && !attackFlag) {
        attackFlag = true;
        releaseFlag = false;
        targetGain = newGain;
    }
    if (newGain < lastGain && releaseFlag) {
        samplesCounter = samplesCountRelease;
        gainDelta = (newGain / gain) / samplesCountRelease;
    }
    if (newGain > lastGain && attackFlag) {
        samplesCounter = samplesCountAttack;
        gainDelta = (newGain / gain) / samplesCountAttack;
    }
    if (attackFlag) {
        gain += gainDelta;
        samplesCounter--;
        if (samplesCounter == 0) attackFlag = false;
    }
    if (releaseFlag) {
        gain -= gainDelta;
        samplesCounter--;
        if (samplesCounter == 0) releaseFlag = false;
    }
    lastGain = targetGain;
}

void process_compressor_16bit(int16_t *samples, int length, float threshold, float ratio, float makeUpGain, float attackTime, float releaseTime){
    float dBFS[length];
    float gainRedution[length];
    float dBFSt;
    float overthreshold;
    float compressed;
    float delta;
    float newGain;
    float gain;
    float lastGain, gainDelta, targetGain;
    bool attackFlag;
    bool releaseFlag; 
    int samplesCounter;
    ratio=1/ratio;
    computeDecibelsFullScale(samples, dBFS, length);
    computeRMS(dBFS, length, length/4);
    
    for (int i=0;i!=length;i++){
        dBFSt=dBFS[i];
        if (dBFSt>threshold){
            overthreshold=dBFSt-threshold;
            overthreshold=overthreshold/ratio;
            compressed=overthreshold+threshold;
            delta=dBFSt-compressed;
        }
        newGain=db2ratio(delta);
        smoothGainReduction(newGain, lastGain, targetGain, gainDelta, gain, attackFlag, releaseFlag, samplesCounter, attackTime, releaseTime);
        gainRedution[i]=gain;
    }
    
    for (int i=0;i!=length;i++){
        samples[i]=(samples[i]/gainRedution[i])*makeUpGain;
    }
}