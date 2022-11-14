#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : m_table(*this)
{
    // Make sure you set the size of the component after
    // you add any child components

    m_formatManager.registerBasicFormats();
  
    m_playStop.setButtonText(TRANS("Play"));
    m_playStop.setEnabled(false);

    m_table.setModel(this);
    m_table.setOutlineThickness(1);
    m_table.getHeader().addColumn(juce::String(TRANS("File Name")), 1, 200);
    m_table.getHeader().addColumn(juce::String(TRANS("Duration(Seconds)")), 2, 100);
    m_table.getHeader().addColumn(juce::String(TRANS("Sample Rate")), 3, 100);
    m_table.getHeader().addColumn(juce::String(TRANS("Channels")), 4, 70);
    m_table.getHeader().addColumn(juce::String(TRANS("Description")), 5, 250);
    
    addAndMakeVisible(m_playStop);
    addAndMakeVisible(m_table);
    
    m_playStop.onClick = [&]()
    {
        m_playSoundFile = !m_playSoundFile;
        if(m_playSoundFile) {
            m_playStop.setButtonText(TRANS("Pause"));
        } else {
            m_playStop.setButtonText(TRANS("Play"));
        }
    };
    


    setSize (800, 600);

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
    m_table.setModel(nullptr);
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::darkgrey);

    // You can add your drawing code here!
 
}


void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    auto transpControl = bounds.removeFromBottom(35);
   // auto categories = bounds.removeFromLeft(getWidth() / 6);

    m_playStop.setBounds(transpControl);
    m_table.setBounds(bounds);
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
                if(!fileToTest.hasFileExtension("wav")   )throw TRANS("Please select a Wav file");
                if(!fileToTest.existsAsFile())throw TRANS("That file doesn't exist!!");
                
                auto fileReader = m_mainApp.m_formatManager.createReaderFor(fileToTest);
                if(fileReader == nullptr) throw TRANS("Error Loading the File");
                auto sampleRate =fileReader->sampleRate;
                double fileLength = fileReader->lengthInSamples / sampleRate;
                int numChannels = fileReader->numChannels;
                juce::String filePath = fileToTest.getFullPathName();
                
                
                showFile(fileToTest, fileLength,sampleRate, numChannels, filePath);
                delete fileReader;

                
            } catch (juce::String message){
                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,fileToTest.getFileName() , message);
            }
        }
    }
}


void DragAndDropTable::showFile(juce::File& file, double length, double sampleRate, int numChannels, juce::String filePath)
{
    m_fileArray.push_back(FileInfo(file,length,sampleRate, numChannels, filePath));
    ++m_numRows;
    updateContent();
}

void MainComponent::prepFileToPlay(int rowNumber)
{
    if(rowNumber >= 0 && rowNumber < m_table.m_fileArray.size()){
        auto fileReader = m_formatManager.createReaderFor(m_table.m_fileArray[rowNumber].file);
    
        m_sampleBuffer.setSize(2, static_cast<int>(fileReader->lengthInSamples));
    
        auto useRightChannel = fileReader->numChannels > 1;
        
        fileReader->read(&m_sampleBuffer, 0, static_cast<int>(fileReader->lengthInSamples), 0, true, useRightChannel);
    
        m_playStop.setEnabled(true);
    
        delete fileReader;
    }
}

    /*
     
     
     
     THIS SECTION IS FOR DRAG AND DROP EXPORT
     
     
     
     */




void DragAndDropTable::dragExport()
{
    if (!m_selectedFile.exists())
    {
        m_isDragging = false;
        return;
    }
    m_acceptingfiles = false;
    performExternalDragDropOfFiles(m_selectedFile.getFullPathName(), false, this, [this](){
        m_acceptingfiles = true;
        deselectAllRows();
    }
    );
    juce::Timer::callAfterDelay(500, [&](){m_isDragging = false;});
}

/*
 
 
 
 THIS SECTION IS FOR THE TABLE
 
 
 
 
 
 */

int MainComponent::getNumRows()
{
    
    return m_table.m_numRows;
}

