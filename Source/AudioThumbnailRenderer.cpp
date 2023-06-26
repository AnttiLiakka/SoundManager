/*
  ==============================================================================

    AudioThumbnailRenderer.cpp
    Created: 25 Jun 2023 7:23:05pm
    Author:  antti

  ==============================================================================
*/

#include "AudioThumbnailRenderer.h"

AudioThumbnailRenderer::AudioThumbnailRenderer(const juce::AudioBuffer<float>& BufferToDraw, const GainModule& gainModule):
                                                                                              m_bufferToDraw(BufferToDraw),
                                                                                              m_sampleRate(44000),
                                                                                              m_gainModule(gainModule)
{

}

AudioThumbnailRenderer::~AudioThumbnailRenderer()
{
  
}

void AudioThumbnailRenderer::drawBuffer(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    reDraw(g, bounds);
}

void AudioThumbnailRenderer::setSampleRate(const double newSampleRate)
{
    m_sampleRate = newSampleRate;
}

void AudioThumbnailRenderer::reDraw(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    m_path.clear();
    m_samples.clear();

    auto readPointerLeft = m_bufferToDraw.getReadPointer(0);
    auto drawRatio = m_bufferToDraw.getNumSamples() / bounds.getWidth();

    // Mono Scenario
    if (m_bufferToDraw.getNumChannels() == 1)
    {
        for (int n = 0; n < m_bufferToDraw.getNumSamples(); n += drawRatio)
        {
            m_samples.push_back(readPointerLeft[n]);
        }

        m_path.startNewSubPath(0, bounds.getHeight() / 2);

        for (int i = 0; i < m_samples.size(); ++i)
        {
            float sample = m_gainModule.processSample(m_samples[i]);
            auto point = juce::jmap<float>(sample, -1.0f, 1.0f, bounds.getHeight(), 0);
            m_path.lineTo(i, point);
        }
    }
    // Stereo Scenario
    else
    {
        auto midPoint = bounds.getHeight() / 2;
        //Left channel first

        for (int n = 0; n < m_bufferToDraw.getNumSamples(); n += drawRatio)
        {
            m_samples.push_back(readPointerLeft[n]);
        }

        m_path.startNewSubPath(0, midPoint / 2);

        for (int i = 0; i < m_samples.size(); ++i)
        {
            float leftSample = m_gainModule.processSample(m_samples[i]);
            auto leftpoint = juce::jmap<float>(leftSample, -1.0f, 1.0f, bounds.getHeight() / 2, 0);
            m_path.lineTo(i, leftpoint);
        }



        //Right channel
        m_samples.clear();
        auto readPointerRight = m_bufferToDraw.getReadPointer(1);


        for (int i = 0; i < m_bufferToDraw.getNumSamples(); i += drawRatio)
        {
            m_samples.push_back(readPointerRight[i]);
        }

        m_path.startNewSubPath(0, midPoint + midPoint / 2);

        for (int i = 0; i < m_samples.size(); ++i)
        {
            auto rightSample = m_gainModule.processSample(m_samples[i]);
            auto rightPoint = juce::jmap<float>(rightSample, -1.0f, 1.0f, bounds.getHeight(), bounds.getHeight() / 2);
            m_path.lineTo(i, rightPoint);
        }
    }

    g.strokePath(m_path, juce::PathStrokeType(1));
}
