#pragma once

#include <JuceHeader.h>
#include "DragAndDropTable.h"
#include "CategoryListModel.h"
#include "SoundTableModel.h"
#include "TransportEditor.h"
#include "SoundManager.h"
#include "TransportPlayer.h"

class TransportEditor;

///This class is the main component of the application, it brings all the classes together in a single window. It also contains the menu bar.
class MainComponent  : public juce::Component, public juce::MenuBarModel, public juce::ApplicationCommandTarget
{
    friend class DragAndDropTable;
    friend class CategoryListModel;
    friend class SoundTableModel;
    friend class SoundManager;
    friend class TransportEditor;
    
public:
    //==============================================================================
    ///The constructor
    MainComponent();
    ///The destructor. Saves data. deletes the temporary audio file folder and sets the table, category and menubar models to nullptr to avoid issues on showdown.
    ~MainComponent() override;

    //==============================================================================
   

    //==============================================================================
    ///Virtual function inherited from Juce Component. This function is overridden to paint the backround of the component
    void paint (juce::Graphics& g) override;
    ///Virtual funcion inherited from Juce Component. This function is overridden to layout all of the child components into their correct position.
    void resized() override;
    ///Pure virtual function inherited from Juce MenuBarModel and is overridden to return the names for the different bars in the menu.
    juce::StringArray getMenuBarNames() override;
    ///Pure virtual function inherited from Juce MenuBarModel. This function is overridden to create a Juce PopupMenu based on which bar was clicked.
    juce::PopupMenu getMenuForIndex (int menuIndex, const juce::String& menuName) override;
    ///Pure virtual function inherited from Juce MenuBarModel. This function does not do anything since this application uses the ApplicationCommandManager and all of the menu items are command items, however we are still required to have it
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
    ///This function opes a dialogwindow containing a texteditor explaining some of applications functionalities
    void openHelpFile();
    ///Pure virtual function inherited from Juce ApplicationCommandTarget and is overridden to return m_transport.
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    ///Pure virtual function inherited from Juce ApplicationCommandTarget and is overridden to add all command IDs into the array that is passed-in
    void getAllCommands(juce::Array<juce::CommandID> &commands) override;
    ///Pure virtual function inherited from Juce ApplicationCommandTarget and it is overridden to setup all of the header bar menu items
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    ///Pure virtual function inherited from Juce ApplicationCommandTarget and it is overridden to perform menu functionalities when a header bar menu item is clicked
    bool perform (const juce::ApplicationCommandTarget::InvocationInfo &info) override;
    ///CommandIDs for header bar menu items
    enum CommandIDs
    {
        CopyOnImportYes = 1,
        CopyOnImportNo,
        ImportFile,
        SaveData,
        AddCategory,
        AudioSettings,
        Features,
        Search
        
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
    ///The transport player
    class TransportPlayer m_player;
    ///Buttons for transport control and search function.
    juce::DrawableButton m_searchButton;
    ///The search bar
    juce::Label m_searchBar;
    ///The TextEditor containing the helptext
    juce::TextEditor m_helpText;
    ///True if a soundfile should be currently playing. defaults to false.
    bool m_playSoundFile = false;
    ///The current position in the buffer, defaulted to zero so that the playback starts from the beginning of the audio file
    int m_playPosition = 0;
    ///The menubar Component
    juce::MenuBarComponent m_menuBar;
    ///This member enables tooltips for the application
    juce::TooltipWindow m_tooltipWindow;
    ///The ApplicationCommandManager
    juce::ApplicationCommandManager m_commandManager;
    ///The xml element holding the applications save data
    std::unique_ptr<juce::XmlElement> m_audioLibrary;
    ///This folder is created into the users application data directory and it is used to store this applications saved data
    juce::File m_saveFolder;
    ///The file where the xml data is stored
    juce::File m_saveFile;
    ///This folder contains all of the saved audio files
    juce::File m_savedAudioFiles;
    ///This folder contains all of the temporary audio files, it is deleted when the application closes to tidy up.
    juce::File m_tempAudioFiles;
    ///This folder contains all audio files that are being exported somewhere else
    juce::File m_filesToBeExported;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
