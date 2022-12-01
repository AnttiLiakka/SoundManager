/*
  ==============================================================================

    SoundManager.cpp
    Created: 21 Nov 2022 10:59:39am
    Author:  Antti

  ==============================================================================
*/
#include "MainComponent.h"

juce::Identifier SoundManager::m_fileInfo = juce::Identifier("fileInfo");
juce::Identifier SoundManager::m_information = juce::Identifier("information");
juce::Identifier SoundManager::m_categories = juce::Identifier("categories");

juce::Identifier SoundManager::m_isVisible = juce::Identifier("isvisible");
juce::Identifier SoundManager::m_filePath = juce::Identifier("filepath");
juce::Identifier SoundManager::m_fileName = juce::Identifier("filename");
juce::Identifier SoundManager::m_duration = juce::Identifier("duration");
juce::Identifier SoundManager::m_channels = juce::Identifier("channels");
juce::Identifier SoundManager::m_sampleRate = juce::Identifier("samplerate");
juce::Identifier SoundManager::m_description = juce::Identifier("description");
juce::Identifier SoundManager::m_category = juce::Identifier("category");
juce::Identifier SoundManager::m_id = juce::Identifier("id");


SoundManager::SoundManager(MainComponent& mainApp) : m_mainApp(mainApp),
                                                           m_audioLibraryTree(
                                                                                {"audiolibrary", {},
                                                                                    {
                                                                                     }
                                                                                 }
                                                                              )


{
    m_audioLibraryTree.addListener(&m_mainApp.m_tableModel);
    
    
    
}

SoundManager::~SoundManager()
{
    
}

void SoundManager::getTreeFromXml(juce::XmlElement* xmlelement)
{
    juce::ValueTree testTree;
    
    juce::XmlElement* testElement = xmlelement;
    
    if(testElement != nullptr)
    {
        testTree = juce::ValueTree::fromXml(*testElement);
        
        if(testTree.isValid())
        {
            m_audioLibraryTree = testTree;
        }
    }
}

int SoundManager::getNumFileTrees()
{
    jassert(m_audioLibraryTree.isValid());
    int numFileInfos = m_audioLibraryTree.getNumChildren();
    
    int numVisibleFileInfos = 0;
    
    for(int i = 0; i < numFileInfos; ++i)
    {
        auto fileInfo = m_audioLibraryTree.getChild(i);
        
        int isVisible = fileInfo.getProperty(m_isVisible);
        
        if (isVisible == 1)
        {
            ++numVisibleFileInfos;
        }
    }
    if(numVisibleFileInfos == 0) m_mainApp.m_transport.noFileSelected();

    return numVisibleFileInfos;
}

juce::String SoundManager::getInformationAtIndex(int index, int property)
{
    juce::String PropertyValue;
    
    
    auto Fileinfo = getVisibleChildAtIndex(index);
    
    auto Information = Fileinfo.getChildWithName(m_information);
    
    switch (property) {
        case 1:
            PropertyValue = Information.getProperty(m_fileName);
            break;
            
        case 2:
            PropertyValue = Information.getProperty(m_duration);
            break;
            
        case 3:
            PropertyValue = Information.getProperty(m_channels);
            break;
        
        case 4:
            PropertyValue = Information.getProperty(m_sampleRate);
            break;
            
        case 5:
            PropertyValue = Information.getProperty(m_description);
    }
    
    
    return PropertyValue;
}

juce::String SoundManager::printValueTree()
{
    jassert(m_audioLibraryTree.isValid());
    
    juce::String valueTreeString;
    valueTreeString = m_audioLibraryTree.toXmlString();
    return valueTreeString;
}

void SoundManager::saveTreeToXml()
{
    jassert(m_audioLibraryTree.isValid());
    
    m_mainApp.m_audioLibrary = m_audioLibraryTree.createXml();
    
    m_mainApp.m_audioLibrary->writeTo(m_mainApp.m_saveFile);
}

