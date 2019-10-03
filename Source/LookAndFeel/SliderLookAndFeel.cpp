/*
  ==============================================================================

    SliderLookAndFeel.cpp
    Created: 15 Aug 2019 2:48:25pm
    Author:  Utokusa

  ==============================================================================
*/

#include "SliderLookAndFeel.h"

//==============================================================================
void SliderLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                       float sliderPos, float minSliderPos, float maxSliderPos,
                       const Slider::SliderStyle, Slider&)
{
    
    Image thumb = ImageCache::getFromMemory(BinaryData::SliderThumb_4x_png, BinaryData::SliderThumb_4x_pngSize);
    constexpr float positionXOffset = 13.0f;
    float positionX = sliderPos - positionXOffset;
    constexpr float positionY = 14.0f;
    Rectangle<float> knobArea(positionX, positionY, thumbWidth, thumbHeight);
    g.drawImage(thumb, knobArea, RectanglePlacement::Flags::xLeft);
}

void SliderLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    
    Image knob = ImageCache::getFromMemory(BinaryData::SliderThumb_4x_png, BinaryData::SliderThumb_4x_pngSize);
    
    float positionX = sliderPos * (static_cast<float>(sliderWidth) - thumbWidth);
    constexpr float positionY = 14.0f;
    Rectangle<float> knobArea(positionX, positionY, thumbWidth, thumbHeight);
    g.drawImage(knob, knobArea, RectanglePlacement::Flags::xLeft, false);
}

Label* SliderLookAndFeel::createSliderTextBox (Slider& slider)
{
    auto* l = LookAndFeel_V3::createSliderTextBox (slider);
    l->setColour (Label::outlineColourId, Colours::transparentWhite );
    l->setColour (Label::outlineWhenEditingColourId, Colour(entryOutlineEditingColourId) );
    l->setColour (TextEditor::backgroundColourId,Colour(entryBackgroundColourId));
    
    return l;
}

Slider::SliderLayout SliderLookAndFeel::getSliderLayout (Slider&)
{
    Slider::SliderLayout layout;
    
    layout.textBoxBounds = textBoxBounds;
    layout.sliderBounds = sliderBounds;
    
    return layout;
}
