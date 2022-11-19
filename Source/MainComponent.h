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

private:
    //==============================================================================
    // Your private member variables go here...

  
    DragAndDropTable m_table;
    
    juce::ListBox m_categories;
    
    CategoryListModel m_categoryModel;
    
    //Potential search bar for categories
    std::unique_ptr<juce::TextEditor> m_categoryHeader;
    
    
    
    
    juce::AudioBuffer<float> m_sampleBuffer;
    juce::AudioFormatManager m_formatManager;
    juce::TextButton m_playStop, m_saveData, m_printData;
    
    bool m_playSoundFile = false;
    int m_playPosition = 0;
    
    int m_lastSelectedRow;
    
    //For save data
    juce::XmlElement m_audioLibrary;
    juce::File m_saveLocation;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
