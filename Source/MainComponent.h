#pragma once

#include <JuceHeader.h>
#include "DragAndDropTable.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
    friend class DragAndDropTable;
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;


private:
    //==============================================================================
    // Your private member variables go here...

  
    DragAndDropTable m_table;
    
    juce::AudioBuffer<float> m_sampleBuffer;
    juce::AudioFormatManager m_formatManager;
    juce::TextButton m_playStop;
    
    bool m_playSoundFile = false;
    int m_playPosition = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
