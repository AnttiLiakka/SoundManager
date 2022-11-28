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
    //m_deviceManager.addAudioCallback(&m_player);
    m_editor.m_audioDeviceManager.addAudioCallback(&m_player);
    addChangeListener(&m_editor);
}

TransportPlayer::~TransportPlayer()
{
    //Something Something, Shutdown Audio, Something
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
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    
    
    
    bufferToFill.clearActiveBufferRegion();
    if(!m_playing) return;
    
    auto numSamples = bufferToFill.numSamples;
    float* leftChannel = bufferToFill.buffer->getWritePointer(0);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1);
    
    for(int i = 0; i < numSamples; ++i)
    {
        leftChannel[i] = m_buffer.getSample(0, m_playPosition);
        rightChannel[i] = m_buffer.getSample(1, m_playPosition);
        
        ++m_playPosition;
        m_playPosSeconds = m_playPosition / m_sampleRate;
        
        if(m_playPosition >= m_buffer.getNumSamples())
        {
            sendChangeMessage();
            m_playPosition = 0;
        }
    }

}

void TransportPlayer::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

void TransportPlayer::playTestSound()
{
    //m_player.playTestSound();
}

void TransportPlayer::playBuffer()
{
    //m_player.play(&m_buffer);
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
    m_playPosition = 0;
    m_playPosSeconds = 0;
}
