/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "./GUI/FreqResponseComponent.h"
#include "./LookAndFeel/GlobalLookAndFeel.h"
#include "./LookAndFeel/KnobLookAndFeel.h"
#include "./LookAndFeel/SliderLookAndFeel.h"
#include "./GUI/LimiterWorkingDetector.h"
#include "./DSP/BiquadLimiterDsp.h"

//==============================================================================
/**
 */
class BiquadLimiterAudioProcessorEditor  : public AudioProcessorEditor
                                                , Timer
{
public:
    BiquadLimiterAudioProcessorEditor (BiquadLimiterAudioProcessor&, AudioProcessorValueTreeState&);
    ~BiquadLimiterAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    //    void sliderValueChanged (Slider* slider) override;
    //    void comboBoxChanged (ComboBox* comboBox) override;
    void timerCallback() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BiquadLimiterAudioProcessor& processor;
    
    AudioProcessorValueTreeState& valueTreeState;
    
    //==============================================================================
    static constexpr int windowHeight = 600;
    static constexpr int windowWidth = 500;
    
    //==============================================================================
    
    KnobLookAndFeel smallKnobLookAndFeel;
    KnobLookAndFeel largeKnobLookAndFeel;
    SliderLookAndFeel sliderLookAndFeel;
    GlobalLookAndFeel globalLookAndFeel;
    
    using Bf = BiquadLimiter;
    std::array<Slider, Bf::numCoef> s_coef;
    std::array<Label, Bf::numCoef> s_coefLabel;
    
    Slider s_freq;
    Slider s_q;
    Slider s_gain;
    
    Label s_freqLabel;
    Label s_qLabel;
    Label s_gainLabel;
    
    ComboBox cb_type;
    Label cb_typeLabel;
    
    ToggleButton btn_fine { "Fine" };
    
    //==============================================================================
    using SliderAttachment = AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = AudioProcessorValueTreeState::ComboBoxAttachment;
    
    std::array<std::unique_ptr<SliderAttachment>, Bf::numCoef> s_coefAttachment;
    std::unique_ptr<SliderAttachment> s_freqAttachment;
    std::unique_ptr<SliderAttachment> s_qAttachment;
    std::unique_ptr<SliderAttachment> s_gainAttachment;
    
    std::unique_ptr<ComboBoxAttachment> cb_typeAttachment;
    
    FrequencyResponseComponent freqRes;
    LimiterWorkingDetector limiterWorkingDetector;
    
    std::atomic<bool> isFreqResUpToDate;
    
    // Slider sensitivity
    static constexpr int sensitivitySlow = 28000;
    static constexpr int sensitivityFast = 700;
    
    //==============================================================================
    void updateToggleState();
    void setFreqResUpToDateFlag (bool val);
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadLimiterAudioProcessorEditor)
};
