/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BiquadLimiterAudioProcessorEditor::BiquadLimiterAudioProcessorEditor (BiquadLimiterAudioProcessor& p, AudioProcessorValueTreeState& vts)
: AudioProcessorEditor (&p), processor (p), valueTreeState (vts)
, smallKnobLookAndFeel (true)
, largeKnobLookAndFeel (false)
, freqRes (processor,valueTreeState, processor.getBiquadFilterDsp())
, limiterWorkingDetector (processor.getBiquadFilterDsp())
, isFreqResUpToDate (false)
{
    // Set the editor's size to whatever you need it to be.
    setSize (windowWidth, windowHeight);
    
    // Initialize biquad filter sliders.
    auto sliderIds = processor.getBiquadFilerCoefIds();
    for (int i = 0; i < Bf::numCoef; ++i)
    {
        s_coef[i].setLookAndFeel (&sliderLookAndFeel);
        s_coef[i].setSliderStyle (Slider::RotaryHorizontalDrag);
        s_coef[i].setMouseDragSensitivity (sensitivityFast);
        s_coef[i].setTextBoxIsEditable(false);
        s_coef[i].setVelocityModeParameters (1, 1, 0.0, false);
        s_coefLabel[i].setText (valueTreeState.getParameter (sliderIds[i])->name, dontSendNotification);
        
        addAndMakeVisible (s_coef[i]);
        s_coefAttachment[i].reset (new SliderAttachment (valueTreeState, sliderIds[i], s_coef[i]));
        addAndMakeVisible (s_coefLabel[i]);
    }
    
    // Initialize frequency slider (knob).
    s_freq.setLookAndFeel (&largeKnobLookAndFeel);
    s_freq.setSliderStyle (Slider::RotaryVerticalDrag);
    s_freq.setTextBoxIsEditable(false);
    s_freqLabel.setText (valueTreeState.getParameter ("frequency")->name, dontSendNotification);
    addAndMakeVisible (s_freq);
    s_freqAttachment.reset (new SliderAttachment (valueTreeState, "frequency", s_freq));
    addAndMakeVisible (s_freqLabel);
    
    // Initialize Q slider (knob).
    s_q.setLookAndFeel (&largeKnobLookAndFeel);
    s_q.setSliderStyle (Slider::RotaryVerticalDrag);
    s_q.setTextBoxIsEditable(false);
    s_qLabel.setText (valueTreeState.getParameter ("q")->name, dontSendNotification);
    addAndMakeVisible (s_q);
    s_qAttachment.reset (new SliderAttachment (valueTreeState, "q", s_q));
    addAndMakeVisible (s_qLabel);
    
    // Initialize input gain slider (knob).
    s_gain.setLookAndFeel (&smallKnobLookAndFeel);
    s_gain.setSliderStyle (Slider::RotaryVerticalDrag);
    s_gain.setTextBoxIsEditable(false);
    s_gainLabel.setText (valueTreeState.getParameter ("gain")->name, dontSendNotification);
    addAndMakeVisible (s_gain);
    s_gainAttachment.reset (new SliderAttachment (valueTreeState, "gain", s_gain));
    addAndMakeVisible (s_gainLabel);
    
    // Initialize filter type combobox.
    auto pTypeParam = dynamic_cast<AudioParameterChoice*>(valueTreeState.getParameter ("filter_type"));
    cb_type.setLookAndFeel(&globalLookAndFeel);
    cb_type.addItemList (pTypeParam->getAllValueStrings(), 1);
    cb_type.setSelectedItemIndex (pTypeParam->getIndex(), dontSendNotification);
    cb_type.setJustificationType (Justification::centred);
    addAndMakeVisible (cb_type);
    cb_typeAttachment.reset (new ComboBoxAttachment (valueTreeState, "filter_type", cb_type));
    
    // Initialize fine tuning on/off button .
    btn_fine.setLookAndFeel (&globalLookAndFeel);
    addAndMakeVisible (btn_fine);
    btn_fine.onClick = [this] { updateToggleState(); };
    
    // Initialize furequency responce graph.
    addAndMakeVisible (freqRes);
    
    // Initialize limiter working detector.
    addAndMakeVisible (limiterWorkingDetector);
    
    
    startTimerHz (100);
    //==============================================================================
    setWantsKeyboardFocus (true);
    
    
}

