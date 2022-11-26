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

class TransportEditor : public juce::Component, public juce::ChangeListener
{
    friend class MainComponent;
    
public:
    
    enum PlayState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };
    
    TransportEditor(class SoundTableModel& tableModel);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    void setFileToPlay(juce::File file);
    
    void paintLoadedFile(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    void changePlayState (TransportEditor::PlayState newPlayState);
    
    void relocateFile(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    bool isFileValid(juce::File fileToTest);
    
private:
    
    class SoundTableModel& m_tableModel;
    
    juce::AudioThumbnailCache m_thumbnailCache;
    
    juce::AudioThumbnail m_thumbnail;
    ///The  audio format manager
    juce::AudioFormatManager m_formatManager;
    
    juce::DrawableButton m_playButton, m_pauseButton, m_stopButton;
    
    juce::TextButton m_relocateButton;
    
    bool m_fileSelected = false, m_fileIsValid = false;

    juce::File m_fileToPlay;
    
    PlayState playState;
    
    
};
