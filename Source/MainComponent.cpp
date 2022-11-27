#include "MainComponent.h"
#include "SoundManager.h"

//==============================================================================
MainComponent::MainComponent() : juce::AudioAppComponent(m_audioDeviceManager),
                                 m_tableModel(*this, m_valueTree),
                                 m_table(*this),
                                 m_valueTree(*this),
                                 m_categories("categories", nullptr),
                                 m_categoryModel(*this, m_valueTree),
                                 m_transport(m_tableModel),
                                 m_searchButton("Search", juce::DrawableButton::ButtonStyle::ImageRaw),
                                 m_audioLibrary(std::make_unique<juce::XmlElement>("audiolibrary")),
                                 m_saveFile(juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory).getChildFile("SoundManager"))
{
    // Make sure you set the size of the component after
    // you add any child components
    
    //Check whether application folder already exists in application data directory
    if(!m_saveFile.exists())
    {
        {
            //if not, create one
            m_saveFile.createDirectory();
        }
    }
    //check whether a save file already exists
    m_saveFile = m_saveFile.getChildFile("audiolibrary.xml");
    
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
    
    m_table.m_formatManager.registerBasicFormats();
    m_audioDeviceManager.initialise(0, 2, nullptr, true);
    
    m_audioSettings = std::make_unique<juce::AudioDeviceSelectorComponent>(m_audioDeviceManager, 0, 0, 0, 2, false, false, false, true);
    
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
    
    m_searchBar.setEditable(false, true, false);
    m_searchBar.addListener(&m_valueTree);
    
    
    m_searchButton.setImages(juce::Drawable::createFromImageData(BinaryData::MagGlassInactive_svg, BinaryData::MagGlassInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::MagGlassHover_svg, BinaryData::MagGlassHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::MagGlassActive_svg, BinaryData::MagGlassActive_svgSize).get());
    
    m_searchButton.addShortcut(juce::KeyPress('f'));
    
    m_searchButton.onClick = [&]()
    {
        m_searchBar.showEditor();
    };
    
    m_searchButton.setTooltip("Search");
    
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

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }    
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    
#if JUCE_MAC
   juce::MenuBarModel::setMacMainMenu(nullptr);
#endif
    
    m_table.setModel(nullptr);
    m_categories.setModel(nullptr);
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    
    /*
    bufferToFill.clearActiveBufferRegion();
    
    if(!m_playSoundFile) return;
    
    auto numSamples = bufferToFill.numSamples;
    float* leftChannel = bufferToFill.buffer->getWritePointer(0);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1);
    
    for(int n = 0 ; n < numSamples; ++n)
    {
        leftChannel[n] = m_sampleBuffer.getSample(0, m_playPosition);
        rightChannel[n] = m_sampleBuffer.getSample(1, m_playPosition);
        
        
        ++m_playPosition;
        if(m_playPosition >= m_sampleBuffer.getNumSamples())m_playPosition = 0;
        
    }
     */
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
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
                juce::String filePath = fileToTest.getFullPathName();
                
                
                m_mainApp.m_valueTree.AddFile(fileToTest, fileLength,sampleRate, numChannels, filePath);
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
                                    cellPopupAction(selection, m_lastSelectedRow, columnId, mouseEvent);
                                });
        }
    }
}

void DragAndDropTable::backgroundClicked (const juce::MouseEvent&)
{
    deselectAllRows();
    m_mainApp.m_categories.deselectAllRows();
}

void SoundTableModel::selectedRowsChanged(int lastRowSelected)
{
    m_lastSelectedRow = lastRowSelected;

    m_mainApp.m_transport.setFileToPlay(m_valueTreeToListen.getFileOnRow(m_lastSelectedRow));
}

juce::Component* SoundTableModel::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component *existingComponentToUpdate)
{
    if (columnId ==5)
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
                m_valueTreeToListen.updateDescription(newDescription, label->row);
            };
        }
        label->setText(m_valueTreeToListen.getInformationAtIndex(label->row, 5), juce::NotificationType::dontSendNotification);
        existingComponentToUpdate = label;
    }
    return existingComponentToUpdate;
}

void SoundTableModel::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    m_mainApp.m_table.updateContent();
}

void SoundTableModel::valueTreePropertyChanged(juce::ValueTree& parentTree, const juce::Identifier& property)
{
   m_mainApp.m_table.updateContent();
}

