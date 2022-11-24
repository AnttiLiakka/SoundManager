/*
  ==============================================================================

    SoundManager.h
    Created: 21 Nov 2022 10:59:39am
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include<JuceHeader.h>
#include"MainComponent.h"

class SoundManager : public juce::Label::Listener
{
    friend class MainComponent;
    friend class DragAndDropTable;
    friend class SoundTableModel;
    
public:
    
    SoundManager(class MainComponent& mainApp);
    ~SoundManager();
    
    ///Sets m_audioLibraryTree as a representation of given juce XmlElement
    void getTreeFromXml(juce::XmlElement* xmlelement);
    
    ///Returns the number of FileInfo trees in m_audioLibraryTree
    int getNumFileTrees();
    
    ///Returns the  value of a property at given index in m_audioLibraryTree as a juce String. Property value of 1 returns the filename, 2 returns duration, 3 returns channels, 4 returns samplerate and 5 returns description. So for example, index 2, property 2 returns the second fileinfo trees duration value.
    juce::String getInformationAtIndex(int index, int property);

    ///Returns a string representation of m_audioLibraryTree that can be used for debugging
    juce::String printValueTree();
    ///Saves the tree into the m_mainApp's m_audioLibrary juce XmlElement
    void saveTreeToXml();
    
    ///This function adds a new fileInfo into Tree, note the default value to desciption is an emptry string because when a file is first imported into the application, it will not have a description. However, when a fileInfo is added via xml data, It can potentially have a description string
    void AddFile(juce::File& file, double length,double sampleRate, int numChannels, juce::String filePath, juce::String description = "");
    
    ///updates the description string of a  ValueTree located on the row
    void updateDescription(juce::String newString, int rowNum);
    ///This functon removes a fileInfo from the tree based on the index proviced
    void removeFileInfoTree(int index);
    ///This function adds a category into a fileInfo
    void addCategory(juce::String name, int rowNumber);
    
    void filterByCategory(juce::String categoryName);
    
    void filterBySearch(juce::String searchText);
    
    void setAllVisible();
    
    void addExistingCategories();
    
    void labelTextChanged(juce::Label* labelThatHasChanged) override;
    //This function updates table with currently visible children trees
    juce::ValueTree getVisibleChildAtIndex(int index);
    
private:
    
    class MainComponent& m_mainApp;
    
    juce::ValueTree m_audioLibraryTree;
    
    juce::Array<juce::ValueTree> m_currentTable;
    
    //Identifiers for child trees
    static juce::Identifier m_fileInfo;
    static juce::Identifier m_information;
    static juce::Identifier m_categories;
    
    //Identifier for properties within child trees
    static juce::Identifier m_filePath;
    static juce::Identifier m_isVisible;
    static juce::Identifier m_fileName;
    static juce::Identifier m_duration;
    static juce::Identifier m_channels;
    static juce::Identifier m_sampleRate;
    static juce::Identifier m_description;
    static juce::Identifier m_category;
    static juce::Identifier m_id;
    
};
