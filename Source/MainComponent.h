#pragma once

#include <JuceHeader.h>
#include "DragAndDropTable.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, juce::TableListBoxModel
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

    void prepFileToPlay(int rowNumber);
    
    //TableListBoxModel =0 virtuals
    void paintRowBackground(juce::Graphics& p, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    int  getNumRows() override;


    //Other TableListBoxModel functions
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& mouseEvent) override;
    void selectedRowsChanged(int lastRowSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component *existingComponentToUpdate) override;


private:
    //==============================================================================
    // Your private member variables go here...

  
    DragAndDropTable m_table;
    
    juce::AudioBuffer<float> m_sampleBuffer;
    juce::AudioFormatManager m_formatManager;
    juce::TextButton m_playStop;
    
    bool m_playSoundFile = false;
    int m_playPosition = 0;

    //For TableModel
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