void SoundManager::AddFile(juce::File& file, double length,double sampleRate, int numChannels, juce::String filePath, juce::String description)
{
    auto fileInfo = juce::ValueTree
    { m_fileInfo, {{m_filePath, filePath}, {m_isVisible, 1}},
      {
        { m_information, {{ m_fileName, file.getFileName() }, {m_duration, length}, {m_channels, numChannels},{m_sampleRate, sampleRate}, {m_description, description}},
        },
        { m_categories, {},
        }
      }
    };
    
    m_audioLibraryTree.appendChild(fileInfo, nullptr);
    setAllVisible();
}

void SoundManager::updateDescription(juce::String newString, int rowNum)
{
    jassert(m_audioLibraryTree.isValid());
    auto fileInfo = m_audioLibraryTree.getChild(rowNum);
    auto information = fileInfo.getChildWithName(m_information);
    information.setProperty(m_description, newString, nullptr);
}

void SoundManager::removeFileInfoTree(int index)
{
    jassert(m_audioLibraryTree.isValid());
    m_audioLibraryTree.removeChild(index, nullptr);
    
}

void SoundManager::addCategory(juce::String name, int rowNumber)
{
    //set up a new category valuetree to add to the categories of the correct tree
    juce::ValueTree newCategory(m_category);
    newCategory.setProperty(m_id, name, nullptr);
    
    //get the filepath of the tree that was clicked
    juce::ValueTree child = m_currentTable[rowNumber];
    jassert(child.isValid());
    juce::String filepath = child.getProperty(m_filePath);
    
    //get a child from m_audioLibrarytree with the same filepath. This is the tree the category needs to be added
    auto correctFileInfo = m_audioLibraryTree.getChildWithProperty(m_filePath, filepath);
    jassert(correctFileInfo.isValid());
    
    //get the categories child of the tree
    auto categories = correctFileInfo.getChildWithName(m_categories);
   //add new category as a child
    categories.appendChild(newCategory, nullptr);
    
}

void SoundManager::deleteCategory(juce::String name, int rowNumber)
{
    auto categoryToDelete = name;
    juce::ValueTree fileInfo = m_currentTable[rowNumber];
    
    auto categories = fileInfo.getChildWithName(m_categories);
    
    auto category = categories.getChildWithProperty(m_id, categoryToDelete);
    if(category.isValid())
    {
        categories.removeChild(category, nullptr);
    }
}

void SoundManager::filterByCategory(juce::String categoryName)
{
    for(int i = 0; i < m_audioLibraryTree.getNumChildren(); ++i)
    {
        bool categoryExists = false;
        
        auto fileInfo = m_audioLibraryTree.getChild(i);
        
        auto categories = fileInfo.getChildWithName(m_categories);
        
        for(int n = 0; n < categories.getNumChildren(); ++n)
        {
            auto category = categories.getChild(n);
            if(category.isValid())
            {
                juce::String name = category.getProperty(m_id);
                if(name.equalsIgnoreCase(categoryName))
                {
                    categoryExists = true;
                }
            }
        }
        
        if (categoryExists)
        {
            fileInfo.setProperty(m_isVisible, 1, nullptr);
        } else
        {
            fileInfo.setProperty(m_isVisible, 0, nullptr);
        }
    }
}

void SoundManager::filterBySearch(juce::String searchText)
{
    auto testString = searchText;
    
    for (int i = 0; i <m_audioLibraryTree.getNumChildren(); ++i)
    {
        auto fileInfo = m_audioLibraryTree.getChild(i);
        auto information = fileInfo.getChildWithName(m_information);
        juce::String fileName = information.getProperty(m_fileName);
        
        if(fileName.containsIgnoreCase(testString))
        {
            fileInfo.setProperty(m_isVisible, 1, nullptr);
        } else
        {
            fileInfo.setProperty(m_isVisible, 0, nullptr);
        }
    }
}

void SoundManager::setAllVisible()
{
    for (int i = 0; i <m_audioLibraryTree.getNumChildren(); ++i)
    {
        auto fileInfo = m_audioLibraryTree.getChild(i);
        fileInfo.setProperty(m_isVisible, 1, nullptr);
    }
}

