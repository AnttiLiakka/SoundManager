/*
  ==============================================================================

    TransportEditor.h
    Created: 25 Nov 2022 2:15:23pm
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SoundTableModel;

class TransportEditor : juce::Component
{
    friend class MainComponent;
    
public:
    
    TransportEditor(class SoundTableModel& tableModel);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setFileToPlay(juce::File file);
    
    void paintLoadedFile(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    
private:
    
    SoundTableModel& m_tableModel;
    
    juce::AudioThumbnailCache m_thumbnailCache;
    
    juce::AudioThumbnail m_thumbnail;
    ///The audiobuffer
    juce::AudioBuffer<float> m_sampleBuffer;
    ///The  audio format manager
    juce::AudioFormatManager m_formatManager;
    
    juce::DrawableButton m_playButton, m_pauseButton, m_stopButton;
    
    bool m_fileSelected = false, m_fileIsValid = false, m_shouldBePainting = false;
    
    juce::File m_fileToPlay;
    
};
