/*
  ==============================================================================

    FreqResponseComponent.h
    Created: 11 Jul 2019 10:31:11pm
    Author:  Utokusa

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../DSP/DspCommon.h"
#include "../DSP/BiquadLimiterDsp.h"
#include <complex>
#include <cmath>

// Draw frequency responce.
class FrequencyResponseComponent  : public juce::Component
{
public:
    
    static constexpr float frameWidth = 238;
    static constexpr float frameHeight = 170;
    
    FrequencyResponseComponent(AudioProcessor& p, AudioProcessorValueTreeState& vts, BiquadLimiter& bl);
    FrequencyResponseComponent() = delete;
    
    ~FrequencyResponseComponent(){}
    
    void paint(Graphics& g) override;
    
    void resized() override{}
    
    
    
private:
    AudioProcessor& processor;
    AudioProcessorValueTreeState& parameters;
    BiquadLimiter& biauadLimiter;
    
    int BorderThickness;
    int graphX;
    int graphY;
    int graphWidth;
    int graphHeight;
    
    Path filter_response_path_;
    
    std::vector<Line<float>> tickLines;
    
    float getParamValue(String s);
    float getSmoothedParamValue(int index);
    void createGraphPath(Graphics& g);
    using cp = std::complex<float>;
    float calcGain(float freq, cp a0, cp a1, cp a2, cp b0, cp b1, cp b2);
    
    //=============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyResponseComponent)
};
