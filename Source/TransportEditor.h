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
///This class is the component user interacts with to controls the TransportPlayer.
class TransportEditor : public juce::Component, public juce::ChangeListener, private juce::Timer /*public juce::ApplicationCommandTarget*/
{
    friend class MainComponent;
    friend class TransportPlayer;
    friend class DragAndDropTable;
    
public:
    ///The constructor, takes in references to the  SoundTableModel, MainComponent and TransportPlayer.
    TransportEditor(class SoundTableModel& tableModel, class MainComponent& mainApp, class TransportPlayer& player);
    ~TransportEditor();
    ///This virtual function is inherited from Juce Component and it is overridden to paint the waveform and the playhead or any error texts.
    void paint(juce::Graphics& g) override;
    ///This virtual functon is inherted from Juce Component and it is overridden to place all buttons, sliders etc. onto the component.
    void resized() override;
    ///This pure virtual function is inherited from Juce changeListener and it is overriden to listen when the TransportPlayer has reached the end of the playback and when that happens, this class tells it to stop playback.
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    //juce::ApplicationCommandTarget* getNextCommandTarget() override;
    
    //void getAllCommands(juce::Array<juce::CommandID> &commands) override;
    
    //void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    
    //bool perform (const juce::ApplicationCommandTarget::InvocationInfo &info) override;
    ///This function is called by the SoundTableModel and it is used to load to selected audiofile into the TransportPlayers audiobuffer.
    void setFileToPlay(juce::File file);
    ///This function is called by the paint function and it is used to paint the waveform and playhead of the selected file.
    void paintLoadedFile(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    ///This function is called by the paint function and it is used to paint the "File Not Found" text instead of the waveform.
    void paintRelocateFile(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    ///This function is called by the paint function and it is used to paint an area user has selected via mouseDrag.
    void paintSelection(juce::Graphics& g, int startPos, int endPos);
    ///This function removes the selected area.
    void resetSelection();
    ///This function is called to set the TransportPlayers playposition and endposition based on the area user has selected.
    void setSelectionPlay();
    ///This function returns true if the audiofile selected by the user is valid. It determines whether paintLoadedFile or paintRelocateFile is called by the paint function.
    bool isFileValid(juce::File fileToTest);
    ///This function is called when the playbutton is clicked.
    void playButtonClicked();
    ///This function is called when the stop button is clicked.
    void stopButtonClicked();
    ///This function is called when the loop button is clicked.
    void loopButtonClicked();
    ///Virtual function inherited from Juce Component. This function is called when the component is clicked with a mouse, it is overridden to control the location of the playhead based on mouseclick location.
    void mouseDown(const juce::MouseEvent& event) override;
    ///Virtual function inherted from Juce Component. This function is called when the component is dragged with a mouse. This function is overridden to control which section of the soundfile is played and drag export.
    void mouseDrag(const juce::MouseEvent& event) override;
    
    void mouseUp(const juce::MouseEvent& event) override;
    ///This function is called to open the audiosettings.
    void openAudioSettings();
    ///This function is called if isFileValid returns true and it loads the audiofile into the TransportPlayers audiobuffer.
    void loadAudioFile(juce::File file);
    ///This function chages the playbutton to pausebutton.
    void changePlayToPause();
    ///This function changes the pausebutton to playbutton.
    void changePauseToPlay();
    ///This function is called when the user deselects all rows, for example by clicking the table background. it clears the audio thumbnail and makes sure that the locate button is hidden.
    void noFileSelected();
    
    void prepSelectionBuffer();
    
    void createFileFromSelection();
    
    enum CommandIDs
    {
        Looping = 0,
        PlaybuttonActive = 0,
        StopbuttonActive = 0
    };
    
private:
    ///Reference to the SoundTableModel.
    class SoundTableModel& m_tableModel;
    ///Reference to the MainComponent.
    class MainComponent& m_mainApp;
    ///Reference to the TransportPlayer.
    class TransportPlayer& m_player;
    ///This member is need to initialize m_thumbnail and it is used to manage multiple audiothumbnail objects.
    juce::AudioThumbnailCache m_thumbnailCache;
    ///This member paints the waveform of the selected audiofile.
    juce::AudioThumbnail m_thumbnail;
    ///The  audio format manager.
    juce::AudioFormatManager m_formatManager;
    ///The audio device manager.
    juce::AudioDeviceManager m_audioDeviceManager;
    ///The audio device selector component
    std::unique_ptr<juce::AudioDeviceSelectorComponent> m_audioSettings;
    ///The playbutton, stopbutton, loopbutton and relocatebutton
    juce::DrawableButton m_playButton, m_stopButton, m_loopButton, m_relocateButton;
    ///This label displays the current playhead position in seconds
    juce::Label m_playheadPosition;
    ///Volume slider that the user can interact with to control the volume of the playback
    juce::Slider m_volumeSlider;
    ///Various booleans used to control what is drawn by the paint function, prevent exported files to be dropped back into the application and control section play functionality
    bool m_fileSelected = false, m_fileIsValid = false, m_canDragFile = true, m_sectionSelected = false, m_sectionPlayActive = false;
    ///Various integers that represent the size of the audio buffer and which section of the sound file has been selected
    int m_numBufferSamples, m_mouseDragStartPos, m_mouseDragDistance, m_mouseDragEndPos;
    ///File that has been selected for playback
    juce::File m_fileToPlay;
    ///Enumerator representing the different states of playback
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
    
    juce::File m_tempFile;
    
    juce::AudioBuffer<float> m_selectionBuffer;
    ///This funtion is used to change the state of playback
    void changePlayState (TransportEditor::PlayState newPlayState);
    ///This virtual function is inherited from Juce Timer and it is overridden to repaint the component at regular intervals. This makes sure that the playhead is drawn into a correct position.
    void timerCallback() override;
    
};
