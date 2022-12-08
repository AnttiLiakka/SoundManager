#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() :
                                 m_tableModel(*this, m_valueTree),
                                 m_table(*this, m_savedAudioFiles),
                                 m_valueTree(*this),
                                 m_categories("categories", nullptr),
                                 m_categoryModel(*this, m_valueTree),
                                 m_transport(m_tableModel, *this, m_player),
                                 m_player(m_transport),
                                 m_searchButton("Search", juce::DrawableButton::ButtonStyle::ImageRaw),
                                 m_tooltipWindow(this),
                                 m_audioLibrary(std::make_unique<juce::XmlElement>("audiolibrary")),
                                 m_saveFolder(juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory).getChildFile("SoundManager"))
{
    // Make sure you set the size of the component after
    // you add any child components
    
    //Check whether application folder already exists in application data directory
    if(!m_saveFolder.exists())
    {
        {
            //if not, create one
            m_saveFolder.createDirectory();
        }
    }
    //check whether a save file already exists
    m_saveFile = m_saveFolder.getChildFile("audiolibrary.xml");
    
    if(!m_saveFile.exists())
    {
        //if not create a save file
        m_saveFile.create();
        //and save an empty xml file
        m_valueTree.saveTreeToXml();
    } else
    {
        //if yes, load the save data from it
        m_audioLibrary = juce::XmlDocument::parse(m_saveFile);
        //And import the data into the valueTree
        m_valueTree.getTreeFromXml(m_audioLibrary.get());
        //Also need to add all the existing categories into the categoryListModel
        m_valueTree.addExistingCategories();
        //Finally, set everything visible
        m_valueTree.setAllVisible();
    }
    
    m_savedAudioFiles = m_saveFolder.getChildFile("SavedFiles");
    
    if(!m_savedAudioFiles.exists())
    {
        m_savedAudioFiles.createDirectory();
    }
    
    m_tempAudioFiles = m_saveFolder.getChildFile("TempFiles");
    
    if(!m_tempAudioFiles.exists())
    {
        m_tempAudioFiles.createDirectory();
    }

    m_table.m_formatManager.registerBasicFormats();

    m_commandManager.registerAllCommandsForTarget(this);
    m_commandManager.registerAllCommandsForTarget(&m_transport);
    m_commandManager.getKeyMappings()->resetToDefaultMappings();
    addKeyListener(m_commandManager.getKeyMappings());
    
    
    m_table.setModel(&m_tableModel);
    m_table.setOutlineThickness(1);
    m_table.setMultipleSelectionEnabled(false);
    m_table.getHeader().addColumn(juce::String(TRANS("File Name")), 1, 200, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
    m_table.getHeader().addColumn(juce::String(TRANS("Duration(Seconds)")), 2, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
    m_table.getHeader().addColumn(juce::String(TRANS("Sample Rate")), 3, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
    m_table.getHeader().addColumn(juce::String(TRANS("Channels")), 4, 70, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
    m_table.getHeader().addColumn(juce::String(TRANS("Description")), 5, 250, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
    
    getLookAndFeel().setColour(juce::ListBox::backgroundColourId, juce::Colours::black.withLightness(0.1));
    
    m_table.getHeader().setColour(juce::TableHeaderComponent::textColourId, juce::Colours::white);
    m_table.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, juce::Colours::darkred);
    m_table.getHeader().setColour(juce::TableHeaderComponent::outlineColourId, getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    m_table.setColour(juce::ListBox::outlineColourId, juce::Colours::darkred);
    m_searchBar.setColour(juce::Label::backgroundColourId, juce::Colours::black.withLightness(0.15));
    m_searchBar.setColour(juce::Label::outlineColourId, juce::Colours::darkred);
    m_searchButton.setColour(juce::DrawableButton::backgroundColourId, juce::Colours::black.withLightness(0.15));
    
    m_categories.setModel(&m_categoryModel);
    m_categories.setOutlineThickness(1);
    m_categories.setColour(juce::ListBox::outlineColourId, juce::Colours::darkred);
    
    getLookAndFeel().setColour(juce::ScrollBar::ColourIds::thumbColourId, juce::Colours::darkred);
    
    m_menuBar.setModel(this);
    setApplicationCommandManagerToWatch(&m_commandManager);
    
    m_searchBar.setEditable(false, true, false);
    m_searchBar.addListener(&m_valueTree);
    
    m_helpText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::black);
    m_helpText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::white);
    m_helpText.setFont(juce::Font(20));
    m_helpText.setMultiLine(true);
    m_helpText.setReadOnly(true);
    m_helpText.setCaretVisible(false);
    m_helpText.setSize(650, 800);
    
    m_searchButton.setImages(juce::Drawable::createFromImageData(BinaryData::MagGlassInactive_svg, BinaryData::MagGlassInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::MagGlassHover_svg, BinaryData::MagGlassHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::MagGlassActive_svg, BinaryData::MagGlassActive_svgSize).get());
    
    m_searchButton.onClick = [&]()
    {
        m_searchBar.showEditor();
    };
    
    m_searchButton.setTooltip(TRANS("Search, 'F'"));
    
    
    addAndMakeVisible(m_table);
    addAndMakeVisible(m_categories);
    addAndMakeVisible(m_menuBar);
    addAndMakeVisible(m_searchBar);
    addAndMakeVisible(m_transport);
    
    addAndMakeVisible(m_searchButton);

    m_categoryModel.addCategoryToList(TRANS("Ambiance"));
    m_categoryModel.addCategoryToList(TRANS("Impact"));
    m_categoryModel.addCategoryToList(TRANS("Gunshot"));
    m_categoryModel.addCategoryToList(TRANS("Footsteps"));
    m_categoryModel.addCategoryToList(TRANS("Weather"));
    
#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(this);
#endif
    
    setSize (900, 600);

}
 

MainComponent::~MainComponent()
{
#if JUCE_MAC
   juce::MenuBarModel::setMacMainMenu(nullptr);
#endif
    
    m_table.setModel(nullptr);
    m_categories.setModel(nullptr);
    //Deleteing the tempFiles folder to clean up just incase audiofile were left there
    if(m_tempAudioFiles.isDirectory()) m_tempAudioFiles.deleteRecursively();
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
}


void MainComponent::resized()
{
    
    auto table = getLocalBounds();
    auto header = table.removeFromTop(25);
    auto searchArea = header.removeFromRight(150);
    auto transport = table.removeFromBottom(150);
    auto category = table.removeFromLeft(100);
    
    m_categories.setBounds(category);
    m_table.setBounds(table);
    m_menuBar.setBounds(header);
    m_searchButton.setBounds(searchArea.removeFromLeft(25));
    m_searchBar.setBounds(searchArea);
    m_transport.setBounds(transport);
    
}

/*
 
 
 
 THIS SECTION IS FOR COMMANDS
 
 
 
 */

juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
    return &m_transport;
}

void MainComponent::getAllCommands(juce::Array<juce::CommandID> &commands)
{
    juce::Array<juce::CommandID> commandArray { CommandIDs::CopyOnImportYes,
                                                CommandIDs::CopyOnImportNo,
                                                CommandIDs::ImportFile,
                                                CommandIDs::SaveData,
                                                CommandIDs::AddCategory,
                                                CommandIDs::AudioSettings,
                                                CommandIDs::Features,
                                                CommandIDs::Search };
    commands.addArray(commandArray);
}

void MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result)
{
    switch (commandID)
    {
        case CommandIDs::CopyOnImportYes:
            result.setInfo("Copy On Import", "Application copies audio files on import", "Option Menu", 0);
            result.setTicked(m_table.m_copyOnImport);
            break;
            
        case CommandIDs::CopyOnImportNo:
            result.setInfo("Do Not Copy On Import", "Application does not copy audio files on improt", "Option Menu", 0);
            result.setTicked(!m_table.m_copyOnImport);
            break;
            
        case CommandIDs::ImportFile:
            result.setInfo("Import File", "Imports selected audio file", "File Menu", 0);
            break;
            
        case CommandIDs::SaveData:
            result.setInfo("Save Data", "Saves data to the xml element", "File Menu", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
            break;
            
        case CommandIDs::AddCategory:
            result.setInfo("Add Category", "Adds category to the category list", "Category Menu", 0);
            break;
            
        case CommandIDs::AudioSettings:
            result.setInfo("Audio Settings", "Opens audio device settings dialog window", "Options Menu", 0);
            break;
            
        case CommandIDs::Features:
            result.setInfo("Features", "Opens a file containing info about this application", "Features Menu", 0);
            break;
            
        case CommandIDs::Search:
            result.setInfo("Search", "Button for search bar", "Search Bar", 0);
            result.addDefaultKeypress('f', 0);
            break;
            
        default:
            DBG("Meh");
            break;
    }
}

bool MainComponent::perform (const juce::ApplicationCommandTarget::InvocationInfo &info)
{
    switch (info.commandID)
    {
        case CommandIDs::CopyOnImportYes:
            m_table.m_copyOnImport = true;
            
            break;
            
        case CommandIDs::CopyOnImportNo:
            m_table.m_copyOnImport = false;
            break;
            
        case CommandIDs::ImportFile:
            m_table.manualFileImport();
            break;
            
        case CommandIDs::SaveData:
            m_valueTree.saveTreeToXml();
            DBG("Saved");
            break;
            
        case CommandIDs::AddCategory:
            m_categoryModel.openCategoryTextEditor();
            break;
            
        case CommandIDs::AudioSettings:
            m_transport.openAudioSettings();
            break;
            
        case CommandIDs::Features:
            openHelpFile();
            break;
            
        case CommandIDs::Search:
            m_searchButton.triggerClick();
            break;
        default:
            return false;
    }
    return true;
}

void MainComponent::openHelpFile()
{
    std::unique_ptr<juce::MemoryInputStream> input;
    
    input.reset(new juce::MemoryInputStream(BinaryData::help_txt, BinaryData::help_txtSize, false));
    
    juce::String text = input->readEntireStreamAsString();
    
    m_helpText.setText(text);
    
    juce::DialogWindow::showDialog(TRANS("Features"), &m_helpText, this, juce::Colours::white, true);
}

/*
 
 
 
 THIS SECTION IS FOR DRAG AND DROP IMPORT
 
 
 
 */


//Gets called everytime a file is dragged over the table
bool DragAndDropTable::isInterestedInFileDrag(const juce::StringArray& files)
{        
    if(!m_acceptingfiles) return false;
    //Go through every file to see whether it is an wav or aiff file
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".aiff"))
        {
            return true;
        }
    }

    return false;
}

