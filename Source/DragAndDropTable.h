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
        int numChannels;
        juce::String description;
        juce::String filePath;
        std::vector<juce::String> categories;
        
        void printCategories()
        {
            DBG("Number of Categories: " + juce::String(categories.size()));
            for(int i = 0 ; i < categories.size();++i)
            {
                DBG("Category: " + categories[i]);
            }
        }
        
        bool categoryExists(juce::String newCategory)
        {
            for(int i = 0 ; i < categories.size();++i)
            {
                if(  categories[i].equalsIgnoreCase(newCategory)   ) return true;
            }
            return false;
        }
        void addCategory(juce::String newCategory)
        {
            if(!categoryExists(newCategory))
            {
                DBG("Adding Category: " + newCategory);
                categories.push_back(juce::String(newCategory));
            }
        }
        
        
        void changeDescription(juce::String newDescription)
        {
            description = newDescription;
        }
        
        void printInfo()
        {
            DBG("Length: " + juce::String(lengthInSeconds));
            DBG("SampleRate: " + juce::String(sampleRate));
            DBG("Channels: " + juce::String(numChannels));
            DBG("Description: " + description);
            DBG("File path: " + filePath);
        }
        
        FileInfo(juce::File _file,double _lengthInSeconds,double _sampleRate, int _numChannels, juce::String _filePath):
            file(_file),
            lengthInSeconds(_lengthInSeconds),
            sampleRate(_sampleRate),
            numChannels(_numChannels),
            description(),
            filePath(_filePath)
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
    void showFile(juce::File& file, double length,double sampleRate, int numChannels, juce::String filePath);
    
    //for table
    void backgroundClicked (const juce::MouseEvent&) override;
    void updateDescription(juce::String newString, int rowNum);
    
    void printFileArray();
    
private:

    class MainComponent& m_mainApp;
    
    bool m_acceptingfiles = true;
    bool m_isDragging = false;
    
    juce::File m_selectedFile;
    juce::File m_previouslySelectedFile;
    juce::Component m_file;
    
    std::vector<FileInfo> m_fileArray;
    
    //For table
    int m_numRows = 0;
    juce::Font m_font = (15.0f);
};
