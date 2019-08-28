/*
  ==============================================================================

    LimiterWorkingDetector.cpp
    Created: 11 Aug 2019 4:17:26pm
    Author:  Utokusa

  ==============================================================================
*/

#include "LimiterWorkingDetector.h"


LimiterWorkingDetector::LimiterWorkingDetector(BiquadLimiter& biquadFilter)
    : biquadFilter(biquadFilter)
{
    startTimerHz(30);
}

// Lights the LED if the limiter is working.
void LimiterWorkingDetector::paint(juce::Graphics &g)
{
    g.setColour (Colours::white);
    
    g.drawText(String("Limiter"), gap * 2 + largerCircleRadius, gap, textWidth, textHeight, Justification::topLeft);
    auto isClipping = biquadFilter.isLimiterWorking();
    float thickness;
    if (true == isClipping)
    {
        // LED is on.
        g.setColour(Colour(ClippingClourId));
        thickness = 2.0f;
    }
    else
    {
        // LED is off.
        g.setColour(Colour(NotClippingColourId));
        thickness = 2.0f;
    }
    g.drawEllipse(gap + SubRadius, gap + SubRadius, smallerCircleRadius, smallerCircleRadius, thickness);
}