//This is called when a files are dropped and we are interested in them
void DragAndDropTable::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
           
            auto fileToTest = juce::File(file);
            
            try {
                if(fileToTest.isDirectory())throw TRANS("You've seleced a directory, please pick an audio file");
                if(!fileToTest.hasFileExtension("wav;aiff")   )throw TRANS("Please select wav or aiff file");
                if(!fileToTest.existsAsFile())throw TRANS("That file doesn't exist!!");
                
                auto* valueTree = &m_mainApp.m_valueTree.m_audioLibraryTree;
                
                if (valueTree->isValid())
                {
                
                    auto existingFileName = valueTree->getChildWithProperty(SoundManager::m_filePath, juce::var(fileToTest.getFullPathName()));
                
                    if (existingFileName.isValid())
                    {
                        throw TRANS("Cannot import duplicate files");
                    }
                }
                
                auto fileReader = m_formatManager.createReaderFor(fileToTest);
                if(fileReader == nullptr) throw TRANS("Error Loading the File");
                auto sampleRate =fileReader->sampleRate;
                double fileLength = std::round( fileReader->lengthInSamples / sampleRate);
                int numChannels = fileReader->numChannels;
                juce::String filePath;
                if(!m_copyOnImport)
                {
                    filePath = fileToTest.getFullPathName();
                } else
                {
                    auto newFile = m_saveLocation.getChildFile(fileToTest.getFileName());
                    if(!newFile.existsAsFile())
                    {
                        newFile.create();
                    } else
                    {
                        //Lets prevent a leak
                        delete fileReader;
                        //And lets also prevent the creation of dublicate files
                        throw TRANS("This File Already Exists In Save Folder");
                    }
                    if(!fileToTest.copyFileTo(newFile)) throw TRANS("Error Coping the File");
                    filePath = newFile.getFullPathName();
                }
                
                m_mainApp.m_valueTree.addFile(fileToTest, fileLength,sampleRate, numChannels, filePath);
                delete fileReader;

                
            } catch (juce::String message){
                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,fileToTest.getFileName() , message);
            }
        }
    }
}

