/*
  ==============================================================================

    SoundManager.cpp
    Created: 21 Nov 2022 10:59:39am
    Author:  Antti

  ==============================================================================
*/
//WHY DONT I HAVE TO #include "SoundManager.h"
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


SoundManager::SoundManager(class MainComponent& mainApp) : m_mainApp(mainApp)

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

    return numFileInfos;
}

juce::String SoundManager::getInformationAtIndex(int index, int property)
{
    juce::String PropertyValue;
    
    
    auto Fileinfo = m_audioLibraryTree.getChild(index);
    
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

void addCategory(juce::String name)
{
    
}
