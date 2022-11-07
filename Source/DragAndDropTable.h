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

class DragAndDropTable : public juce::TableListBox, public juce::TableListBoxModel, public juce::DragAndDropContainer, public juce::FileDragAndDropTarget
{
    friend class MainComponent;
public:
    DragAndDropTable(class MainComponent& mainApp) : TableListBox("table", this), DragAndDropContainer(),
        m_mainApp(mainApp)
    {
        

    }
    //For drag and drop import
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void loadDroppedFile(const juce::String& path);

    //For drag and drop export
    void dragExport();
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent &event) override;
    void showFile(juce::File& file);

    //Do I really need this?
    void resized() override;
    
    //TableListBoxModel virtual functions
    int getNumRows() override;
    void paintRowBackground(juce::Graphics &p, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    
    
private:

    class MainComponent& m_mainApp;
    
    bool m_acceptingfiles = true;
    
    juce::TextButton m_fileInfo;
    juce::File m_selectedFile;    
    juce::Component m_file;
};