/*
 
 
 
 THIS SECTION IS FOR THE TABLE
 
 
 
 
 
 */

int SoundTableModel::getNumRows()
{
    jassert(m_valueTreeToListen.m_audioLibraryTree.isValid());
    
    int numRows = m_valueTreeToListen.getNumFileTrees();
    return numRows;
}

void SoundTableModel::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::darkred);
    else
        g.fillAll(m_mainApp.getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
}

void SoundTableModel::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
   
    if(columnId == 1)
    {
        g.setColour(juce::Colours::white);
        g.drawText(m_valueTreeToListen.getInformationAtIndex(rowNumber, 1), 2, 0, width - 4, height, juce::Justification::centredLeft);
        g.setColour(m_mainApp.getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    }else if (columnId == 2)
    {
        //This column is Duration
        g.setColour(juce::Colours::white);
        g.drawText(m_valueTreeToListen.getInformationAtIndex(rowNumber, 2), 2, 0, width - 4, height, juce::Justification::centred);
        g.setColour(m_mainApp.getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
                
    } else if (columnId == 3)
    {
        //This column is sample rate
        g.setColour(juce::Colours::white);
        g.drawText(m_valueTreeToListen.getInformationAtIndex(rowNumber, 4), 2, 0, width - 4, height, juce::Justification::centred);
        g.setColour(m_mainApp.getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
                
    } else if (columnId == 4)
    {
        //This column is num channels
        g.setColour(juce::Colours::white);
        g.drawText(m_valueTreeToListen.getInformationAtIndex(rowNumber, 3), 2, 0, width - 4, height, juce::Justification::centred);
        g.setColour(m_mainApp.getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
                
    }
}
      
void SoundTableModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& mouseEvent)
{
    m_lastSelectedRow = rowNumber;
    
    if(mouseEvent.mods.isRightButtonDown())
    {
        if(mouseEvent.mods.isCommandDown())
        {
            juce::PopupMenu deleteCategoryMenu;
            
            deleteCategoryMenu.addSectionHeader(TRANS("Remove Category"));
            
            //Need to add function to get number of categories
            auto categories = m_valueTreeToListen.getCategories(m_lastSelectedRow);
            
            for(int i = 0; i < categories.size(); ++i)
            {
                auto title = categories[i];
                deleteCategoryMenu.addItem(i + 1, TRANS(title));
            }
            
            deleteCategoryMenu.showMenuAsync(juce::PopupMenu::Options(), [&](int selection)
                                             {
                                                auto categories = m_valueTreeToListen.getCategories(m_lastSelectedRow);
                                                auto categoryToDelete = categories[selection - 1];
                                                DBG(categoryToDelete);
                                                m_valueTreeToListen.deleteCategory(categoryToDelete, m_lastSelectedRow);
                                             });
        } else
        {
        juce::PopupMenu cellMenu;
        juce::PopupMenu cateMenu;
        
        cellMenu.addItem(1, TRANS("Delete Item"));
        cellMenu.addSeparator();
        
        cateMenu.addItem(2, TRANS("New Category"));
        cateMenu.addSeparator();
        
        for (int i = 0; i < m_mainApp.m_categoryModel.numCategories(); ++i)
        {
            auto title = m_mainApp.m_categoryModel.m_uniqueCategories[i];
            cateMenu.addItem(i + 3, TRANS(title));
        }
        
        
        cellMenu.addSubMenu(TRANS("Add To Category"), cateMenu);
        
        cellMenu.showMenuAsync(juce::PopupMenu::Options() ,  [&](int selection)
                               {
                                    m_mainApp.m_table.cellPopupAction(selection, m_lastSelectedRow, columnId);
                                });
        }
    }
}

void DragAndDropTable::backgroundClicked (const juce::MouseEvent&)
{
    deselectAllRows();
    m_mainApp.m_categories.deselectAllRows();
    m_mainApp.m_transport.noFileSelected();
}

void SoundTableModel::selectedRowsChanged(int lastRowSelected)
{
    m_lastSelectedRow = lastRowSelected;
    if(getNumRows() > 0)
    {
        m_mainApp.m_transport.setFileToPlay(m_valueTreeToListen.getFileOnRow(m_lastSelectedRow));
    } else
    {
        m_mainApp.m_transport.noFileSelected();
    }
}

juce::Component* SoundTableModel::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component *existingComponentToUpdate)
{

    if (columnId == 5)
    {
        auto* label = static_cast<SoundTableModel::CellLabel *>(existingComponentToUpdate);
        if (label == nullptr)
        {
            label = new SoundTableModel::CellLabel();
            label->setRow(rowNumber);
            label->setEditable(false, true, false);
            label->onTextChange = [&,label]()
            {
                juce::String newDescription = label->getText();
                DBG(juce::String(label->row));
                m_valueTreeToListen.updateDescription(newDescription, label->row);
            };
        }
        if(rowNumber != label->row)
        {
            label->setRow(rowNumber);
        }
        label->setText(m_valueTreeToListen.getInformationAtIndex(label->row, 5), juce::NotificationType::dontSendNotification);
        //existingComponentToUpdate = label;
        return label;
    }
    return nullptr;
}

void SoundTableModel::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    m_mainApp.m_table.updateContent();
    m_valueTreeToListen.saveTreeToXml();
}

