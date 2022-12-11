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
///This class is the component user interacts with to control the TransportPlayer.
class TransportEditor : public juce::Component, public juce::ChangeListener, private juce::Timer,                       public juce::ApplicationCommandTarget
{
    friend class MainComponent;
    friend class TransportPlayer;
    friend class DragAndDropTable;
    
    struct BufferingAlertWindow : public juce::ThreadWithProgressWindow
    {
        BufferingAlertWindow(juce::AudioBuffer<float> buffer) :
                          juce::ThreadWithProgressWindow(TRANS("Reading file into a buffer....."), true, true),
                          m_audioBuffer(buffer),
                          m_owner(nullptr)
        {
            m_formatManager.registerBasicFormats();
            setStatusMessage(TRANS("This is a very long audio file!"));
        }
        
        void run() override
        {
            auto fileToRead = m_owner->m_fileToPlay;
            std::unique_ptr<juce::AudioFormatReader> reader (m_formatManager.createReaderFor(fileToRead));
            
            int length = static_cast<int>(reader->lengthInSamples);
            int blockSize = 512;
            int numBlocks = length / blockSize;
            m_audioBuffer.clear();
            m_audioBuffer.setSize(2, length);
            int samplesRead = 0;
         
            for(int i = 0; i < numBlocks; ++i)
            {
                if (threadShouldExit()) break;
                
                setProgress(i / (double) numBlocks);
                int startPosition = blockSize * i;
                
                if(i != numBlocks - 1)
                {
                    reader->read(&m_audioBuffer, startPosition, blockSize, startPosition, true, true);
                    samplesRead += blockSize;
                } else
                {
                    reader->read(&m_audioBuffer, startPosition, length - samplesRead, startPosition, true, true);
                }
            }
            
            setProgress (-1.0);
            setStatusMessage (TRANS("Phew, Almost there!"));
            wait (1500);
        }
        
        void threadComplete (bool userPressedCancel) override
        {
            if (userPressedCancel)
            {
                m_owner->noFileSelected();
            }
            m_owner->setBuffer(m_audioBuffer);
        }
        
        void setOwner (TransportEditor* newOwner)
        {
            m_owner = newOwner;
        }
        
        //juce::AudioFormatReader* m_reader;
        juce::AudioBuffer<float> m_audioBuffer;
        juce::AudioFormatManager m_formatManager;
        TransportEditor* m_owner;
    };
    
    struct ExportAlertWindow : public juce::ThreadWithProgressWindow
    {
      ExportAlertWindow(TransportEditor& owner, double sampleRate)
                                            : juce::ThreadWithProgressWindow(TRANS("Creating file...."), true, false),
                                              m_owner(owner),
                                              m_sampleRate(sampleRate)
        {
            setStatusMessage(TRANS("Writing file from selection...."));
        }
        
        void run() override
        {
            
            auto buffer = m_owner.m_selectionBuffer;
            auto file = m_owner.m_tempFile;
            
            writer.reset(format.createWriterFor(new juce::FileOutputStream(file), m_sampleRate, m_owner.m_selectionBuffer.getNumChannels(), 24, {}, 0));
            
            if(writer != nullptr)
            {
                int length = buffer.getNumSamples();
                int blockSize = 512;
                int numBlocks = length / blockSize;
                int samplesWritten = 0;
                
                for (int i = 0; i < numBlocks; ++i)
                {
                    if (threadShouldExit()) break;
                    setProgress(i / (double) numBlocks);
                    int startPosition = blockSize * i;
                    
                    if(i != numBlocks - 1)
                    {
                        if(!writer->writeFromAudioSampleBuffer(buffer, startPosition, blockSize)) jassertfalse;
                        samplesWritten += blockSize;
                    } else
                    {
                        writer->writeFromAudioSampleBuffer(buffer, startPosition, length - samplesWritten);
                    }
                }
            } else jassertfalse;
            
            setProgress(-1.0);
            setStatusMessage(TRANS("Finishing up..."));
            wait (1500);
        }
        
        void threadComplete (bool userPressedCancel) override
        {
            if (userPressedCancel)
            {
                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,m_owner.m_tempFile.getFileName(), TRANS("Render Cancelled"));
            }
            m_owner.m_tempFileRendered = true;
            m_owner.m_renderButton.setEnabled(false);
            m_owner.m_renderButton.setTooltip(TRANS("Render complete, exporting now exports selected section"));
        }
        
