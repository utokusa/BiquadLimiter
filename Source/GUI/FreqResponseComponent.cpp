/*
  ==============================================================================

    FreqResponseComponent.cpp
    Created: 18 Aug 2019 7:56:10pm
    Author:  Utokusa

  ==============================================================================
*/

#include "FreqResponseComponent.h"

FrequencyResponseComponent::FrequencyResponseComponent(AudioProcessor& p
                                                       , AudioProcessorValueTreeState& vts
                                                       , BiquadLimiter& bl)
    : processor(p)
    , parameters(vts)
    , biauadLimiter(bl)
    , BorderThickness(2)
    , graphX(BorderThickness)
    , graphY(BorderThickness)
{
    graphWidth = getWidth() - 2 *  BorderThickness;
    graphHeight = getHeight() - 2 *  BorderThickness;
}

void FrequencyResponseComponent::paint(Graphics& g)
{
    graphWidth = getWidth() - 2 *  BorderThickness;
    graphHeight = getHeight() - 2 *  BorderThickness;
    
    //=============================================================================
    // draw tick lines
    tickLines.clear();
    // draw x-axis ticks from 30 Hz to 10,000 Hz on log scale.
    constexpr int minExp = 1;  //minimum exponent
    constexpr int maxExp = 4;  //maximum exponent
    constexpr int minMsd = 1;
    constexpr int maxMsd = 9;
    constexpr float tickMarginY = 13;  // make space for labels.
    for (int exponent = minExp; exponent <= maxExp; ++exponent) // exponent
    {
        for (int msd = minMsd; msd <= maxMsd; ++msd) // Most Significant Digit
        {
            if (1 == exponent && msd <= 2) // We don't draw under 20 Hz
            {
                continue;
            }
            else if (4 <= exponent && 2 <= msd) // We don't draw over 20,000 Hz
            {
                break;
            }
            float frequency = msd * std::pow(10.0f, exponent);
            float xPosition = graphWidth / 3.0f * std::log10(frequency / 20.0f) + graphX;
            tickLines.push_back(Line<float>(xPosition ,0, xPosition, graphHeight - tickMarginY));
        }
    }
    
    // Draw y-axis ticks from -18 dB to 18 dB on linear scale.
    constexpr int minMag = -18;  // mimimum magnitude
    constexpr int maxMag = 18;  // maximum magnitude
    constexpr int deltaMag = 6;  // delta magnitude
    constexpr float tickMarginX = 17;  // make space for labels.
    for (int magnitude = minMag; magnitude <= maxMag; magnitude += deltaMag)
    {
        float yPosition = graphY + graphHeight / 2.0f - static_cast<float>(magnitude) / 24.0f * (graphHeight / 2.0f);
        
        tickLines.push_back(Line<float>(graphX, yPosition, graphWidth - tickMarginX, yPosition));
    }
    
    // Draw to Graphic g.
    const Colour tickLineColour = Colour(0x60F3F1F1);
    g.setColour (tickLineColour);
    for (auto line : tickLines)
    {
        g.drawLine (line, 1.0f);
    }
    
    //=============================================================================
    // Draw tick label numbers
    
    // X-axis
    using flags = Justification::Flags;
    constexpr int xAxisLabelY = 145;
    int xAxisLabelX = 45;
    constexpr int deltaX = 80;
    constexpr int xAxisLabelWidth = 50;
    constexpr int xAxisLabelHeight = 30;
    
    g.drawText("100", xAxisLabelX, xAxisLabelY, xAxisLabelWidth, xAxisLabelHeight, flags::left);
    xAxisLabelX += deltaX;
    g.drawText("1k", xAxisLabelX, xAxisLabelY, xAxisLabelWidth, xAxisLabelHeight, flags::left);
    xAxisLabelX += deltaX;
    g.drawText("10k", xAxisLabelX, xAxisLabelY, xAxisLabelWidth, xAxisLabelHeight, flags::left);
    
    // Y-axis
    constexpr int yAxisLabelX = 212;
    int yAxisLabelY = 28;
    constexpr int deltaY = 42;
    constexpr int yAxisLabelWidth = 20;
    constexpr int yAxisLabelHeight = 30;
    g.drawText("12", yAxisLabelX, yAxisLabelY, yAxisLabelWidth, yAxisLabelHeight, flags::right);
    yAxisLabelY += deltaY;
    g.drawText("0", yAxisLabelX, yAxisLabelY, yAxisLabelWidth, yAxisLabelHeight, flags::right);
    
    //=============================================================================
    // Draw frequency responce
    // Create frequency responce path.
    createGraphPath(g);
    
    // Fill the graph path with a gradient color.
    ColourGradient gradient(Colour(0x40FFFFFF), Point<float>(0, graphY), Colour(0x00FFFFFF), Point<float>(0, (graphY + graphHeight)), false);
    g.setGradientFill(gradient);
    g.fillPath(filter_response_path_);
    
    // Stroke the graph path.
    g.setColour (Colour(0xF0F5F5F5));
    float line_width = 2.5f;
    PathStrokeType stroke(line_width, PathStrokeType::beveled, PathStrokeType::rounded);
    g.strokePath(filter_response_path_, stroke);
    
    // Draw graph frame using a image.
    Image frame = ImageCache::getFromMemory(BinaryData::FreqResponce_4x_png, BinaryData::FreqResponce_4x_pngSize);
    Rectangle<float> frameArea(0, 0, frameWidth, frameHeight);
    g.drawImage(frame, frameArea, RectanglePlacement::Flags::xLeft, false);
    
}