void SoundTableModel::valueTreePropertyChanged(juce::ValueTree& parentTree, const juce::Identifier& property)
{
    m_mainApp.m_table.updateContent();
    m_valueTreeToListen.saveTreeToXml();
}

void SoundTableModel::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
{
    m_mainApp.m_table.updateContent();
    juce::String filename = childWhichHasBeenRemoved.getProperty(m_valueTreeToListen.m_filePath);
    juce::File removedFile(filename);
    if(removedFile.getParentDirectory() == m_mainApp.m_table.m_saveLocation)
    {
        removedFile.deleteFile();
    }
    m_mainApp.m_transport.noFileSelected();
    m_valueTreeToListen.saveTreeToXml();
}

void SoundTableModel::locateFile(juce::File file)
{
    auto fileToLocation = file;
    m_valueTreeToListen.setNewFilepath(file);

}

void SoundTableModel::reloadWaveform()
{
    m_mainApp.m_transport.setFileToPlay(m_valueTreeToListen.getFileOnRow(m_lastSelectedRow));
}

void SoundTableModel::preventFileImport()
{
    m_mainApp.m_table.m_acceptingfiles = false;
}

void SoundTableModel::allowFIleImport()
{
    m_mainApp.m_table.m_acceptingfiles = true;
}

/*
 
 
 
 THIS SECTION IS FOR THE MENUS
 
 
 
 
 */

