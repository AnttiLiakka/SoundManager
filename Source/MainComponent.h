#pragma once

#include <JuceHeader.h>
#include "DragAndDropTable.h"
#include "CategoryListModel.h"
#include "SoundTableModel.h"
#include "TransportEditor.h"
#include "SoundManager.h"

class TransportEditor;

class MainComponent  : public juce::AudioAppComponent, public juce::MenuBarModel
{
    friend class DragAndDropTable;
    friend class CategoryListModel;
    friend class SoundTableModel;
    friend class SoundManager;
    friend class TransportEditor;
    
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
    
    //Menu functions
    ///Pure virtual function inherited from Juce MenuBarModel. This fuction returns the names for the different bars in the menu
    juce::StringArray getMenuBarNames() override;
    ///Pure virtual function inherited from Juce MenuBarModel. This function creates a juce PopupMenu based on which bar was clicked
    juce::PopupMenu getMenuForIndex (int menuIndex, const juce::String& menuName) override;
    ///Pure virtual function inherited from Juce MenuBarModel. This function is used to decide what to do once user has clicked an option on the Juce PopupMenu created with getMenuForIndex
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
   

private:
    //==============================================================================

    ///The model for the sound file table
    class SoundTableModel m_tableModel;
    ///The sound file table with drag and drop functionality 
    class DragAndDropTable m_table;
    ///The valueTree
    class SoundManager m_valueTree;
    ///The list of categories
    class juce::ListBox m_categories;
    ///The model for the list of categories
    class CategoryListModel m_categoryModel;
    ///The transport editor
    class TransportEditor m_transport;
    
    juce::AudioDeviceManager m_audioDeviceManager;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> m_audioSettings;
    
    ///Buttons for transport control and search function.
    juce::DrawableButton m_searchButton;
    ///The search bar
    juce::Label m_searchBar;
    
    ///True if a soundfile should be currently playing. defaults to false.
    bool m_playSoundFile = false;
    ///The current position in the buffer, defaulted to zero so that the playback starts from the beginning of the audio file
    int m_playPosition = 0;
    ///The menubar Component
    juce::MenuBarComponent m_menuBar;
    //For save data
    ///The xml element holding the applications save data
    std::unique_ptr<juce::XmlElement> m_audioLibrary;
    ///The file where the xml data is stored
    juce::File m_saveFile;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
