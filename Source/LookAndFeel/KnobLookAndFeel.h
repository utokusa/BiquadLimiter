/*
  ==============================================================================

    KnobLookAndFeel.h
    Created: 15 Aug 2019 2:48:15pm
    Author:  Utokusa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class KnobLookAndFeel  : public LookAndFeel_V4
{
public:
    
    static constexpr int width_large = 100;
    static constexpr int height_large = 100;
    static constexpr int width_small = 100;
    static constexpr int height_small = 80;
    static constexpr int textWidth = 100;
    static constexpr int textHeight = 20;
    
    enum ColourIds
    {
        entryOutlineEditingColourId = 0x80979797,
        entryBackgroundColourId = 0xFFD6D6D6
    };
    
    KnobLookAndFeel (float isSmall);
    ~KnobLookAndFeel () {}

    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
    Label* createSliderTextBox (Slider& slider) override;
    const int getLeftBrank () const;
    
private:
    float leftBrank;
    float sliderSize;
    float thumbAndShadowSize;
    float distanceBetweenSliderCenterAndKnobCenter;
    bool isSmall;
};