juce::StringArray MainComponent::getMenuBarNames()
{
    return {"File", "Category", "Options", "Help" };
}

juce::PopupMenu MainComponent::getMenuForIndex (int menuIndex, const juce::String& menuName)
{
    juce::PopupMenu menu;
    
    if (menuIndex == 0)
    {
        //menu.addItem(1,"Import Audio File");
        menu.addCommandItem(&m_commandManager, CommandIDs::ImportFile);
        //menu.addItem(2, "Save Data");
        menu.addCommandItem(&m_commandManager, CommandIDs::SaveData);
    }
    
    if (menuIndex == 1)
    {
        //menu.addItem (2, "Add New Category");
        menu.addCommandItem(&m_commandManager, CommandIDs::AddCategory);
    }
    else if (menuIndex == 2)
    {
        juce::PopupMenu copyMenu;
        menu.addCommandItem(&m_commandManager, CommandIDs::AudioSettings);
        copyMenu.addCommandItem(&m_commandManager, CommandIDs::CopyOnImportYes);
        copyMenu.addCommandItem(&m_commandManager, CommandIDs::CopyOnImportNo);
        menu.addSubMenu(TRANS("Copy On Import"), copyMenu);
    }
    else if (menuIndex == 3)
    {
        //menu.addItem (3, "Readme");
        menu.addCommandItem(&m_commandManager, CommandIDs::Features);
    }
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    
}

