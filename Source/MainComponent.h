#pragma once

#include <JuceHeader.h>
#include "DragAndDropTable.h"
#include "CategoryListModel.h"
#include "SoundTableModel.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
    friend class DragAndDropTable;
    friend class CategoryListModel;
    friend class SoundTableModel;
    friend class SoundManager;
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void prepFileToPlay(int rowNumber);
        
    //PopupMenu functions
    void cellPopupAction(int selection, int rowNumber, int columnId, const juce::MouseEvent& mouseEvent);
    void AddNewCategory(juce::String newCategory);
    
    //Data structure
    void saveContentToXml();
    void printContent();
    void loadXmlContent();
    // void importDataIntoArray();

private:
    //==============================================================================
    ///The list of categories
    juce::ListBox m_categories;
    ///The model for the list of categories
    CategoryListModel m_categoryModel;
    ///The model for the sound file table
    SoundTableModel m_tableModel;
    ///The sound file table with drag and drop functionality 
    DragAndDropTable m_table;
    ///The valueTree
    SoundManager m_valueTree;
    
    
    
    ///The audiobuffer
    juce::AudioBuffer<float> m_sampleBuffer;
    ///The  audio format manager
    juce::AudioFormatManager m_formatManager;
    ///Buttons for playback and save data
    juce::TextButton m_playStop, m_saveData, m_printData, m_printArray;
    
    ///Whether sound file is currently playing or not, defauts to false
    bool m_playSoundFile = false;
    ///The current position in the buffer, defaulted to zero so that the playback starts from the beginning of the audio file
    int m_playPosition = 0;
    
    ///This member holds the last selected row number. This is important so that the user does not have to reclick rows after each action
    int m_lastSelectedRow;
    
    
    //For save data
    ///The xml element holding the applications save data
    std::unique_ptr<juce::XmlElement> m_audioLibrary;
    ///The file where the xml data is stored
    juce::File m_saveFile;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
