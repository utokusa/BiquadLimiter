/*
  ==============================================================================

    LimiterDsp.h
    Created: 9 Aug 2019 11:02:34pm
    Author:  Utokusa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

// This is the limiter class used in befor feedback section of BiquadLimiter.
// The implementation is based on the link below.
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
    std::vector<float> decayValMilliSec;  // isLimitterWorking is true when decayValMilliSec > 0.
    const float epsilon = 0.0001f;
    const float ititDecayValMilliSec = 100.0f;  // Whien limiter starts working, decayValMilliSec will be set to this value.
    
    float convertLinearToDb(float linearVal);
    float convertDbToLinear(float dbVal);
};

