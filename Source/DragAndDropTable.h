/*
  ==============================================================================

    DragAndDropTable.h
    Created: 3 Nov 2022 7:16:50pm
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MainComponent.h"

///This class is the table component holding the SoundTableModel. Supported audio files can be dragged and dropped in the table to add them to the SoundManager database
class DragAndDropTable : public juce::TableListBox, public
                         juce::FileDragAndDropTarget
{
    friend class MainComponent;
    friend class CategoryListModel;
    friend class SoundTableModel;
    friend class SoundManager;
    friend class TransportEditor;
    
public:
    ///The constructor, takes a reference to the MainComponent
    DragAndDropTable(class MainComponent& mainApp, juce::File& saveLocation) :
                     TableListBox("table", nullptr),
                     m_mainApp(mainApp),
                     m_saveLocation(saveLocation)
    {
        

    }
    ///Pure virtual function inherited from juce FileDragAndDropTarget. This function is used to check whether the target is interested in the files dragged over it and it is overridden to return true if the files are .wav or .aiff audiofiles.
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    ///Pure virtual function inherited from juce FileDragAndDropTarget. Called when files are dropped onto the table and it is overridden to check whether the dropped files are valid audiofiles. If they are, they are added to the database using the Addfile function of SoundManager.
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    ///Pure virtual function inherited from juce TableListBox. Called when table background is clicked and it is overridden to deselect all table cells and category listbox rows.
    void backgroundClicked (const juce::MouseEvent&) override;
    ///This function is used for manual file import. The functionality is very similar to the filesDropped function.
    void manualFileImport();
    
private:
    
    ///Reference to the MainComponent
    class MainComponent& m_mainApp;
    ///Reference to the folder to which audio files are saved if m_copyOnImport is true;
    juce::File& m_saveLocation;
    ///States whether the table is currently accepting imported files. This member is initialised to true
    bool m_acceptingfiles = true;
    ///States whether the application should copy imported audio files into the savedAudiofiles folder
    bool m_copyOnImport;
    ///The font used to write the text in the cells
    juce::Font m_font = (15.0f);
    ///The  audio format manager needed to test whether audio files are valid
    juce::AudioFormatManager m_formatManager;
    ///Filechooser used for manually importing files
    std::unique_ptr<juce::FileChooser> m_fileChooser;
};
