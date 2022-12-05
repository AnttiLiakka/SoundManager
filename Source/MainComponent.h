#pragma once

#include <JuceHeader.h>
#include "DragAndDropTable.h"
#include "CategoryListModel.h"
#include "SoundTableModel.h"
#include "TransportEditor.h"
#include "SoundManager.h"
#include "TransportPlayer.h"

class TransportEditor;

///This class is the main component of the application, it brings all the classes together in a single window. It also contains the menubar.
class MainComponent  : public juce::Component, public juce::MenuBarModel, public juce::KeyListener, public juce::ApplicationCommandTarget
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
   

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    ///Pure virtual function inherited from Juce MenuBarModel. This fuction returns the names for the different bars in the menu.
    juce::StringArray getMenuBarNames() override;
    ///Pure virtual function inherited from Juce MenuBarModel. This function creates a juce PopupMenu based on which bar was clicked.
    juce::PopupMenu getMenuForIndex (int menuIndex, const juce::String& menuName) override;
    ///Pure virtual function inherited from Juce MenuBarModel. This function is used to decide what to do once user has clicked an option on the Juce PopupMenu created with getMenuForIndex.
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
    
    bool keyPressed (const juce::KeyPress &key, juce::Component* originatingComponent) override;
    
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    
    void getAllCommands(juce::Array<juce::CommandID> &commands) override;
    
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    
    bool perform (const juce::ApplicationCommandTarget::InvocationInfo &info) override;
    
    enum CommandIDs
    {
        CopyOnImport = 1,
        ImportFile,
        SaveData,
        AddCategory,
        AudioSettings,
        Features
        
    };

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
    
    class TransportPlayer m_player;
    
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
    ///This member enables tooltips for the application
    juce::TooltipWindow m_tooltipWindow;
    
    juce::ApplicationCommandManager m_commandManager;
    ///The xml element holding the applications save data
    std::unique_ptr<juce::XmlElement> m_audioLibrary;
    ///The file where the xml data is stored
    juce::File m_saveFile;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
