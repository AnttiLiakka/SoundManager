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
    //Something Something, Shutdown Audio, Something
    m_editor.m_audioDeviceManager.removeAudioCallback(&m_player);
    m_editor.m_audioDeviceManager.closeAudioDevice();
}

void TransportPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    m_sampleRate = sampleRate;
}


void TransportPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    if(!m_playing) return;
    
    auto numSamples = bufferToFill.numSamples;
    float* leftChannel = bufferToFill.buffer->getWritePointer(0);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1);
    
    for(int i = 0; i < numSamples; ++i)
    {
        leftChannel[i] = m_buffer.getSample(0, m_playPosition) * m_volume;
        rightChannel[i] = m_buffer.getSample(1, m_playPosition) * m_volume;
        
        ++m_playPosition;
        m_playPosSeconds = m_playPosition / m_sampleRate;
        
        if(m_playPosition >= m_endPosition.load() || m_playPosition >= m_buffer.getNumSamples())
        {
            m_playPosition = 0;
            sendChangeMessage();
        }
    }

}

void TransportPlayer::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
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
            m_volume = volume;
        }
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
