/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>
#include <limits>

//==============================================================================
BiquadLimiterAudioProcessor::BiquadLimiterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  ),
#endif
  parameters(*this, nullptr)
, defaultCoefficient {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}
, biquadLimiter(defaultCoefficient, maxNumChannels)
, coefIds {"a00", "a01", "a02", "b00", "b01", "b02"}
, coefNames {"a0", "a1", "a2", "b0", "b1", "b2"}
, coefValPtrs {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
, isGraphUpToDate(true)
{
    //==============================================================================
    // parameter value convertion from [0, 1.0] float to physical quantity juce::String.
    // biquad limiter coefficients
    constexpr int kCoefNumDecimal = 4;
    auto valueToTextFunction = [](float value) { return String(value, kCoefNumDecimal); };
    
    // frequency
    auto valueToFreqFunction = [](float value)
    { return String((int)(pow(kFreqBaseNumber, value) * kLowestFreqVal)) + String(" Hz"); };
    
    // q
    constexpr int kQNumDecimal = 2;
    auto valueToQFunction = [](float value)
    { return String(pow(kQBaseNumber, value) * kLowestQVal, kQNumDecimal); };
    
    // input gain
    constexpr int kGainNumDecimal = 1;
    auto valueToGainFunction = [](float value)
    { return String((kGainDynamicRange * (value - 0.5f)), kGainNumDecimal) + String(" dB"); };
    
    //==============================================================================
    // set audio parameters
    using Parameter = AudioProcessorValueTreeState::Parameter;
    using NRange = NormalisableRange<float>;
    NRange nrange1 (0.0001f, 2.0f,  0.f);
    NRange nrange2 (-2.0f, 2.0f,  0.f);
    std::vector<NRange> normalisableRanges {nrange1, nrange2, nrange2, nrange2, nrange2, nrange2};
    for (int i = 0; i < Bf::numCoef; ++i)
    {
        parameters.createAndAddParameter (std::make_unique<Parameter> (coefIds[i], coefNames[i], ""
                                                                       , normalisableRanges[i]
                                                                       , defaultCoefficient[i]
                                                                       ,  valueToTextFunction
                                                                       ,  nullptr
                                                                       , true));
        coefValPtrs[i] = parameters.getRawParameterValue (coefIds[i]);
        parameters.addParameterListener (coefIds[i], this);
    }
    
    NRange nrange3 (0.0f, 1.0f,  0.0f);
    parameters.createAndAddParameter(std::make_unique<Parameter> ("frequency", "Freq", "", nrange3, 1.0f
                                                                  ,  valueToFreqFunction,  nullptr, true));
    parameters.createAndAddParameter(std::make_unique<Parameter> ("q", "Q", "", nrange3, 0.0f
                                                                  ,  valueToQFunction,  nullptr, true));
    parameters.createAndAddParameter(std::make_unique<Parameter> ("gain", "In", "", nrange3, 0.5f
                                                                  , valueToGainFunction, nullptr));
    parameters.createAndAddParameter(std::make_unique<AudioParameterChoice> ("filter_type", "Type", FILTER_TYPES, 0));
    freq = parameters.getRawParameterValue("frequency");
    q = parameters.getRawParameterValue("q");
    gain = parameters.getRawParameterValue("gain");
    
    parameters.addParameterListener ("frequency", this);
    parameters.addParameterListener ("q", this);
    parameters.addParameterListener ("filter_type", this);
    
    parameters.state = ValueTree(Identifier("BiquadLimiter"));
    
}

BiquadLimiterAudioProcessor::~BiquadLimiterAudioProcessor()
{
}

//==============================================================================
const String BiquadLimiterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BiquadLimiterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool BiquadLimiterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool BiquadLimiterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double BiquadLimiterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BiquadLimiterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int BiquadLimiterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BiquadLimiterAudioProcessor::setCurrentProgram (int index)
{
}

const String BiquadLimiterAudioProcessor::getProgramName (int index)
{
    return {};
}

void BiquadLimiterAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BiquadLimiterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Pre-playback initialisation.
    previousGain = getGainLinearVal (gain);
    
    spec.sampleRate = DspCommon::defaultSampleRateIfOutOfRange (sampleRate);
    spec.numChannels = maxNumChannels;
    spec.maximumBlockSize = samplesPerBlock;
    isGraphUpToDate.store (false);
    
    clipper.prepare (spec);
    clipper.functionToUse = clippingFunctionNormal;
    
    
}

void BiquadLimiterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BiquadLimiterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void BiquadLimiterAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    
    auto currentGain = getGainLinearVal(gain);
    
    if (currentGain == previousGain)
    {
        buffer.applyGain (currentGain);
    }
    else
    {
        buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);
        previousGain = currentGain;
    }
    
    std::array<float, Bf::numCoef> tmpCoef;
    for (int i = 0; i < Bf::numCoef; ++i) tmpCoef[i] = *(coefValPtrs[i]);
    
    biquadLimiter.setCoefficient (tmpCoef, spec.sampleRate);
    
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        if (channel < maxNumChannels)
        {
            biquadLimiter.DoProcess(buffer.getWritePointer(channel), buffer.getNumSamples(), channel, spec.sampleRate);
        }
    }
    
    biquadLimiter.UpdateWorkingState();
    
    if (buffer.getNumSamples() > 0)
    {
        // wrap input buffer
        dsp::AudioBlock<float> audioBlock(buffer);
        dsp::ProcessContextReplacing<float> context(audioBlock);
        clipper.process(context);
    }
}

//==============================================================================
bool BiquadLimiterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BiquadLimiterAudioProcessor::createEditor()
{
    return new BiquadLimiterAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void BiquadLimiterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);

}

void BiquadLimiterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
     
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
        {
            for (int i = 0; i < 2; ++i)
            {
                // We want to prevent Biquad Filter Coefficient
                // from being rewritten by setBiquadFilterCoef() in parameterChanged().
                // So we assign ValueTree::fromXml(*xmlState) to parameters.state twice.
                // In the second assignment to parameters.state,
                // parameterChanged () is never called.
                parameters.replaceState (ValueTree::fromXml (*xmlState));
            }
        }
    
}


void BiquadLimiterAudioProcessor::setBiquadFilterCoef()
{
    if (spec.sampleRate > DspCommon::minSampleRate && spec.sampleRate < DspCommon::maxSampleRate)
    {
        auto targetFreq = getFreqVal(freq);
        if (targetFreq > spec.sampleRate / 2.0f)
        {
            targetFreq = spec.sampleRate / 2.0f;
        }
        auto targetQ = getQVal(q);
        using Ptr = ReferenceCountedObjectPtr<dsp::IIR::Coefficients<float>>;
        String ChoiceName = (dynamic_cast<AudioParameterChoice*>(parameters.getParameter("filter_type")))->getCurrentChoiceName();
        Ptr pCoe;
        
        if (ChoiceName == "LPF")
        {
            pCoe = *dsp::IIR::Coefficients<float>::makeLowPass(spec.sampleRate, targetFreq, targetQ);
        }
        else if (ChoiceName == "HPF")
        {
            pCoe = *dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, targetFreq, targetQ);
        }
        else if (ChoiceName == "BPF")
        {
            pCoe = *dsp::IIR::Coefficients<float>::makeBandPass(spec.sampleRate, targetFreq, targetQ);
        }
        
        std::array<float, Bf::numCoef>  currentCoefVals {1.0f, pCoe->coefficients[3], pCoe->coefficients[4]
            , pCoe->coefficients[0], pCoe->coefficients[1], pCoe->coefficients[2]};
        
        for (int i = 0; i < Bf::numCoef; ++i)
        {
            // https://docs.juce.com/master/classAudioProcessorParameter.html#ac9b67f35339db50d2bd9a026d89390e1
            // https://forum.juce.com/t/au-parameter-automation-issue-togglebutton-does-not-write/34277/47
            // This time we seem not to have to use neither beginChangeGesture and endChangeGesture.
            auto param = parameters.getParameter(coefIds[i]);
//            param->beginChangeGesture();
            param->setValueNotifyingHost (param->convertTo0to1(currentCoefVals[i]));
//            param->endChangeGesture();
        }
        
    }
}


void BiquadLimiterAudioProcessor::parameterChanged (const String& parameterID, float newValue)
{
    if (parameterID == "frequency" || parameterID == "q" || parameterID == "filter_type")
    {
        setBiquadFilterCoef();
    }
    isGraphUpToDate.store (false);
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BiquadLimiterAudioProcessor();
}