void DragAndDropTable::manualFileImport()
{
    
    m_fileChooser = std::make_unique<juce::FileChooser>(TRANS("Load a file"),juce::File(),"*.wav; *.aiff"   );
    
    auto flags = juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode   ;
    
    m_fileChooser->launchAsync(flags, [&](const juce::FileChooser& chooser){
        
        juce::File fileToTest(chooser.getResult());
        
        try {
            if(fileToTest.isDirectory())throw TRANS("You've seleced a directory, please pick an audio file");
            if(!fileToTest.existsAsFile())throw TRANS("No file selected");
            
            auto* valueTree = &m_mainApp.m_valueTree.m_audioLibraryTree;
            
            if (valueTree->isValid())
            {
            
                auto existingFileName = valueTree->getChildWithProperty(SoundManager::m_filePath, juce::var(fileToTest.getFullPathName()));
            
                if (existingFileName.isValid())
                {
                    throw TRANS("Cannot import duplicate files");
                }
            }
            
            auto fileReader = m_formatManager.createReaderFor(fileToTest);
            if(fileReader == nullptr) throw TRANS("Error Loading the File");
            auto sampleRate =fileReader->sampleRate;
            double fileLength = std::round( fileReader->lengthInSamples / sampleRate);
            int numChannels = fileReader->numChannels;
            juce::String filePath = fileToTest.getFullPathName();
            
            
            m_mainApp.m_valueTree.addFile(fileToTest, fileLength,sampleRate, numChannels, filePath);
            delete fileReader;

        } catch (juce::String message){
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,fileToTest.getFileName() , message);
        }
    } );
}

void DragAndDropTable::cellPopupAction(int selection, int rowNumber, int columnId)
{
   //Delete item
   if(selection == 1)
   {
       m_mainApp.m_valueTree.removeFileInfoTree(rowNumber);
   }
   //New Category
   else if(selection == 2)
   {
       auto categoryTextEditor = std::make_unique<juce::Label>(TRANS("Category Name"));
       
       auto textEditorPos = getHeader().getColumnPosition(columnId);
       
       categoryTextEditor->setEditable(true, false, true);
       categoryTextEditor->setSize(200, 50);
       categoryTextEditor->addListener(&m_mainApp.m_categoryModel);
       
       juce::CallOutBox::launchAsynchronously(std::move(categoryTextEditor), textEditorPos, &m_mainApp);
   }
   //This checks the selection and selects a category to add to the item
   else
   {
       for (int i = 0; i < m_mainApp.m_categoryModel.numCategories(); ++i )
       {
           //Since 0, 1, 2 selections are already taken, we need to add 3 to i
           if(selection == i + 3)
           {
               auto categoryToAdd = m_mainApp.m_categoryModel.m_uniqueCategories[i];
               m_mainApp.m_valueTree.addCategory(categoryToAdd, m_mainApp.m_tableModel.m_lastSelectedRow);
           }
       }
   }
}

