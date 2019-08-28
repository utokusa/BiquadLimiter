/*
  ==============================================================================

    KnobLookAndFeel.cpp
    Created: 15 Aug 2019 2:48:15pm
    Author:  Utokusa

  ==============================================================================
*/

#include "KnobLookAndFeel.h"

//==============================================================================
KnobLookAndFeel::KnobLookAndFeel (float isSmall)
    :isSmall (isSmall)
{
    if (isSmall)
    {
        leftBrank = 25.0f;
        sliderSize = 44.0f;
        thumbAndShadowSize = 12.0f;
    }
    else
    {
        leftBrank = 18.0f;
        sliderSize = 61.0f;
        thumbAndShadowSize = 15.6f;
    }
    const auto ratio = 13.0f / 12.0f;
    distanceBetweenSliderCenterAndKnobCenter = thumbAndShadowSize * ratio;
}

void KnobLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    Point<float> thumbCenterPoint(0, -distanceBetweenSliderCenterAndKnobCenter);
    thumbCenterPoint.applyTransform (AffineTransform::rotation (angle).translated (sliderSize / 2.0f + leftBrank, sliderSize / 2.0f));
    Image thumb;
    if (isSmall)
    {
        thumb = ImageCache::getFromMemory(BinaryData::SmallThumb_4x_png, BinaryData::SmallThumb_4x_pngSize);
    }
    else
    {
        thumb = ImageCache::getFromMemory(BinaryData::Thumb_4x_png, BinaryData::Thumb_4x_pngSize);
    }
    
    Rectangle<float> thumbArea(thumbCenterPoint.getX()- thumbAndShadowSize / 2.0f, thumbCenterPoint.getY()- thumbAndShadowSize / 2.0f, thumbAndShadowSize, thumbAndShadowSize);
    g.drawImage(thumb, thumbArea, RectanglePlacement::Flags::xLeft, false);
}

Label* KnobLookAndFeel::createSliderTextBox (Slider& slider)
{
    auto* l = LookAndFeel_V3::createSliderTextBox (slider);
    l->setColour (Label::outlineColourId, Colours::transparentWhite );
    l->setColour (Label::outlineWhenEditingColourId, Colour(entryOutlineEditingColourId) );
    l->setColour (TextEditor::backgroundColourId,Colour(entryBackgroundColourId));
    
    return l;
}

const int KnobLookAndFeel::getLeftBrank () const
{
    return leftBrank;
}
