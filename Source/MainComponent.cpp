#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : m_table(*this)
{
    // Make sure you set the size of the component after
    // you add any child components

    m_formatManager.registerBasicFormats();
  
    m_playStop.setButtonText(TRANS("Play"));
    m_playStop.setEnabled(false);
    
    m_table.setColour(juce::ListBox::outlineColourId, juce::Colours::yellow);
    m_table.setOutlineThickness(1);
    m_table.getHeader().setSortColumnId(1, true);
    m_table.setMultipleSelectionEnabled(true);
    m_table.getHeader().addColumn(juce::String(TRANS("File name")), m_table.m_columnId, 200);
    
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
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
 
}

void DragAndDropTable::paint (juce::Graphics& g)
{
    paintRowBackground(g, 1, 20, 20, true);
    paintCell(g, 1, m_columnId, getHeader().getColumnWidth(m_columnId), 100, true);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    //auto headerBar = bounds.removeFromTop(30);
    auto transpControl = bounds.removeFromBottom(35);

    m_playStop.setBounds(transpControl);
    m_table.setBounds(bounds);
}

void DragAndDropTable::resized()
{
    m_fileInfo.setBounds(getLocalBounds().removeFromBottom(50));
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
           
            auto testingFile = juce::File(file);
            
            try {
                if(testingFile.isDirectory())throw TRANS("You've seleced a directory, please pick an audio file");
                if(!testingFile.hasFileExtension("wav")   )throw TRANS("Please select a Wav file");
                if(!testingFile.existsAsFile())throw TRANS("That file doesn't exist!!");
                
                
                //load file
                loadDroppedFile(file);
                
                //store its information
                m_selectedFile = file;
                
                
            } catch (juce::String message){
                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,TRANS("Loading Sound Error") , message);
            }
        }
    }
}


void DragAndDropTable::loadDroppedFile(const juce::String& path)
{
    
    auto file = juce::File(path);
    auto fileReader = m_mainApp.m_formatManager.createReaderFor(file);
  
   
    
    m_mainApp.m_sampleBuffer.setSize(fileReader->numChannels,fileReader->lengthInSamples);
    
    fileReader->read(&m_mainApp.m_sampleBuffer, 0, fileReader->lengthInSamples, 0, true, true);
    
    m_mainApp.m_playStop.setEnabled(true);
    
    delete fileReader;
    DBG("file dropped");
    m_text = file.getFileName();
    updateContent();
    DBG(m_text);
    
    showFile(file);
    
    
    /*
     
     
     
     THIS SECTION IS FOR DRAG AND DROP EXPORT
     
     
     
     */
}

void DragAndDropTable::mouseDown(const juce::MouseEvent& event)
{
    m_acceptingfiles = false;
    dragExport();
}

void DragAndDropTable:: mouseUp (const juce::MouseEvent &event)
{
    m_acceptingfiles = true;
}

void DragAndDropTable::dragExport()
{
    if (!m_selectedFile.exists()) return;   
    performExternalDragDropOfFiles(m_selectedFile.getFullPathName(), false);
}

void DragAndDropTable::showFile(juce::File& file)
{
   // m_fileInfo.setButtonText(file.getFileName());
    //addAndMakeVisible(m_fileInfo);
    //m_fileInfo.setEnabled(false);
    updateContent();
}


/*
 
 
 
 THIS SECTION IS FOR THE TABLE
 
 
 
 
 
 */

int DragAndDropTable::getNumRows()
{
    
    return m_numRows;
}

void DragAndDropTable::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{
    g.fillAll (juce::Colours::grey);
}

void DragAndDropTable::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if(columnId == 1){
    g.setFont(font);
    g.setColour(juce::Colours::white);
    g.drawText(m_text, 2, 0, width - 4, height, juce::Justification::centredLeft);
    g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    }
    
}

void DragAndDropTable::cellClicked(int rowNumber, int columnId, const juce::MouseEvent&)
{
    DBG("ITS WORKING");
}





