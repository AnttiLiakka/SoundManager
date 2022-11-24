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
class MainComponent  : public juce::AudioAppComponent, public juce::MenuBarModel
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
    
    ///This function is used to prepare a file in a given row for playback by loading it into the samplebuffer using a filereader
    void prepFileToPlay(int rowNumber);
        
    //Menu functions
    ///Pure virtual function inherited from Juce MenuBarModel. This fuction returns the names for the different bars in the menu
    juce::StringArray getMenuBarNames() override;
    ///Pure virtual function inherited from Juce MenuBarModel. This function creates a juce PopupMenu based on which bar was clicked
    juce::PopupMenu getMenuForIndex (int menuIndex, const juce::String& menuName) override;
    ///Pure virtual function inherited from Juce MenuBarModel. This function is used to decide what to do once user has clicked an option on the Juce PopupMenu created with getMenuForIndex
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
    ///This function is used for manual file import. The functionality is very similar to Addfile in the SoundManager class
    void manualFileImport();
    
    ///This function is used to create a new category that will appear into the categorylist and can be assigned to files by right clicking them on the table
    void AddNewCategory(juce::String newCategory);
    
    //Data structure
    ///This function is to be deleted
    void printXmlContent();
    ///This function is to be deleted
    void loadXmlContent();
    // void importDataIntoArray();

private:
    //==============================================================================

    ///The model for the sound file table
    SoundTableModel m_tableModel;
    ///The sound file table with drag and drop functionality 
    DragAndDropTable m_table;
    ///The valueTree
    SoundManager m_valueTree;
    ///The list of categories
    juce::ListBox m_categories;
    ///The model for the list of categories
    CategoryListModel m_categoryModel;
    
    
    ///The audiobuffer
    juce::AudioBuffer<float> m_sampleBuffer;
    ///The  audio format manager
    juce::AudioFormatManager m_formatManager;
    ///Filechooser used for manually importing files
    std::unique_ptr<juce::FileChooser> m_fileChooser;
    ///Buttons for transport control and search function.
    juce::DrawableButton m_playButton, m_pauseButton, m_stopButton, m_searchButton;
    ///The search bar
    juce::Label m_searchBar;
    
    ///True if a soundfile should be currently playing. defaults to false.
    bool m_playSoundFile = false;
    ///The current position in the buffer, defaulted to zero so that the playback starts from the beginning of the audio file
    int m_playPosition = 0;
    
    ///This member holds the last selected row number. This is important so that the user does not have to reclick rows after each action
    int m_lastSelectedRow;
    
    ///The menubar Component
    juce::MenuBarComponent m_menuBar;
    
    //For save data
    ///The xml element holding the applications save data
    std::unique_ptr<juce::XmlElement> m_audioLibrary;
    ///The file where the xml data is stored
    juce::File m_saveFile;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