void CategoryListModel::listPopupAction()
{
    juce::String categoryToDelete = m_uniqueCategories[m_selectedRow];
    m_valueTreeToListen.categoryDeleted(categoryToDelete);
    m_uniqueCategories.erase(m_uniqueCategories.begin() + m_selectedRow);
    m_mainApp.m_categories.updateContent();
}


/*
 
 
 
 THIS SECTION IS FOR THE CATEGORIES
 
 
 
 
 */


int CategoryListModel::getNumRows()
{
    int numCategories = static_cast<int>(m_uniqueCategories.size());
    return numCategories;
}

void CategoryListModel::paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected)
{
 
    if (rowIsSelected)
        g.fillAll(juce::Colours::darkred);
    else
        g.setColour(m_mainApp.getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    
    g.setColour(juce::Colours::white);
    g.drawText(m_uniqueCategories[rowNumber], 2, 0, width - 4, height, juce::Justification::centredLeft);
    g.setColour(m_mainApp.getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
}

void CategoryListModel::listBoxItemClicked(int row, const juce::MouseEvent& mouseEvent)
{
    m_selectedRow = row;
    m_mainApp.m_table.deselectAllRows();
    m_mainApp.m_transport.noFileSelected();
    juce::String categoryName = m_uniqueCategories[m_selectedRow];
    m_mainApp.m_valueTree.filterByCategory(categoryName);
    
    if(mouseEvent.mods.isRightButtonDown())
    {
        juce::PopupMenu listMenu;
        
        listMenu.addItem(1, TRANS("Delete Category"));
        
        listMenu.showMenuAsync(juce::PopupMenu::Options() ,  [&](int selection)
                           {
                            listPopupAction();
                            });
    }
    
    if(mouseEvent.mods.isCommandDown())
    {
        m_mainApp.m_categories.deselectAllRows();
        m_mainApp.m_valueTree.setAllVisible();
    }
}

void CategoryListModel::selectedRowsChanged(int lastRowSelected)
{
    m_mainApp.m_valueTree.setAllVisible();
    m_selectedRow = lastRowSelected;
    //If no row is selected m_selectedRow is set to -1 so we need to check whether this is the case, else the vector will crash
    if(m_selectedRow < 0) return;
    juce::String categoryName = m_uniqueCategories[lastRowSelected];
    m_mainApp.m_valueTree.filterByCategory(categoryName);
}

void CategoryListModel::addCategoryToList(juce::String name)
{
    if (categoryExists(name)) return;
    m_uniqueCategories.push_back(name);
    m_mainApp.m_categories.updateContent();
}

bool CategoryListModel::categoryExists(juce::String category)
{
    for(int i = 0 ; i < m_uniqueCategories.size();++i)
    {
        if(  m_uniqueCategories[i].equalsIgnoreCase(category)   ) return true;
    }
    return false;
}

int CategoryListModel::numCategories()
{
    return static_cast<int>(m_uniqueCategories.size());
}

void CategoryListModel::labelTextChanged(juce::Label* labelThatHasChanged)
{
    auto newCategory = labelThatHasChanged->getText();
    addCategoryToList(newCategory);
}

void CategoryListModel::openCategoryTextEditor()
{
    auto categoryTextEditor = std::make_unique<juce::Label>();
    
    auto headerPos = m_mainApp.getLocalBounds().removeFromTop(20);
    
    auto textEditorPos = headerPos.removeFromLeft(20);
    
    categoryTextEditor->setEditable(true, false, true);
    categoryTextEditor->setSize(200, 50);
    categoryTextEditor->addListener(this);
    
    juce::CallOutBox::launchAsynchronously(std::move(categoryTextEditor), textEditorPos, &m_mainApp);
}
