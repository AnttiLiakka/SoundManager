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

class DragAndDropTable : public juce::TableListBox, public juce::DragAndDropContainer, public juce::FileDragAndDropTarget
{
    friend class MainComponent;
public:
    DragAndDropTable(class MainComponent& mainApp) : TableListBox("table", nullptr), DragAndDropContainer(),
        m_mainApp(mainApp)
    {
        

    }
    //For drag and drop import
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void loadDroppedFile(const juce::String& path);

    //For drag and drop export
    void dragExport();   
    void showFile(juce::File& file, double length);
    void setAcceptTrue();
private:

    class MainComponent& m_mainApp;
    
    bool m_acceptingfiles = true;
    
    juce::TextButton m_fileInfo;
    juce::File m_selectedFile;    
    juce::Component m_file;
    
    juce::Array<juce::File> m_fileArray;
    juce::Array<double> m_lengthArray;
    
    //For table
    int m_numRows = 0;
    juce::Font m_font = (15.0f);
};
