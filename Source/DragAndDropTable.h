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
    
    struct FileInfo
    {
        juce::File file;
        double lengthInSeconds;
        double sampleRate;
        
        FileInfo(juce::File _file,double _lengthInSeconds,double _sampleRate):
            file(_file),
            lengthInSeconds(_lengthInSeconds),
            sampleRate(_sampleRate)
        {
            
        }
        FileInfo()
        {
        }
    };
    
    
public:
    DragAndDropTable(class MainComponent& mainApp) : TableListBox("table", nullptr), DragAndDropContainer(),
        m_mainApp(mainApp)
    {
        

    }
    //For drag and drop import
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    //For drag and drop export
    void dragExport();   
    void showFile(juce::File& file, double length,double sampleRate);
    
private:

    class MainComponent& m_mainApp;
    
    bool m_acceptingfiles = true;
    
    juce::TextButton m_fileInfo;
    juce::File m_selectedFile;
    juce::File m_previouslySelectedFile;
    juce::Component m_file;
    
    juce::Array<FileInfo> m_fileArray;
    juce::Array<double> m_lengthArray;
    
    //For table
    int m_numRows = 0;
    juce::Font m_font = (15.0f);
};