void MainComponent::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::darkred);
    else
        g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setFont(m_table.m_font);
    if(columnId == 1){
    g.setColour(juce::Colours::white);
    g.drawText(m_table.m_fileArray[rowNumber].file.getFileName(), 2, 0, width - 4, height, juce::Justification::centredLeft);
    g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    } else if (columnId == 2)
    {
        g.setColour(juce::Colours::white);
        g.drawText(juce::String(m_table.m_fileArray[rowNumber].lengthInSeconds), 2, 0, width - 4, height, juce::Justification::centred);
        g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    } else if (columnId == 3)
    {
        g.setColour(juce::Colours::white);
        g.drawText(juce::String(m_table.m_fileArray[rowNumber].sampleRate), 2, 0, width - 4, height, juce::Justification::centred);
        g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    } else if (columnId == 4)
    {
        g.setColour(juce::Colours::white);
        g.drawText(juce::String(m_table.m_fileArray[rowNumber].numChannels), 2, 0, width - 4, height, juce::Justification::centred);
        g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    }
    
}

void MainComponent::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& mouseEvent)
{
    if(mouseEvent.mods.isRightButtonDown())
    {
        juce::PopupMenu cellMenu;
        juce::PopupMenu cateMenu;
    
        
        cellMenu.addItem(1,TRANS("Delete Item"));
        cellMenu.addSeparator();
        
        
        cateMenu.addItem(2, TRANS("Print Categories"));
        cateMenu.addSeparator();
        cateMenu.addItem(3, TRANS("Ambiance"));
        cateMenu.addItem(4, TRANS("Impact"));
        cateMenu.addItem(5, TRANS("GunShot"));
        
        cellMenu.addSubMenu(TRANS("Add to Category"), cateMenu);
         
        cellMenu.showMenuAsync(juce::PopupMenu::Options() ,  [&](int selection)
                           {
                                cellPopupAction(selection, rowNumber);
                           }
        );
        
    }else if (mouseEvent.mods.isCommandDown())
    {
        if(m_table.m_isDragging) return;
        m_table.m_isDragging = true;
        juce::File file = m_table.m_fileArray[rowNumber].file;
        m_table.m_selectedFile = file;
        m_table.dragExport();
        m_table.deselectAllRows();
    
    }
    
}

void DragAndDropTable::backgroundClicked (const juce::MouseEvent&)
{
    deselectAllRows();
}

void MainComponent::selectedRowsChanged(int lastRowSelected)
{
    prepFileToPlay(lastRowSelected);
}

juce::Component* MainComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component *existingComponentToUpdate)
{
    if (columnId == 5)
    {
        juce::Label * label = static_cast<juce::Label *>(existingComponentToUpdate);
        if (label == nullptr)
        {
            label = new juce::Label;
            label->onTextChange = [&,label ]()
            {
                jassert(label != nullptr);
                
                juce::String newDescription = label->getText();
                m_table.updateDescription(newDescription, rowNumber);
                m_table.printFileArray();
            };
            
        }
        label->setEditable(false, true, false);
        existingComponentToUpdate = label;
    }
    return existingComponentToUpdate;
}

void DragAndDropTable::updateDescription(juce::String newString, int rowNum)
{
    m_fileArray[rowNum].changeDescription(newString);
}

void DragAndDropTable::printFileArray()
{
    int arrayLength =  static_cast<int>(m_fileArray.size());
    DBG("*******************************");
    DBG("Number of Elements: " + juce::String(arrayLength));
    for(int i = 0; i < arrayLength; ++i)
 {
     m_fileArray[i].printInfo();
 }
    DBG("*******************************");
}

/*
 
 
 
 THIS SECTION IS FOR THE Menus
 
 
 
 
 */

void MainComponent::cellPopupAction(int selection, int rowNumber)
{
    // 1 = Delete
    // 2 = New Category
    // 3 = Ambiance
    // 4 = Impact
    // 5 = Gunshot
   if(selection == 1)
   {
       DBG("Delete This Row!");
       m_table.updateContent();
   } else if(selection == 2)
   {
       m_table.m_fileArray[rowNumber].printCategories();
   }
   else if(selection == 3)
   {
       m_table.m_fileArray[rowNumber].addCategory("Ambiance");
       
   }
   else if(selection == 4)
   {
       m_table.m_fileArray[rowNumber].addCategory("Impact");
   }
   else if(selection == 5)
   {
       m_table.m_fileArray[rowNumber].addCategory("GunShot");
   }
}



