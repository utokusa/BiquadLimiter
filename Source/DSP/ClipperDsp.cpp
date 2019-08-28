/*
  ==============================================================================

    ClipperDsp.cpp
    Created: 16 Aug 2019 11:49:37am
    Author:  Utokusa

  ==============================================================================
*/

#include "ClipperDsp.h"
#include <cmath>

float clippingFunctionTanh(float inputValue)
{
    float threshold = std::tanhf(inputValue);
    float outputValue = inputValue;
    if (std::abs(inputValue) >= std::abs(threshold)) outputValue = threshold;
    
    return outputValue;
}
float clippingFunctionNormal(float inputValue)
{
    float outputValue = inputValue;
    constexpr float threashold = 2.0f;
    if (outputValue > threashold)
    {
        outputValue = threashold;
    }
    else if (outputValue < -threashold)
    {
        outputValue = -threashold;
    }
    
    return outputValue;
}
