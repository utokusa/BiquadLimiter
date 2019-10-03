/*
  ==============================================================================

    LimiterWorkingDetector.h
    Created: 11 Aug 2019 4:17:26pm
    Author:  Utokusa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../DSP/BiquadLimiterDsp.h"

// When limiter in Biquad Limiter working, the detector LED shines.
class LimiterWorkingDetector  : public Component
                                     , Timer
{
public:
    
    static constexpr int width = 70;
    static constexpr int height = 20;
    
    enum ColourIds
    {
        ClippingClourId = 0x8FBD8383,  // Shining LED red color.
        NotClippingColourId = 0x8FFFFFFF  // Not shining.
    };
    LimiterWorkingDetector(BiquadLimiter& biquadFilter);
    void paint(Graphics& g) override;  // Lights the LED if the limiter is working.
    void resized() override {}
    void timerCallback() override {repaint();}
private:
    BiquadLimiter& biquadFilter;
    
    //=============================================================================
    static constexpr float largerCircleDiameter = 14.0f;
    static constexpr float smallerCircleDiameter = 8.0f;
    static constexpr float diffRadius = (largerCircleDiameter - smallerCircleDiameter) / 2.0f;
    static constexpr int numCircle = 5 ;
    static constexpr float gap = 5.f;
    static constexpr float textWidth = 50.0f;
    static constexpr float textHeight = 20.0f;
    
    //=============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LimiterWorkingDetector)
};
