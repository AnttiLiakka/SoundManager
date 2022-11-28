/*
  ==============================================================================

    TransportProcessor.h
    Created: 28 Nov 2022 9:39:33am
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TransportEditor.h"

class TransportPlayer : public juce::AudioSource, public juce::ChangeBroadcaster
{
    friend class TransportEditor;
public:
    
    TransportPlayer(class TransportEditor& editor);
    
    ~TransportPlayer();
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void releaseResources() override;
    
    void playTestSound();
    
    void playBuffer();
    
    void prepForNewFile();
    
    void startPlayback();
    
    void pausePlayback();
    
    void stopPlayback();
private:
    
    class TransportEditor& m_editor;
    
    juce::AudioBuffer<float> m_buffer;
    
    //juce::SoundPlayer m_player;
    
    juce::AudioSourcePlayer m_player;
    
    int m_playPosition = 0;
    
    double m_sampleRate;
    
    float m_playPosSeconds = 0;
    
    bool m_playing = false;
    
};