float FrequencyResponseComponent::getParamValue(String s)
{
    auto pParam = parameters.getRawParameterValue(s);
    return pParam->load();
}

float FrequencyResponseComponent::getSmoothedParamValue(int index)
{
    auto coef = biauadLimiter.getCoefVal(index);
    return coef;
}

void FrequencyResponseComponent::createGraphPath(Graphics& g)
{
    float sampleRate = static_cast<float>(processor.getSampleRate());
    sampleRate = DspCommon::defaultSampleRateIfOutOfRange(sampleRate);
    
    cp a0(getParamValue("a00"), 0.0f);
    cp a1(getParamValue("a01"), 0.0f);
    cp a2(getParamValue("a02"), 0.0f);
    cp b0(getParamValue("b00"), 0.0f);
    cp b1(getParamValue("b01"), 0.0f);
    cp b2(getParamValue("b02"), 0.0f);
    
    
    graphWidth = getWidth() - 2 *  BorderThickness;
    graphHeight = getHeight() - 2 *  BorderThickness;
    
    float val = 0;
    float freq = 20.0f / sampleRate; // Normalized frequency
    const float offsetY = graphY + graphHeight / 2.0f;
    constexpr float maxGain = 24.0f;
    const float dbToPixel = graphHeight / 2.0f / maxGain;  // Constant float to convert decibel gain to pixel.
    
    float prev_val = offsetY - dbToPixel * calcGain(freq, a0, a1, a2, b0, b1, b2);
    if (!isfinite(prev_val))
    {
        prev_val = offsetY;
    }
    
    filter_response_path_.clear();
    filter_response_path_.startNewSubPath(graphX, prev_val);
    auto first_val = prev_val;
    
    for (int i=0; i<=graphWidth; ++i)
    {
        freq = 20.0f * pow(1000.0f, static_cast<float>(i) / static_cast<float>(graphWidth)) / sampleRate;
        val = offsetY - dbToPixel * calcGain(freq, a0, a1, a2, b0, b1, b2);
        if (isfinite(val)) // Equals to `if (!isnan(val) && !isinf(val))`.
        {
//            filter_response_path_.lineTo(GRAPH_X_POINT + i + 1, val);
            filter_response_path_.lineTo(graphX + i, val);
            prev_val = val;
        }
        
    }
    
    constexpr float margin = 5.0f;
    filter_response_path_ = filter_response_path_.createPathWithRoundedCorners(10.0f);
    filter_response_path_.lineTo(graphX + graphWidth + margin, prev_val);
    filter_response_path_.lineTo(graphX + graphWidth + margin, graphHeight + margin);
    filter_response_path_.lineTo(graphX - margin, graphHeight + margin);
    filter_response_path_.lineTo(graphX - margin, first_val);
    filter_response_path_.closeSubPath ();
    
}

float FrequencyResponseComponent::calcGain(float freq, cp a0, cp a1, cp a2, cp b0, cp b1, cp b2)  // cp is an alias for complex.
{
    // cp is an alias for complex.
    cp exp_phase1(0.0f, -2.0f * DspCommon::PI * freq);
    cp exp_phase2(0.0f, -4.0f * DspCommon::PI * freq);
    cp  numerator = b0 + std::exp(exp_phase1) * b1 + std::exp(exp_phase2) * b2;
    cp  denominator = a0 + std::exp(exp_phase1) * a1 + std::exp(exp_phase2) * a2;
    
    // This function may return NAN or INF.
    return 20.0f * std::log10 (std::abs(numerator / denominator));
}

