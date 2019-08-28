/*
  ==============================================================================

    GlobalLookAndFeel.cpp
    Created: 15 Aug 2019 2:47:34pm
    Author:  Utokusa

  ==============================================================================
*/

#include "GlobalLookAndFeel.h"

//==============================================================================
GlobalLookAndFeel::GlobalLookAndFeel()
{
    setColour (PopupMenu::backgroundColourId, Colour(PopMenueColourId));
    setColour (PopupMenu::highlightedBackgroundColourId, Colour(PopMenueHighlightedColourId));
}

void GlobalLookAndFeel::drawComboBox (Graphics& g, int width, int height, bool isButtonDown,
                   int buttonX, int buttonY, int buttonW, int buttonH,
                   ComboBox& box)
{  
}

Font GlobalLookAndFeel::getComboBoxFont (ComboBox& box)
{
    constexpr float minimumFontSize = 14.0f;
    constexpr float heightToFontSizeRatio = 0.7f;
    return { jmin (minimumFontSize, box.getHeight() * heightToFontSizeRatio) };
}

void GlobalLookAndFeel::positionComboBoxText (ComboBox& box, Label& label)
{
    constexpr float x = 4.0f;
    constexpr float y = 1.0f;
    constexpr float widthMargin = 30.0f;
    constexpr float widthRatio = 1.0f;
    constexpr float heightMargin = 2.0f;
    constexpr float heightRatio = 0.8f;
    float textWidth = (box.getWidth() - widthMargin) * widthRatio;
    float textHeight = (box.getHeight() - heightMargin) * heightRatio;
    
    label.setBounds (x, y, textWidth, textHeight);
    label.setFont (getComboBoxFont (box));
}

void GlobalLookAndFeel::drawTickBox (Graphics& g, Component &, float x, float y, float w, float h
                  , bool ticked, bool isEnabled
                  , bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    constexpr float ratio = 0.7;
    constexpr float thickness = 1.0f;
    g.setColour (Colours::white);
    Rectangle<float> Area (x + w*(1-ratio)/2.0f,y+ h*(1-ratio)/2.0f,w*ratio,h*ratio);
    g.drawEllipse (Area,thickness);
    if (ticked)
    {
        constexpr float tickRatio = 0.4;
        Rectangle<float> tickArea (x + w*(1-tickRatio)/2.0f,y+ h*(1-tickRatio)/2.0f,w*tickRatio,h*tickRatio);
        g.fillEllipse (tickArea);
    }
}
