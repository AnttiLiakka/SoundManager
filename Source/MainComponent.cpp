#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : m_table(*this)
{
    // Make sure you set the size of the component after
    // you add any child components.    

    

    m_playStop.setButtonText(TRANS("Play"));
    m_playStop.setEnabled(false);   

    addAndMakeVisible(m_playStop);
    addAndMakeVisible(m_table);

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

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    auto transpControl = bounds.removeFromBottom(50);

    m_playStop.setBounds(transpControl);
    m_table.setBounds(bounds);
}

void DragAndDropTable::resized()
{
    m_fileInfo.setBounds(getLocalBounds().removeFromBottom(50));
}



//Gets called everytime a file is dragged over the table
bool DragAndDropTable::isInterestedInFileDrag(const juce::StringArray& files)
{    
    
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

//Is called when a files are dropped and we are interested in them
void DragAndDropTable::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            //load file
            loadDroppedFile(file);

            //store its information
            m_selectedFile = file;
           
           
        }
    }
}

//Returns the file path for dropped file
void DragAndDropTable::loadDroppedFile(const juce::String& path)
{
    auto file = juce::File(path);
   // createReaderFor(*file);
    DBG("file dropped");
    showFile(file);
}

void DragAndDropTable::mouseDown(const juce::MouseEvent& event)
{    
    dragExport();
    
}

void DragAndDropTable::dragExport()
{
    if (!m_selectedFile.exists()) return;   
    performExternalDragDropOfFiles(m_selectedFile.getFullPathName(), false);

    

}

void DragAndDropTable::showFile(juce::File& file)
{
    m_fileInfo.setButtonText(file.getFileName());
    addAndMakeVisible(m_fileInfo);
    m_fileInfo.setEnabled(false);
}








