/*
  ==============================================================================

    BiquadLimiterDsp.cpp
    Created: 18 Aug 2019 8:40:42pm
    Author:  Utokusa

  ==============================================================================
*/

#include "BiquadLimiterDsp.h"


BiquadLimiter::BiquadLimiter(float a0, float a1, float a2
                             , float b0, float b1, float b2, int numChannel_)
    : numChannel(numChannel_)
    , smoothed_coef{a0,a1,a2,b0,b1,b2}
    , coef{a0,a1,a2,b0,b1,b2}
    , biquadFilterBuffer(numChannel)
    , limiter1(l1_threasholdDb, l1_kneeWidthDb, l1_attackTime, l1_releaseTime, numChannel, l1_isGainShareMode)
    , limiter2(l2_threasholdDb, l2_kneeWidthDb, l2_attackTime, l2_releaseTime, numChannel, l2_isGainShareMode)
{
}

BiquadLimiter::BiquadLimiter(std::array<float, numCoef> coef, int numChannel_)
    : numChannel(numChannel_)
    , smoothed_coef(coef)
    , coef(coef)
    , biquadFilterBuffer(numChannel)
    , limiter1(l1_threasholdDb, l1_kneeWidthDb, l1_attackTime, l1_releaseTime, numChannel, l1_isGainShareMode)
    , limiter2(l2_threasholdDb, l2_kneeWidthDb, l2_attackTime, l2_releaseTime, numChannel, l2_isGainShareMode)
{
}

void BiquadLimiter::setCoefficient(float a0, float a1, float a2
                                   , float b0, float b1, float b2
                                   , double sampleRate)
{
    double fixedSamplingRate = DspCommon::defaultSampleRateIfOutOfRange(sampleRate);
    coef = smoothed_coef;

    smoothed_coef[0] = smoothVal(smoothed_coef[0], a0, fixedSamplingRate);
    smoothed_coef[1] = smoothVal(smoothed_coef[1], a1, fixedSamplingRate);
    smoothed_coef[2] = smoothVal(smoothed_coef[2], a2, fixedSamplingRate);
    smoothed_coef[3] = smoothVal(smoothed_coef[3], b0, fixedSamplingRate);
    smoothed_coef[4] = smoothVal(smoothed_coef[4], b1, fixedSamplingRate);
    smoothed_coef[5] = smoothVal(smoothed_coef[5], b2, fixedSamplingRate);
}

void BiquadLimiter::setCoefficient(std::array<float, numCoef> new_coef
                                   , double sampleRate)
{
    double fixedSamplingRate = DspCommon::defaultSampleRateIfOutOfRange(sampleRate);
    coef = smoothed_coef;
    
    for (int i = 0; i < numCoef; ++i)
    {
        smoothed_coef[i] = smoothVal(smoothed_coef[i], new_coef[i], fixedSamplingRate);
    }
    
}

const float& BiquadLimiter::operator [] ( const int i ) const
{
    assert(i >= 0);
    return smoothed_coef[i];
}

void BiquadLimiter::DoProcess(float* bufferPtr, int bufferSize
                              , int channel, double sampleRate)
{
    double fixedSamplingRate = DspCommon::defaultSampleRateIfOutOfRange(sampleRate);
    constexpr int numCoef = 6;
    std::vector<float> delta_coef(numCoef);
    for (int k = 0; k < numCoef; ++k)
    {
        delta_coef[k] = (smoothed_coef[k] - coef[k]) / bufferSize;
    }
    
    std::vector<float> curr_coef(numCoef); // current coefficient
    std::copy(coef.begin(), coef.end(), curr_coef.begin());
    
    for (int i = 0; i < bufferSize; i++) {
        for (int k = 0; k < numCoef; ++k)
        {
            curr_coef[k] += delta_coef[k];
        }
        
        if (channel <= numChannel)
        {
            // Calculate biquad filter output.
            float out0 = (curr_coef[3] * bufferPtr[i]
                          + curr_coef[4] * biquadFilterBuffer[channel].in1
                          + curr_coef[5] * biquadFilterBuffer[channel].in2
                          - curr_coef[1] * biquadFilterBuffer[channel].out1
                          - curr_coef[2] * biquadFilterBuffer[channel].out2
                          ) / curr_coef[0];
            
            if (isnan(out0))out0= 0;  //out0 equals NaN
            auto filterOut = out0;
            
            // Apply limiter before returning the signal to the input.
            out0 = limiter1.DoProcessOneSample(out0, fixedSamplingRate, channel);
            
            // buffering
            biquadFilterBuffer[channel].in2 = biquadFilterBuffer[channel].in1;
            biquadFilterBuffer[channel].in1 = bufferPtr[i];
            
            biquadFilterBuffer[channel].out2 = biquadFilterBuffer[channel].out1;
            biquadFilterBuffer[channel].out1 = out0; // Feedback the signal with the limiter applied.
            
            // Apply limiter to the final output.
            out0 = limiter2.DoProcessOneSample(out0, fixedSamplingRate, channel);
            
            // Check the status of limiter.
            limiter1.UpdateCounter(filterOut, out0, fixedSamplingRate, channel);
            
            
            bufferPtr[i] = out0;
        }
        
    }
}

float BiquadLimiter::getCoefVal (int index)
{
    int fixedIndex = DspCommon::clamp<int> (index, numCoef, 0);
    return smoothed_coef[fixedIndex];
}

float BiquadLimiter::smoothVal(float smoothed, float input, double sampleRate)
{
    float alpha = alphaOfDefaultSampleRate * static_cast<float>(sampleRate / defaultSampleRate);
    
    return alpha * smoothed + (1 - alpha) * input;
}

