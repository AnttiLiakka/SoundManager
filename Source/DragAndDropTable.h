/*
  ==============================================================================

    DragAndDropTable.h
    Created: 3 Nov 2022 7:16:50pm
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DragAndDropTable : public juce::TableListBox, public juce::DragAndDropContainer, public juce::FileDragAndDropTarget
{
public:
    DragAndDropTable() : TableListBox("table", nullptr), DragAndDropContainer()                        
    {
        

    }
    //For drag and drop import
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void loadDroppedFile(const juce::String& path);

    //For drag and drop export
    void dragExport();
    void mouseDown(const juce::MouseEvent& event) override;

    void showFile(juce::File& file);

    //Do I really need this?
    void resized() override;

    

    

private:

    juce::TextButton m_fileInfo;
    juce::File m_selectedFile;    
    juce::Component m_file;


};
