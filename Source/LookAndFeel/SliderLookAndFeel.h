/*
  ==============================================================================

    SliderLookAndFeel.h
    Created: 15 Aug 2019 2:48:25pm
    Author:  Utokusa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class SliderLookAndFeel  : public LookAndFeel_V4
{
public:
    
    static constexpr int sliderWidth = 350;
    static constexpr int sliderHeight = 40;
    static constexpr int labelWidth = 30;
    static constexpr int labelHeight = 20;
    static constexpr int dblas = 3;  // distance bwtween label and slider
    
    enum ColourIds
    {
        entryOutlineEditingColourId = 0x80979797,
        entryBackgroundColourId = 0xFFD6D6D6
    };
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle, Slider&) override;
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
    Label* createSliderTextBox (Slider& slider) override;
    Slider::SliderLayout getSliderLayout (Slider&) override;

private:
    static constexpr float thumbWidth = 26.0f;
    static constexpr float thumbHeight = 26.0f;
    
    const Rectangle<int> textBoxBounds = Rectangle<int> (30, 0, 70, 14);
    const Rectangle<int> sliderBounds = Rectangle<int> (12, 24, 326, 40);
};
