#pragma once

#include <JuceHeader.h>
#include "DragAndDropTable.h"
#include "CategoryListModel.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, juce::TableListBoxModel
{
    friend class DragAndDropTable;
    friend class CategoryListModel;
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
    
    //TableListBoxModel =0 virtuals
    void paintRowBackground(juce::Graphics& p, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    int  getNumRows() override;


    //Other TableListBoxModel functions
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& mouseEvent) override;
    void selectedRowsChanged(int lastRowSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component                                             *existingComponentToUpdate) override;
    
    
    //PopupMenu functions
    void cellPopupAction(int selection, int rowNumber, int columnId, const juce::MouseEvent& mouseEvent);
    void AddNewCategory(juce::String newCategory);
    
    //Data structure
    void saveContentToXml();
    void printContent();
    void loadXmlContent();

private:
    //==============================================================================
    ///The table with drag and drop functionality holding this model
    DragAndDropTable m_table;
    ///The list of categories
    juce::ListBox m_categories;
    ///The model for the list of categories
    CategoryListModel m_categoryModel;
    
    
    
    ///The audiobuffer
    juce::AudioBuffer<float> m_sampleBuffer;
    ///The  audio format manager
    juce::AudioFormatManager m_formatManager;
    ///Buttons for playback and save data
    juce::TextButton m_playStop, m_saveData, m_printData;
    
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
