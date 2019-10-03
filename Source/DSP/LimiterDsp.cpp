/*
  ==============================================================================

    LimiterDsp.cpp
    Created: 9 Aug 2019 11:02:34pm
    Author:  Utokusa
  ==============================================================================
*/

#include "LimiterDsp.h"
#include <cmath>

LimiterDsp::LimiterDsp(float threasholdDb, float kneeWidthDb, float attackTime, float releaseTime, int numChannel, bool isGainShareMode)
    : threasholdDb(threasholdDb)
    , kneeWidthDb(kneeWidthDb)
    , attackTime(attackTime)
    , releaseTime(releaseTime)
    , numChannel(numChannel)
    , prevGainSmoothedDb(numChannel)
    , isGainShareMode(isGainShareMode)
    , isLimitterWorking(false)
    , decayValMilliSec(numChannel)
{
}

LimiterDsp::~LimiterDsp(){}

float LimiterDsp::DoProcessOneSample(float inputSigLinear, double sampleRate, int channel, float sidechainSigLinear)
{
    //==============================================================================
    // When isGainShareMode is true, the limiter uses one shared gain.
    if (isGainShareMode)
    {
        channel = 0;
    }
    
    //==============================================================================
    // Convert linear side chain input Signal to dB.
    float InputScSigdB = convertLinearToDb(sidechainSigLinear);
    
    //==============================================================================
    // The gain computer.
    // Calc static characteristic properties (soft knee).
    float staticCharacteristic;

    if (InputScSigdB < threasholdDb - kneeWidthDb / 2.0f)
    {
        staticCharacteristic = InputScSigdB;
    }
    else if ( threasholdDb - kneeWidthDb / 2.0f <= InputScSigdB && InputScSigdB <= threasholdDb + kneeWidthDb / 2.0f)
    {
        staticCharacteristic = InputScSigdB - std::pow(InputScSigdB - threasholdDb + kneeWidthDb / 2.0f, 2.0f) / (2 * kneeWidthDb);
    }
    else
    {
        staticCharacteristic = threasholdDb;
    }
    
    //==============================================================================
    // Calc gain.
    // Gc[n]=Xsc[n] −XdB[n], where Xsc[n] is static characteristic
    // and XdB is input side chain level in dB.
    float computedGainDb;
    if (isinf(InputScSigdB))
    {
        // In case == - Inf
        computedGainDb = 0.0f;
    }
    else
    {
        computedGainDb = staticCharacteristic - InputScSigdB;
    }
    
    //==============================================================================
    // Gain smoothing section
    
    
    static constexpr float forwardVoltage = 90.0f;  // We define forward voltage as 90% of the peak voltage.
    static const float kLogBaseConversion = forwardVoltage / 10.0f;
    // alphaAttack : Smoothing gain used in attack state.
    float alphaAttack = std::exp(- std::log(kLogBaseConversion) / (static_cast<float>(sampleRate) * attackTime));
    // alphaRelease : Smoothing gain used in release state.
    float alphaRelease = std::exp(- std::log(kLogBaseConversion) / (static_cast<float>(sampleRate) * releaseTime));

    float gainSmoothedDb;
    if (computedGainDb <= prevGainSmoothedDb[channel])
    {
        gainSmoothedDb = alphaAttack * prevGainSmoothedDb[channel] + (1.0f - alphaAttack) * computedGainDb;
    }
    else
    {
        gainSmoothedDb = alphaRelease * prevGainSmoothedDb[channel] + (1.0f - alphaRelease) * computedGainDb;
    }
    
    //==============================================================================
    // Conver dB gain to linear gain.
    float gain = convertDbToLinear(gainSmoothedDb);
    prevGainSmoothedDb[channel] = gainSmoothedDb;
    
    //==============================================================================
    // Apply gain.
    float outputSigLinear = inputSigLinear * gain;
    
    return outputSigLinear;
}

float LimiterDsp::DoProcessOneSample(float inputSigLinear, double sampleRate, int channel)
{
    float outputSigLinear = DoProcessOneSample(inputSigLinear, sampleRate, channel, inputSigLinear);
    
    return outputSigLinear;
}

void LimiterDsp::UpdateCounter(float inputSigLinear,float outputSigLinear, double sampleRate, int channel)
{
    // Whien limiter starts working, decayValMilliSec will be set to this value.
    if (std::abs(inputSigLinear - outputSigLinear) > epsilon)
    {
        decayValMilliSec[channel] = ititDecayValMilliSec;
    }
    else if (decayValMilliSec[channel] >= 0.0f)
    {
        jassert(sampleRate > 0);
        decayValMilliSec[channel] -= 1.0f / static_cast<float>(sampleRate) * 1000.0f;
    }
}

void LimiterDsp::UpdateWorkingState()
{
    // isLimitterWorking is true when decayValMilliSec > 0.
    bool next_state = false;
    for (auto cnt : decayValMilliSec)
    {
        if (cnt > epsilon)
        {
            next_state = true;
            break;
        }
    }
    
    isLimitterWorking.store(next_state);
}

bool LimiterDsp::IsWorking()
{
    return isLimitterWorking.load();
}

float LimiterDsp::convertLinearToDb(float linearVal)
{
    return 20 * std::log10(std::abs(linearVal));
}

float LimiterDsp::convertDbToLinear(float dbVal)
{
    return std::pow(10.f, dbVal / 20.f);
}






