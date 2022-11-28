/*
  ==============================================================================

    TransportEditor.h
    Created: 25 Nov 2022 2:15:23pm
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TransportPlayer.h"

class SoundTableModel;

class TransportEditor : public juce::Component, public juce::ChangeListener, private juce::Timer
{
    friend class MainComponent;
    friend class TransportPlayer;
    
public:
        
    TransportEditor(class SoundTableModel& tableModel, class MainComponent& mainApp, class TransportPlayer& player);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    void setFileToPlay(juce::File file);
    
    void paintLoadedFile(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    void paintRelocateFile(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    bool isFileValid(juce::File fileToTest);
    
    void playButtonClicked();
    
    void stopButtonClicked();
    
    void loopButtonClicked();
    
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    
    void openAudioSettings();
    
    void loadAudioFile(juce::File file);
    
    void changePlayToPause();
    
    void changePauseToPlay();
    
    
    
private:
    
    class SoundTableModel& m_tableModel;
    
    class MainComponent& m_mainApp;
    
    class TransportPlayer& m_player;
    
    juce::AudioThumbnailCache m_thumbnailCache;
    
    juce::AudioThumbnail m_thumbnail;
    ///The  audio format manager
    juce::AudioFormatManager m_formatManager;
    
    juce::AudioDeviceManager m_audioDeviceManager;
    
    std::unique_ptr<juce::AudioDeviceSelectorComponent> m_audioSettings;
    
    juce::DrawableButton m_playButton, m_stopButton, m_loopButton, m_relocateButton;
    
    juce::Label m_playheadPosition;
    
    bool m_fileSelected = false, m_fileIsValid = false, m_isLooping = false, m_canDragFile = true;

    juce::File m_fileToPlay;
    
    enum PlayState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };
        
    PlayState playState;
    
    void changePlayState (TransportEditor::PlayState newPlayState);
    
    void timerCallback() override;
    
};
