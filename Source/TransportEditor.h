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

class TransportEditor : public juce::Component, public juce::ChangeListener, private juce::Timer
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
    
    void paintRelocateFile(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    bool isFileValid(juce::File fileToTest);
    
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    
private:
    
    class SoundTableModel& m_tableModel;
    
    juce::AudioThumbnailCache m_thumbnailCache;
    
    juce::AudioThumbnail m_thumbnail;
    ///The  audio format manager
    juce::AudioFormatManager m_formatManager;
    
    juce::DrawableButton m_playButton, m_stopButton, m_loopButton, m_relocateButton;
    
    bool m_fileSelected = false, m_fileIsValid = false, m_isLooping = false, m_canDragFile = true;

    juce::File m_fileToPlay;
    
    PlayState playState;
    
    void timerCallback() override;
};
