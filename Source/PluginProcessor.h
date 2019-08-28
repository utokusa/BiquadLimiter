/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

 
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "./DSP/BiquadLimiterDsp.h"
#include "./DSP/ClipperDsp.h"
#include "./DSP/DspCommon.h"

//==============================================================================
/**
 */
class BiquadLimiterAudioProcessor  : public AudioProcessor
                                          , AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    BiquadLimiterAudioProcessor();
    ~BiquadLimiterAudioProcessor();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void setBiquadFilterCoef();
    
    dsp::WaveShaper<float> clipper;
    
    BiquadLimiter& getBiquadFilterDsp() {return biquadLimiter;}
    std::vector<String> getBiquadFilerCoefIds() {return coefIds;}
    
    
    //==============================================================================
    void setIsUpToDate (bool flag) {isGraphUpToDate.store (flag);}
    bool getIsUpToDate()const {return isGraphUpToDate.load();}
    
private:
    //==============================================================================
    static constexpr int maxNumChannels = 2;
    
    AudioProcessorValueTreeState parameters;
    
    
    using Bf = BiquadLimiter;
    
    const std::array<float, Bf::numCoef>  defaultCoefficient;
    BiquadLimiter biquadLimiter;
    
    std::vector<String> coefIds;
    std::vector<String> coefNames;
    
    std::array<float*, Bf::numCoef>  coefValPtrs;
    float* freq = nullptr;
    float* q = nullptr;
    float* gain = nullptr;
    float previousGain;
    
    dsp::ProcessSpec spec;
    
    //==============================================================================
    // parameter value convertion from [0, 1.0] float to physical quantity.
    
    // frequency
    static constexpr float kLowestFreqVal = 20.0f;
    static constexpr float kFreqBaseNumber = 1000.0f;
    const float getFreqVal(const float* freq)const
    {
        return kLowestFreqVal * pow (kFreqBaseNumber, *freq);
    }
    
    // q
    static constexpr float kLowestQVal = 0.2f;
    static constexpr float kQBaseNumber = 100.0;
    const float getQVal (const float* q)const
    {
        return kLowestQVal * pow (kQBaseNumber, *q);
    }
    
    // input gain (Db)
    static constexpr float kGainDynamicRange = 48.0f; // decibel
    const float getGainDecibelVal (const float* gain)const
    {
        // gain range : - kGainDynamicRange / 2 [dB] ~ kGainDynamicRange [dB]
        return kGainDynamicRange * (*gain - 0.5f); // 0 dB equals to 0.5
    }
    
    // input gain (linear)
    const float getGainLinearVal (const float* gain) const
    {
        return pow (10.0f,getGainDecibelVal (gain)/20.f);
    }
    
    //==============================================================================
    void parameterChanged (const String& parameterID, float newValue) override;
    const StringArray FILTER_TYPES { "LPF", "HPF", "BPF" };
    
    std::atomic<bool> isGraphUpToDate;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BiquadLimiterAudioProcessor)
};
