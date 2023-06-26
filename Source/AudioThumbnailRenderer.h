/*
  ==============================================================================

    AudioThumbnailRenderer.h
    Created: 25 Jun 2023 7:23:05pm
    Author:  Antti

  ==============================================================================
*/

#pragma once

#include<JuceHeader.h>
#include"GainModule.h"

class AudioThumbnailRenderer
{
public:

    AudioThumbnailRenderer(const juce::AudioBuffer<float>& BufferToDraw, const GainModule& gainModule);
    ~AudioThumbnailRenderer();

    void drawBuffer(juce::Graphics& g, const juce::Rectangle<int>& bounds);

    void setSampleRate(const double newSampleRate);


private:

    void reDraw(juce::Graphics& g, const juce::Rectangle<int>& bounds);

    const juce::AudioBuffer<float>& m_bufferToDraw;
    const GainModule& m_gainModule;
    std::vector<float> m_samples;
    juce::Path m_path;

    double m_sampleRate;
};