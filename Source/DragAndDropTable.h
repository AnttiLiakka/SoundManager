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
#include "CategoryListModel.h"

class DragAndDropTable : public juce::TableListBox, public juce::DragAndDropContainer, public
                         juce::FileDragAndDropTarget
{
    friend class MainComponent;
    friend class CategoryListModel;
    
    ///A juce label that can be put into a cell to keep the FileInfo description information
    struct CellLabel: public juce::Label
    {
        ///The row this label is on. Important to make sure that the correct description is edited
        int row;
        
        ///Sets the row member
        void setRow(const int rowNum)
        {
            row = rowNum;
        }
        
        
        ///Default Constructor
        CellLabel()
        {
        }
    };
    
    ///Structure that holds information about the cells on the table
    struct FileInfo
    {
        ///The juce File class representation of the file
        juce::File file;
        ///The lenght of the file in seconds
        double lengthInSeconds;
        ///The sample rate of the file
        double sampleRate;
        ///The number of channels the file has
        int numChannels;
        ///The description that can be assigned to the file from the table
        juce::String description;
        ///The filepath to the file
        juce::String filePath;
        ///Contains the information on which categories the structure is assigned to
        std::vector<juce::String> categories;
        
        ///Returns true if a category is already assigned to the structure
        bool categoryExists(juce::String newCategory)
        {
            for(int i = 0 ; i < categories.size();++i)
            {
                if(  categories[i].equalsIgnoreCase(newCategory)   ) return true;
            }
            return false;
        }
        ///Adds a category to the structure
        void addCategory(juce::String newCategory)
        {
            if(!categoryExists(newCategory))
            {
                DBG("Adding Category: " + newCategory);
                categories.push_back(juce::String(newCategory));
            }
        }
        ///Changes the description of the structure
        void changeDescription(juce::String newDescription)
        {
            description = newDescription;
        }
        ///Prints all the members
        void printInfo()
        {
            int numCategories = 0;
            DBG("File path: " + filePath);
            DBG("Length: " + juce::String(lengthInSeconds));
            DBG("SampleRate: " + juce::String(sampleRate));
            DBG("Channels: " + juce::String(numChannels));
            DBG("Description: " + description);
            for(int i = 0 ; i < categories.size();++i)
            {
                DBG("Category: " + categories[i]);
                ++numCategories;
            }
            DBG("Total number of categories: " + juce::String(numCategories));
        }
        
        ///A constructor used to create a FileInfo structure
        FileInfo(juce::File _file,double _lengthInSeconds,double _sampleRate, int _numChannels, juce::String _filePath, juce::String _description = ""):
            file(_file),
            lengthInSeconds(_lengthInSeconds),
            sampleRate(_sampleRate),
            numChannels(_numChannels),
            description(_description),
            filePath(_filePath)
        {
            
        }
        FileInfo()
        {
        }
    };
    
    
public:
    
    /**
        This class is used alongside juce TableListBoxModel to create a table of audio files which can be dragged and dropped in and out of the table
     */
    ///The constructor
    /// The mainApp is the class that holds the TableListBoxModel which is need to create the table
    DragAndDropTable(class MainComponent& mainApp) :
                     TableListBox("table", nullptr),
                     DragAndDropContainer(),
                     m_mainApp(mainApp)
    {
        

    }
    ///Overridden pure virtual functions for the juce DragAndDropTarget class
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    
    ///Called if isInterestedInFileDrag returns true and files are dropped on the table. This checks whether the dropped files are valid audio files
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    ///called if isInterestedinFileDrag returns true and folders are dropped on the table. This checks wether the files in the folder are valid audio files
    void foldersDropped (const juce::Array<juce::File>& folders);

    //For drag and drop export
    
    ///Performs an asynchronous drag and drop export of a selected file.
    void dragExport();
    ///This function adds a new fileInfo into the fileArray, note the default value to desciption is an emptry string because when a file is first imported in to fileArray it will not have a description. However, when a fileInfo is added via xml data, I can potentially have a description string
    void AddFile(juce::File& file, double length,double sampleRate, int numChannels, juce::String filePath, juce::String description = "");
    
    //for table
    
    ///This is called when a table background is clicked
    void backgroundClicked (const juce::MouseEvent&) override;
    
    ///updates the description string of a FileInfo structure located on the row
    void updateDescription(juce::String newString, int rowNum);
    
    //For fileArray
    ///Prints all of the FileInfos in the m_fileArray vector
    void printFileArray();
    
private:
    
    ///A reference to class holding the juce TableListBoxModel
    class MainComponent& m_mainApp;
    
    ///States whether the table is currently accepting imported files
    bool m_acceptingfiles = true;
    ///States whether a file is currently being dragged from this class
    bool m_isDragging = false;
    
    ///The file to be exported if dragExport() is called
    juce::File m_selectedFile;
    ///A container for all the FileInfos
    std::vector<FileInfo> m_fileArray;
    
    //For table
    ///The current number of rows in the table
    int m_numRows = 0;
    
    ///The font used to write the text in the cells
    juce::Font m_font = (15.0f);
};
