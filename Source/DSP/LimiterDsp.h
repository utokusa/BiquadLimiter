/*
  ==============================================================================

    LimiterDsp.h
    Created: 9 Aug 2019 11:02:34pm
    Author:  Utokusa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

// limiter used in befor feedback section of biquad filer.
// This implementation is based on the link below.
// https://jp.mathworks.com/help/audio/ref/limiter-system-object.html
class LimiterDsp
{
public:
    LimiterDsp(float threasholdDb, float kneeWidthDb, float attackTime, float releaseTime, int numChannel, bool isGainShareMode);
    virtual ~LimiterDsp();
    virtual float DoProcessOneSample(float inputSigLinear, double sampleRate, int channel, float sidechainSigLinear);
    float DoProcessOneSample(float inputSigLinear, double sampleRate, int channel);
    void UpdateCounter(float inputSigLinear,float outputSigLinear, double sampleRate, int channel);
    void UpdateWorkingState();
    bool IsWorking();
    
private:
    float threasholdDb;
    float kneeWidthDb;
    float attackTime;
    float releaseTime;
    int numChannel;
    std::vector<float> prevGainSmoothedDb;
    bool isGainShareMode;
    
    std::atomic<bool> isLimitterWorking;
    std::vector<float> cntDecayMilliSec;  // isLimitterWorking is true when cntDecayMilliSec > 0.
    const float epsilon = 0.0001f;
    const float stateDecayMilliSec = 100.0f;  // Whien limiter starts working, cntDecayMilliSec will be set to this value.
    
    float convertLinearToDb(float linearVal);
    float convertDbToLinear(float dbVal);
};

