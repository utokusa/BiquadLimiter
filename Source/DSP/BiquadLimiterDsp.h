/*
  ==============================================================================

    BiquadLimiter.h
    Created: 11 Jul 2019 10:28:44pm
    Author:  Utokusa

  ==============================================================================
*/

#pragma once
#include "LimiterDsp.h"
#include "DspCommon.h"
#include <array>

// DSP class of the biquad filter whitch has a limiter (limiter1) in front of the feedback section,
// and an another limiter in the final section (limiter2).
// Both prevent the output from becoming too loud when the filter is unstable or the input is too loud.
// In addition, limitter1 makes the sound interesting by feeding back the compressed
// sound to the biquad filter.
class BiquadLimiter
{
public:
    static constexpr int numCoef = 6;
    BiquadLimiter(float a0, float a1, float a2, float b0, float b1, float b2, int numChannel);
    BiquadLimiter(std::array<float, numCoef> coef, int numChannel);
    
    BiquadLimiter() = delete;
    ~BiquadLimiter(){};
    
    void setCoefficient(float a0, float a1, float a2, float b0, float b1, float b2, double sampleRate);
    void setCoefficient(std::array<float, numCoef> new_coef, double sampleRate);
    const std::array<float, numCoef>& getSmoothedCofficient() const {return smoothed_coef;}
    const float& operator [] ( const int i ) const;
    void DoProcess(float* bufferPtr, int bufferSize, int channel, double sampleRate);
    bool isLimiterWorking() {return limiter1.IsWorking();}
    void UpdateWorkingState() {limiter1.UpdateWorkingState();}
    float getCoefVal (int index);
    
private:
    struct BiquadLimiterBuffer
    {
    public:
        BiquadLimiterBuffer():in1(0.0f), in2(0.0f), out1(0.0f), out2(0.0f) {}
        ~BiquadLimiterBuffer(){};
        float in1, in2;
        float out1, out2;
    };
    
    // limiter1 parameters
    static constexpr float l1_threasholdDb = 0.0f;
    static constexpr float l1_kneeWidthDb = 6.0f;
    static constexpr float l1_attackTime = 0.002f;
    static constexpr float l1_releaseTime = 0.200f;
    static constexpr bool l1_isGainShareMode = false;
    
    // limiter2 parameters
    static constexpr float l2_threasholdDb = 3.0f;
    static constexpr float l2_kneeWidthDb = 3.0f;
    static constexpr float l2_attackTime = 0.00f;
    static constexpr float l2_releaseTime = 0.200f;
    static constexpr bool l2_isGainShareMode = false;
    
    int numChannel;
    std::array<float, numCoef> smoothed_coef; // smoothed coefficients
    std::array<float, numCoef> coef; // coefficients read from sliders
    std::vector<BiquadLimiterBuffer> biquadFilterBuffer;
    LimiterDsp limiter1;
    LimiterDsp limiter2;
    
    // alphaOfDefaultSampleRate is the value of "alpha" when SampleRate is the default value,
    // where "alpha" is a smoothing coefficient for BiquadLimiter coefficients.
    // This variable is necessary because alpha is proportional to the sampling rate.
    static constexpr float alphaOfDefaultSampleRate = 0.3f;
    static constexpr double defaultSampleRate = 44100.0f;
    float smoothVal(float smoothed, float input, double sampleRate);
    
    
};
