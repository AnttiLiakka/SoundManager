/*
  ==============================================================================

    SoundManager.h
    Created: 21 Nov 2022 10:59:39am
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include<JuceHeader.h>

///This class is the juce ValueTree holding the database for SoundTableModel. 
class SoundManager : public juce::Label::Listener
{
    friend class MainComponent;
    friend class DragAndDropTable;
    friend class SoundTableModel;
    
public:
    ///The constructor. Takes in a reference of the main application
    SoundManager(class MainComponent& mainApp);
    ///The destructor
    ~SoundManager();
    
    ///Sets m_audioLibraryTree as a representation of given juce XmlElement
    void getTreeFromXml(juce::XmlElement* xmlelement);
    
    ///Returns the number of visible FileInfo trees in m_audioLibraryTree
    int getNumFileTrees();
    
    ///Returns the value of a property at given index in m_audioLibraryTree as a juce String. Property value of 1 returns the filename, 2 returns duration, 3 returns channels, 4 returns samplerate and 5 returns description. So for example, index 2, property 2 returns the second fileinfo trees duration value. Do note that this only includes visible children. Invisible children are ignored
    juce::String getInformationAtIndex(int index, int property);

    ///Returns a string representation of m_audioLibraryTree that can be used for debugging
    juce::String printValueTree();
    ///Saves the tree into the m_mainApp's m_audioLibrary juce XmlElement
    void saveTreeToXml();
    
    ///This function adds a new fileInfo into Tree, note the default value to desciption is an emptry string because when a file is first imported into the application, it will not have a description. However, when a fileInfo is added via xml data, It can potentially have a description string
    void AddFile(juce::File& file, double length,double sampleRate, int numChannels, juce::String filePath, juce::String description = "");
    
    ///updates the description string of a juce ValueTree located on the row
    void updateDescription(juce::String newString, int rowNum);
    ///This functon removes a fileinfo from the tree based on the index proviced
    void removeFileInfoTree(int index);
    ///This function adds a category into a fileinfo
    void addCategory(juce::String name, int rowNumber);
    ///This function removes a category from a fileinfo
    void deleteCategory(juce::String name, int rowNumber);
    ///This function is used to filter the table based on selected category. It goes through all fileinfos in the tree and checks whether they have the given category. If they do, it sets them  visible and viseversa
    void filterByCategory(juce::String categoryName);
    ///This function is used to filter the table based on the string written in the search box. It goes through all fileInfos in the tree and checks whether they contain the given string. if they do, it sets them visible and viseversa
    void filterBySearch(juce::String searchText);
    ///This function is used to set all fileinfos visible. It is called on application start and when no categories are selected and the search bar is empty.
    void setAllVisible();
    ///This functon is used to add categories on the ValueTree to the CategoryListModel on application start. This makes sure all categories appear in the Listbox and it is not needed to add them manually everytime the application is launched
    void addExistingCategories();
    ///This funcion returns a juce Array of strings with all of the categories in a fileinfo located on a row.
    juce::StringArray getCategories(int rowNumber);
    ///Pure virtual function inherited from Juce Label Listener. It is overridden to filter the table based on seach box content alongside filterByCategory.
    void labelTextChanged(juce::Label* labelThatHasChanged) override;
    ///This function returns a visible fileinfo ValueTree that is put on a row on the table. It creates an Array of visible FileInfos and the index integer defines which FileInfo is returned from the Array
    juce::ValueTree getVisibleChildAtIndex(int index);
    ///This function returns a file that is currently in a row on the table
    juce::File getFileOnRow(int rowNumber);
    ///This function is the first part of changing a filepath of a fileinfo
    void setNewFilepath(juce::File fileToRelocate);
    
    void changeFilepah(juce::String newPath, juce::String oldPath);
    
private:
    ///Reference to the MainComponent
    class MainComponent& m_mainApp;
    ///The juce ValueTree holdeing the audiolibrary
    juce::ValueTree m_audioLibraryTree;
    ///A juce Array of juce ValueTrees currently visible on the table. This member is crucual to make sure that when user interacts with cells on the table, correct FileInfo is interacted with.
    juce::Array<juce::ValueTree> m_currentTable;
    
    juce::String m_oldFilepath;
    
    std::unique_ptr<juce::FileChooser> m_fileChooser;
    
    ///A static juce Identifier for the fileinfo tree.
    static juce::Identifier m_fileInfo;
    ///A static juce Identifier for the information tree.
    static juce::Identifier m_information;
    ///A static juce Identifier for the categories tree.
    static juce::Identifier m_categories;
    ///A static juce Identifier for the category tree.
    static juce::Identifier m_category;
    
    ///A static juce Identifier for the filepath property.
    static juce::Identifier m_filePath;
    ///A static juce Identifier for the isvisible property.
    static juce::Identifier m_isVisible;
    ///A static juce Identifier for the filename property.
    static juce::Identifier m_fileName;
    ///A static juce Identifier for the duration property.
    static juce::Identifier m_duration;
    ///A static juce Identifier for the channel property.
    static juce::Identifier m_channels;
    ///A static juce Identifier for the samplerate property.
    static juce::Identifier m_sampleRate;
    ///A static juce Identifier for the description property.
    static juce::Identifier m_description;
    ///A static juce Identifier for the id property.
    static juce::Identifier m_id;
    
};
