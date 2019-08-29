# BiquadLimiter

BiquadLimiter is a biquad filter plug-in whitch has a dynamic range limiter in front of the feedback section (Limiter1) and an another limiter in the final section (Limiter2). This plug-in is made with C++  and JUCE. 

## Features

- You can see the six coefficients (a0, ..., b2) of the biquad filter that determine the behavior of the filter. Changing either Frequency / Q / Type will move the coefficient sliders. ([See this link for information on biquad filter coefficients.](https://www.w3.org/2011/audio/audio-eq-cookbook.html))
- The coefficient values a0, ..., b2 can be set individually. This feature gives you a frequency response that is difficult to achieve with a regular 1-channel filter plug-in.
- The two limiters prevent the output from becoming too loud when the filter is unstable or the input is too loud. In addition, the front limiter generates a unique sound by feeding the compressed sound back to the biquad filter.

## Usage

BiquadLimiter can run inside plugin hosts that supports VST, VST3, or AU.

## Parameters and display items

**In** : The input gain before the filter unit. 

**Freq**: The cutoff frequency of biquad filter. 

**Q**:  The q of biquad filter. 

**Type** : This is a combo box to choose filter type. You can select one from LPF (low-pass), BPF (band-pass) and HPF (high-pass). 

**a0, ..., b2** : The coefficients of the biquad filter.

**Fine** : The button to turn on / off the fine adjustment mode of coefficient slider.

**Limiter LED** : This LED indicates whether the limiter is working.

**Frequency display** : The graph shows the ideal filter frequency response without the effect of the limiter. 

## Building

This project can be compiled by running the BiquadLimiter.jucer file.

You need following libraries.

- **JUCE**  (v5.4.3)
- **VST SDK** (v3.6.10) 

If you want build VST (not VST3), you need to use VST SDK v3.6.10 or earlier.