        juce::WavAudioFormat format;
        std::unique_ptr<juce::AudioFormatWriter> writer;
        TransportEditor& m_owner;
        double m_sampleRate;
    };

public:
    ///The constructor, takes in references to the SoundTableModel, MainComponent and TransportPlayer.
    TransportEditor(class SoundTableModel& tableModel, class MainComponent& mainApp, class TransportPlayer& player);
    ~TransportEditor();
    ///Virtual function inherited from Juce Component and it is overridden to paint the waveform and the playhead or any error texts.
    void paint(juce::Graphics& g) override;
    ///Virtual functon inherted from Juce Component and it is overridden to place all buttons, sliders etc. onto the component.
    void resized() override;
    ///Pure virtual function inherited from Juce changeListener and it is overriden to listen when the TransportPlayer has reached the end of the playback and when that happens, this class tells it to stop playback.
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    ///Pure virtual function inherited from Juce ApplicationCommandTarget and it is overridden to return m_mainApp.
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    ///Pure virtual function inherited from Juce ApplicationCommandTarget and it is overridden to add all command IDs into the array that is passed-in.
    void getAllCommands(juce::Array<juce::CommandID> &commands) override;
    ///Pure virtual function inherited from Juce ApplicationCommandTarget and it is overridden to provide the keyboard shortcuts for all commands
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    ///Pure virtual function inherited from Juce ApplicationCommandTarget and it is overridden to perform the action associated with a keypress. For example, if L is pressed, this function will press the loop button by calling the loop buttons triggerClicked() function
    bool perform (const juce::ApplicationCommandTarget::InvocationInfo &info) override;
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
    ///Virtual function inherited from Juce Component. This function is called when a mouse button is released. This function is overridden to call prepSelectionBuffer() if a section is selected
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
    ///This function copies the selected area from the TransportPlayer's buffer into m_selectionbuffer and calls createFileFromSelection().
    void prepSelectionBuffer();
    ///This function writes a new audio file from the m_selectionbuffer into the tempFiles folder. This function also deletes the previously written file to keep the folder a bit more tidy.
    void createFileFromSelection();
    
    void setBuffer(juce::AudioBuffer<float> newBuffer);
    ///CommandIDs for the keypresses
    enum KeyPressCommandIDs
    {
        Loop = 9,
        PlayPause,
        Stop
    };
    
private:
    ///Reference to the SoundTableModel.
    class SoundTableModel& m_tableModel;
    ///Reference to the MainComponent.
    class MainComponent& m_mainApp;
    ///Reference to the TransportPlayer.
    class TransportPlayer& m_player;
    
    std::unique_ptr<BufferingAlertWindow> m_renderWindow;
    
    std::unique_ptr<ExportAlertWindow> m_exportWindow;
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
    juce::DrawableButton m_playButton, m_stopButton, m_loopButton, m_relocateButton, m_renderButton;
    ///This label displays the current playhead position in seconds
    juce::Label m_playheadPosition;
    ///Volume slider that the user can interact with to control the volume of the playback
    juce::Slider m_volumeSlider;
    ///Various booleans used to control what is drawn by the paint function, prevent exported files to be dropped back into the application and control section play functionality
    bool m_fileSelected = false, m_fileIsValid = false, m_canDragFile = true, m_sectionSelected = false, m_sectionPlayActive = false, m_tempFileRendered = false;
    ///Various integers that represent the size of the audio buffer and which section of the sound file has been selected
    int m_numBufferSamples, m_mouseDragStartPos, m_mouseDragDistance, m_mouseDragEndPos, m_selectionStart, m_selectionEnd, m_filenameSuffix = 1;
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
    ///Temporary audio file written from m_selectionBuffer
    juce::File m_tempFile;
    ///Audiobuffer containing the samples in current selection area
    juce::AudioBuffer<float> m_selectionBuffer;
    ///This funtion is used to change the state of playback
    void changePlayState (TransportEditor::PlayState newPlayState);
    ///This virtual function is inherited from Juce Timer and it is overridden to repaint the component at regular intervals. This makes sure that the playhead is drawn into a correct position.
    void timerCallback() override;
    
};
