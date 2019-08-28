/*
  ==============================================================================

    DspCommon.cpp
    Created: 20 Aug 2019 8:19:58pm
    Author:  Utokusa

  ==============================================================================
*/

#include "DspCommon.h"

namespace DspCommon
{
    double clampSampleRate (double sampleRate)
    {
        return clamp<double> (sampleRate, maxSampleRate, minSampleRate);
    }
    
    double fixSampleRate (double sampleRate)
    {
        double dst;
        if (sampleRate < minSampleRate || sampleRate > maxSampleRate)
        {
            dst = defaultSampleRate;
        }
        else
        {
            dst = sampleRate;
        }
        
        return dst;
    }
    
    float smoothParamValue (const float newValue, const float prevSmoothParam, const double sampleRate)
    {
        float smoothParamVal;
        float alpha = smoothingAlphaOfDefaultSampleRate * sampleRate / defaultSampleRate;
        smoothParamVal = alpha * newValue + (1.0f - alpha) * prevSmoothParam;
        
        return smoothParamVal;
    }
}
