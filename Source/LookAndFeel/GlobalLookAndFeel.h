/*
  ==============================================================================

    GlobalLookAndFeel.h
    Created: 15 Aug 2019 2:47:34pm
    Author:  Utokusa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class GlobalLookAndFeel  : public LookAndFeel_V4
{
public:
    
    static constexpr int comboboxWidth = 80;
    static constexpr int comboboxHeight = 32;
    
    enum ColourIds
    {
        PopMenueColourId = 0xA0979797,
        PopMenueHighlightedColourId = 0xA0D6D6D6
    };
    
    GlobalLookAndFeel ();
    ~GlobalLookAndFeel () {}
    
    void drawComboBox (Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       ComboBox& box) override;
    Font getComboBoxFont (ComboBox& box) override;
    
    void positionComboBoxText (ComboBox& box, Label& label) override;
    
    void drawTickBox (Graphics& g, Component &, float x, float y, float w, float h
                     , bool ticked, bool isEnabled
                     , bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
};
