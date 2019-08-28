/*
  ==============================================================================

    DspCommon.h
    Created: 20 Aug 2019 8:19:58pm
    Author:  Utokusa
 
  ==============================================================================
*/

#pragma once
#include <algorithm>

// Common
namespace DspCommon
{
    static constexpr float PI = 3.141593f;
    static constexpr double minSampleRate = 1000.0f;
    static constexpr double maxSampleRate = 10000000.0f;
    static constexpr double defaultSampleRate = 44100.0f;
    
    template <typename T>
    T clamp (const T x, const T upper, const T lower)
    {
        return std::min(upper, std::max(x, lower));
    }
    
    double clampSampleRate (double sampleRate);
    
    double fixSampleRate (double sampleRate);
    
    static constexpr double smoothingAlphaOfDefaultSampleRate = 0.1f;
    float smoothParamValue (const float newValue, const float prevSmoothParam, const double sampleRate);
}
