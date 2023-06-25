/*
  ==============================================================================

    TransportProcessor.cpp
    Created: 28 Nov 2022 9:39:33am
    Author:  Antti

  ==============================================================================
*/

#include "TransportPlayer.h"


TransportPlayer::TransportPlayer(class TransportEditor& editor) : m_editor(editor)
{
    m_player.setSource(this);
    m_editor.m_audioDeviceManager.addAudioCallback(&m_player);
    addChangeListener(&m_editor);
}

TransportPlayer::~TransportPlayer()
{
    //Shutting down audio by clearing the buffer, removing the player and closing the audio device
    m_buffer.clear();
    m_editor.m_audioDeviceManager.removeAudioCallback(&m_player);
    m_editor.m_audioDeviceManager.closeAudioDevice();
}

void TransportPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    m_sampleRate = sampleRate;
}


void TransportPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if(!m_playing) return;



    auto numSamples = bufferToFill.numSamples;
    float* leftChannel = bufferToFill.buffer->getWritePointer(0);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1);

    if (m_buffer.getNumChannels() == 2)
    {

        for (int i = 0; i < numSamples; ++i)
        {
            auto leftSample = m_gainModule.processSample(m_buffer.getSample(0, m_playPosition));

            auto RightSample = m_gainModule.processSample(m_buffer.getSample(1, m_playPosition));

            leftChannel[i] = leftSample;
            rightChannel[i] = RightSample;

            ++m_playPosition;
            m_playPosSeconds.store((float)m_playPosition / m_sampleRate);

            if (m_playPosition >= m_endPosition.load() || m_playPosition >= m_buffer.getNumSamples())
            {
                m_playPosition = 0;
                sendChangeMessage();
            }
        }
    }
    else if (m_buffer.getNumChannels() == 1)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            auto leftSample = m_gainModule.processSample(m_buffer.getSample(0, m_playPosition));

            leftChannel[i] = leftSample;
            rightChannel[i] = leftSample;

            ++m_playPosition;
            m_playPosSeconds.store((float)m_playPosition / m_sampleRate);

            if (m_playPosition >= m_endPosition.load() || m_playPosition >= m_buffer.getNumSamples())
            {
                m_playPosition = 0;
                sendChangeMessage();
            }
        }
    }
}

void TransportPlayer::releaseResources()
{
    
}

void TransportPlayer::prepForNewFile()
{
    m_playing = false;
    m_buffer.clear();
    m_playPosition = 0;
    m_playPosSeconds = 0;
}

void TransportPlayer::startPlayback()
{
    m_playing = true;
}

void TransportPlayer::pausePlayback()
{
    m_playing = false;
}

void TransportPlayer::stopPlayback()
{
    m_playing = false;
    m_playPosSeconds = 0;
    m_playPosition = 0;
}

void TransportPlayer::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &m_editor.m_volumeSlider)
    {
       auto volume = slider->getValue();
        if(volume <= 1)
        {
            m_gainModule.SetGain(volume);
        }
        m_editor.refreshThumbnail(true);
    }
}

void TransportPlayer::setPlayPosition(int newPosition)
{
    m_playPosition = newPosition;
}

void TransportPlayer::setEndPosition(int newEndPosition)
{
    m_endPosition = newEndPosition;
}