juce::ValueTree SoundManager::getVisibleChildAtIndex(int index)
{
    juce::ValueTree child;
    
    juce::Array<juce::ValueTree> visibleChildren;
    
    for (int i = 0; i < m_audioLibraryTree.getNumChildren(); ++i)
    {
        if(m_audioLibraryTree.getChild(i).getProperty(m_isVisible, false))
        {
            visibleChildren.add(m_audioLibraryTree.getChild(i));
        }
        
    }
    
    m_currentTable = visibleChildren;
    child = visibleChildren[index];
    
    return child;
}

void SoundManager::addExistingCategories()
{
    for(int i = 0; i < m_audioLibraryTree.getNumChildren(); ++i)
    {
        auto fileinfo = m_audioLibraryTree.getChild(i);
        auto categories = fileinfo.getChildWithName(m_categories);
        
        for(int n = 0; n < categories.getNumChildren(); ++n)
        {
            auto category = categories.getChild(n);
            if(category.isValid())
            {
                juce::String categoryName = category.getProperty(m_id);
                //This will stop an empty string getting into the list (if fileinfo has no categories it will be empty)
                if(categoryName.isNotEmpty())
                {
                    m_mainApp.m_categoryModel.addCategoryToList(categoryName);
                }
            }
        }
    }
}

void SoundManager::labelTextChanged(juce::Label* labelThatHasChanged)
{
    auto newText = labelThatHasChanged->getText();
    
    if (newText.isNotEmpty())
    {
        m_mainApp.m_categories.deselectAllRows();
        filterBySearch(newText);
    } else
    {
        setAllVisible();
    }
    
}

juce::StringArray SoundManager::getCategories(int rowNumber)
{
    juce::StringArray categoryArray;
    
    auto fileInfo = m_currentTable[rowNumber];
    
    auto categories = fileInfo.getChildWithName(m_categories);
    
    for(int i = 0; i < categories.getNumChildren(); ++i)
    {
        auto category = categories.getChild(i);
        
        if(category.isValid())
        {
            juce::String categoryName = category.getProperty(m_id);
            
            if(categoryName.isNotEmpty())
            {
                categoryArray.add(categoryName);
            }
        }
    }
    
    return categoryArray;
}

juce::File SoundManager::getFileOnRow(int rowNumber)
{
    juce::String filepath;
    
    auto fileInfo = m_currentTable[rowNumber];
    
    filepath = fileInfo.getProperty(m_filePath);
    
    juce::File file(filepath);
    
    return file;
}

void SoundManager::setNewFilepath(juce::File fileToRelocate)
{
    m_oldFilepath = fileToRelocate.getFullPathName();
    
    m_fileChooser = std::make_unique<juce::FileChooser>(TRANS("Select a file"),juce::File(),"*.wav; *.aiff"   );
    auto flags = juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode   ;
    m_fileChooser->launchAsync(flags, [&](const juce::FileChooser& chooser){
        
        juce::File fileToTest(chooser.getResult());
        
        try {
            if(fileToTest.isDirectory())throw TRANS("You've seleced a directory, please pick an audio file");
            if(!fileToTest.existsAsFile())throw TRANS("No file selected");
            auto fileReader = m_mainApp.m_table.m_formatManager.createReaderFor(fileToTest);
            if(fileReader == nullptr) throw TRANS("Error loading the File");
            juce::String newFilepath = fileToTest.getFullPathName();
            delete fileReader;
            
            changeFilepath(newFilepath, m_oldFilepath);
            
        } catch (juce::String message){
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,fileToTest.getFileName() , message);
        }
    
    
    });
}

void SoundManager::changeFilepath(juce::String newPath, juce::String oldPath)
{
    juce::File newFile(newPath);
    
    auto newFilename = newFile.getFileName();
    
    auto fileinfo = m_audioLibraryTree.getChildWithProperty(m_filePath, oldPath);
    jassert(fileinfo.isValid());
    auto information = fileinfo.getChildWithName(m_information);
    if(information.getProperty(m_fileName) != newFilename)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("File name does not match"), TRANS("Please select another file"));
    } else
    {
        fileinfo.setProperty(m_filePath, newPath, nullptr);
        m_mainApp.m_tableModel.reloadWaveform();
    }
}
