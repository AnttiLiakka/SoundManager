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

class TransportPlayer : public juce::AudioSource, public juce::ChangeBroadcaster, public juce::Slider::Listener
{
    friend class TransportEditor;
public:
    
    TransportPlayer(class TransportEditor& editor);
    
    ~TransportPlayer();
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void releaseResources() override;
    
    void sliderValueChanged(juce::Slider *slider) override;
    
    void playTestSound();
    
    void playBuffer();
    
    void prepForNewFile();
    
    void startPlayback();
    
    void pausePlayback();
    
    void stopPlayback();
    
    void setPlayPosition(int newPosition);
    
    void setEndPosition(int newEndPosition);
private:
    
    class TransportEditor& m_editor;
    
    juce::AudioBuffer<float> m_buffer;
    
    //juce::SoundPlayer m_player;
    
    juce::AudioSourcePlayer m_player;
    
    double m_volume = 0.5;
    
    std::atomic<int> m_playPosition { 0 };
    double m_sampleRate;
    
    std::atomic<float> m_playPosSeconds { 0 };
    
    std::atomic<bool> m_playing { false };
    
    std::atomic<bool> m_isLooping { false };
    
    std::atomic<int> m_endPosition { 0 };
    
};