void SoundTableModel::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
{
    m_mainApp.m_table.updateContent();
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
        menu.addItem(1,"Import Audio File");
        menu.addItem(2, "Save Data");
    }
    
    if (menuIndex == 1)
    {
        menu.addItem (2, "Add New Category");
    }
    else if (menuIndex == 2)
    {
        menu.addItem (2, "Audio Settings");
        menu.addItem (3, "Print Tree");
        menu.addItem (4, "Print XML Data");
    }
    else if (menuIndex == 3)
    {
        menu.addItem (3, "ReadMe");
    }
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    juce::PopupMenu menu;
    //File has been clicked
    if (topLevelMenuIndex == 0)
    {
        //Import Audio File
        if (menuItemID == 1)
        {
            manualFileImport();
        }
        //Save Data
        if(menuItemID == 2)
        {
            m_valueTree.saveTreeToXml();
            DBG("Data Saved To XML");
        }
    }
    //Category has been clicked
    else if (topLevelMenuIndex == 1)
    {
        //Add new category
        if (menuItemID == 2)
        {
            auto categoryTextEditor = std::make_unique<juce::Label>(TRANS("Category Name"));
            
            auto headerPos = getLocalBounds().removeFromTop(20);
            
            auto textEditorPos = headerPos.removeFromLeft(20);
            
            categoryTextEditor->setEditable(true, false, true);
            categoryTextEditor->setSize(200, 50);
            categoryTextEditor->addListener(&m_categoryModel);
            
            juce::CallOutBox::launchAsynchronously(std::move(categoryTextEditor), textEditorPos, this);
            
            
        }
        
    }
    //Options has been clicked
    else if (topLevelMenuIndex == 2)
    {
        //Open Audio device manager
        if( menuItemID == 2)
        {
            m_audioSettings = std::make_unique<juce::AudioDeviceSelectorComponent>(m_audioDeviceManager, 0, 0, 0, 2, false, false, false, false);
            m_audioSettings->setSize(400, 600);
            juce::DialogWindow::showDialog("Audio settings",m_audioSettings.get() , this, juce::Colours::black, true);
            
        }
        //Print Tree
        if (menuItemID == 3)
        {
            DBG(m_valueTree.printValueTree());
        }
        //Print XML Data
        if (menuItemID == 4)
        {
            printXmlContent();
        }
    }
    //Help has been clicked
    else if (topLevelMenuIndex == 3)
    {
        //Open readme
        if (menuItemID == 3)
        {
            
        }
    }
}

void MainComponent::manualFileImport()
{
    
    m_fileChooser = std::make_unique<juce::FileChooser>(TRANS("Load a file"),juce::File(),"*.wav; *.aiff"   );
    
    auto flags = juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode   ;
    
    m_fileChooser->launchAsync(flags, [&](const juce::FileChooser& chooser){
        
        juce::File fileToTest(chooser.getResult());
        
        try {
            if(fileToTest.isDirectory())throw TRANS("You've seleced a directory, please pick an audio file");
            if(!fileToTest.existsAsFile())throw TRANS("No file selected");
            
            auto* valueTree = &m_valueTree.m_audioLibraryTree;
            
            if (valueTree->isValid())
            {
            
                auto existingFileName = valueTree->getChildWithProperty(SoundManager::m_filePath, juce::var(fileToTest.getFullPathName()));
            
                if (existingFileName.isValid())
                {
                    throw TRANS("Cannot import duplicate files");
                }
            }
            
            auto fileReader = m_table.m_formatManager.createReaderFor(fileToTest);
            if(fileReader == nullptr) throw TRANS("Error Loading the File");
            auto sampleRate =fileReader->sampleRate;
            double fileLength = std::round( fileReader->lengthInSamples / sampleRate);
            int numChannels = fileReader->numChannels;
            juce::String filePath = fileToTest.getFullPathName();
            
            
            m_valueTree.AddFile(fileToTest, fileLength,sampleRate, numChannels, filePath);
            delete fileReader;

            
        } catch (juce::String message){
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,fileToTest.getFileName() , message);
        }
        
        
        
    } );
    
    
}

void SoundTableModel::cellPopupAction(int selection, int rowNumber, int columnId, const juce::MouseEvent& mouseEvent)
{
    // 1 = Delete
    // 2 = New Category
    // 3 = Ambiance
    // 4 = Impact
    // 5 = Gunshot
    
   //int numCategories = m_mainApp.m_categoryModel.numCategories();
   if(selection == 1)
   {
       m_valueTreeToListen.removeFileInfoTree(rowNumber);
   }
   else if(selection == 2)
   {
       auto categoryTextEditor = std::make_unique<juce::Label>(TRANS("Category Name"));
       
       auto textEditorPos = m_mainApp.m_table.getHeader().getColumnPosition(columnId);
       
       categoryTextEditor->setEditable(true, false, true);
       categoryTextEditor->setSize(200, 50);
       categoryTextEditor->addListener(&m_mainApp.m_categoryModel);
       
       juce::CallOutBox::launchAsynchronously(std::move(categoryTextEditor), textEditorPos, &m_mainApp);
   }
   else
   {
       for (int i = 0; i < m_mainApp.m_categoryModel.numCategories(); ++i )
       {
           if(selection == i + 3)
           {
               auto categoryToAdd = m_mainApp.m_categoryModel.m_uniqueCategories[i];
               m_valueTreeToListen.addCategory(categoryToAdd, m_lastSelectedRow);
               
           }
       }
       
   }
       
    
}

void CategoryListModel::listPopupAction()
{
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
        //m_mainApp.m_categories.deselectRow(row);
        m_mainApp.m_valueTree.setAllVisible();
    }
}

void CategoryListModel::selectedRowsChanged(int lastRowSelected)
{
    m_mainApp.m_valueTree.setAllVisible();
    m_selectedRow = lastRowSelected;
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

void MainComponent::AddNewCategory(juce::String newCategory)
{
    m_categoryModel.addCategoryToList(newCategory);
    m_categories.updateContent();
}

void CategoryListModel::labelTextChanged(juce::Label* labelThatHasChanged)
{
    auto newCategory = labelThatHasChanged->getText();
    addCategoryToList(newCategory);
}


/*
 
 
 
 THIS SECTION IS FOR THE DATA STRUCTURE
 
 
 
 
 */


void MainComponent::printXmlContent()
{
    auto xmlString = m_audioLibrary->toString();
    DBG("**************");
    DBG(xmlString);
    DBG("**************");
}

void MainComponent::loadXmlContent()
{
    m_audioLibrary = juce::XmlDocument::parse(m_saveFile);
}