BiquadLimiterAudioProcessorEditor::~BiquadLimiterAudioProcessorEditor()
{
}

//==============================================================================
void BiquadLimiterAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    Image backgroundImage = ImageCache::getFromMemory(BinaryData::BiquadLimiterBackground_4x_png
                                                      , BinaryData::BiquadLimiterBackground_4x_pngSize);
    g.drawImageWithin(backgroundImage, 0, 0, windowWidth, windowHeight, RectanglePlacement::Flags::xLeft, false);
}

void BiquadLimiterAudioProcessorEditor::resized()
{
    // Lay out the positions of subcomponents.
    
    // For optimization purposes, the background image has a portion of knobs,
    // sliders and graph images. Therefore, we need the following magic numbers
    // to place the components at the predetermined positions.
    // We know this is a problem in terms of maintainability.
    // If there is no performance problem, it will be improved in a future update.
    
    constexpr int sliderX = 75;
    constexpr int sliderYBase = 313;
    constexpr int sliderDelta = 44;
    int sliderCnt = 5;
    constexpr int dblas = SliderLookAndFeel::dblas;  // distance bwtween label and slider
    constexpr int sliderLabelWidth = SliderLookAndFeel::labelWidth;
    constexpr int sliderLabelHeight = SliderLookAndFeel::labelHeight;
    constexpr int sliderWidth = SliderLookAndFeel::sliderWidth;
    constexpr int sliderHeight = SliderLookAndFeel::sliderHeight;
    
    for (int i = 0; i < Bf::numCoef; ++i)
    {
        s_coefLabel[i].setBounds (sliderX, sliderYBase - sliderDelta*sliderCnt, sliderLabelWidth, sliderLabelHeight);
        s_coef[i].setBounds (sliderX, sliderYBase + dblas - sliderDelta*sliderCnt, sliderWidth, sliderHeight);
        --sliderCnt;
    }
    
    constexpr int s_freqX = 299;
    constexpr int s_freqY = 479;
    constexpr int s_freqWidth = KnobLookAndFeel::width_large;
    constexpr int s_freqHeight = KnobLookAndFeel::height_large;
    constexpr int s_freqTextWidth = KnobLookAndFeel::textWidth;
    constexpr int s_freqTextHeight = KnobLookAndFeel::textHeight;
    constexpr int s_freqLabelX = 310;
    constexpr int s_freqLabelY = 543;
    constexpr int s_freqLabelWidth = KnobLookAndFeel::textWidth;
    constexpr int s_freqLabelHeight = KnobLookAndFeel::textHeight;
    s_freq.setBounds (s_freqX-largeKnobLookAndFeel.getLeftBrank(), s_freqY, s_freqWidth, s_freqHeight);
    s_freq.setTextBoxStyle (Slider::TextBoxBelow, false, s_freqTextWidth, s_freqTextHeight);
    s_freqLabel.setBounds (s_freqLabelX,s_freqLabelY,s_freqLabelWidth,s_freqLabelHeight);
    
    constexpr int s_qX = 399;
    constexpr int s_qY = 479;
    constexpr int s_qWidth = KnobLookAndFeel::width_large;
    constexpr int s_qHeight = KnobLookAndFeel::height_large;
    constexpr int s_qTextWidth = KnobLookAndFeel::textWidth;
    constexpr int s_qTextHeight = KnobLookAndFeel::textHeight;
    constexpr int s_qLabelX = 420;
    constexpr int s_qLabelY = 543;
    constexpr int s_qLabelWidth = KnobLookAndFeel::textWidth;
    constexpr int s_qLabelHeight = KnobLookAndFeel::textHeight;
    s_q.setBounds (s_qX-largeKnobLookAndFeel.getLeftBrank(), s_qY, s_qWidth, s_qHeight);
    s_q.setTextBoxStyle (Slider::TextBoxBelow, false, s_qTextWidth, s_qTextHeight);
    s_qLabel.setBounds (s_qLabelX, s_qLabelY, s_qLabelWidth, s_qLabelHeight);
    
    constexpr int s_gainX = 308;
    constexpr int s_gainY = 395;
    constexpr int s_gainWidth = KnobLookAndFeel::width_small;
    constexpr int s_gainHeight = KnobLookAndFeel::height_small;
    constexpr int s_gainTextWidth = KnobLookAndFeel::textWidth;
    constexpr int s_gainTextHeight = KnobLookAndFeel::textHeight;
    constexpr int s_gainLabelX = 320;
    constexpr int s_gainLabelY = 440;
    constexpr int s_gainLabelWidth = KnobLookAndFeel::textWidth;
    constexpr int s_gainLabelHeight = KnobLookAndFeel::textHeight;
    s_gain.setBounds (s_gainX-smallKnobLookAndFeel.getLeftBrank(), s_gainY, s_gainWidth, s_gainHeight);
    s_gain.setTextBoxStyle (Slider::TextBoxBelow, false, s_gainTextWidth, s_gainTextHeight);
    s_gainLabel.setBounds (s_gainLabelX,s_gainLabelY,s_gainLabelWidth,s_gainLabelHeight);
    
    
    constexpr int cb_typeX = 380;
    constexpr int cb_typeY = 403;
    constexpr int cb_typeWidth = GlobalLookAndFeel::comboboxWidth;
    constexpr int cb_typeHeight = GlobalLookAndFeel::comboboxHeight;
    cb_type.setBounds (cb_typeX, cb_typeY, cb_typeWidth, cb_typeHeight);
    
    constexpr int btn_fineX = 350;
    constexpr int btn_fineY = 85;
    constexpr int btn_fineWidth = 60;
    constexpr int btn_fineHeight = 30;
    btn_fine.setBounds (btn_fineX, btn_fineY, btn_fineWidth, btn_fineHeight);
    
    constexpr int freqResX = 42;
    constexpr int freqResY = 396;
    constexpr int freqResWidth = static_cast<int>(FrequencyResponseComponent::frameWidth);
    constexpr int freqResHeight = static_cast<int>(FrequencyResponseComponent::frameHeight);
    freqRes.setBounds (freqResX, freqResY, freqResWidth, freqResHeight);
    
    constexpr int lwdX = 385;  //limiterWorkingDetector : lwd
    constexpr int lwdY = 440;
    constexpr int lwdWidth = LimiterWorkingDetector::width;
    constexpr int lwdHeight = LimiterWorkingDetector::height;
    limiterWorkingDetector.setBounds (lwdX, lwdY, lwdWidth, lwdHeight);
}

void BiquadLimiterAudioProcessorEditor::updateToggleState()
{
    
    auto state = btn_fine.getToggleState();
    int dragSensitivity;
    if (state)
    {
        dragSensitivity = sensitivitySlow;
        
    }
    else
    {
        dragSensitivity = sensitivityFast;
    }
    
    for (int i = 0; i < Bf::numCoef; ++i)
    {
        s_coef[i].setMouseDragSensitivity (dragSensitivity);
    }
}

void BiquadLimiterAudioProcessorEditor::setFreqResUpToDateFlag (bool val)
{
    isFreqResUpToDate.store (val);
}

void BiquadLimiterAudioProcessorEditor::timerCallback()
{
    
        if (!(processor.getIsUpToDate()))
        {
            freqRes.repaint();
            processor.setIsUpToDate(true);
        }
    
//    freqRes.repaint();
}
