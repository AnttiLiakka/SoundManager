/*
  ==============================================================================

    TransportEditor.cpp
    Created: 25 Nov 2022 2:15:23pm
    Author:  Antti

  ==============================================================================
*/

#include "MainComponent.h"

TransportEditor::TransportEditor(class SoundTableModel& tableModel) :
                 m_tableModel(tableModel),
                 m_thumbnailCache(5),
                 m_thumbnail(1024,
                 m_formatManager, m_thumbnailCache),
                 m_playButton(TRANS("Play"), juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_stopButton(TRANS("Stop"), juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_loopButton(TRANS("Loop"), juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_relocateButton(TRANS("Locate File"), juce::DrawableButton::ButtonStyle::ImageFitted),
                 playState(Stopped)
{
    
    m_formatManager.registerBasicFormats();
    
    m_thumbnail.addChangeListener(this);
    

    m_playButton.setImages(juce::Drawable::createFromImageData(BinaryData::playInactive_svg, BinaryData::playInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playHover_svg, BinaryData::playHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playActive_svg, BinaryData::playActive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playActive_svg, BinaryData::playActive_svgSize).get());
    
    m_stopButton.setImages(juce::Drawable::createFromImageData(BinaryData::stopInactive_svg, BinaryData::stopInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::stopHover_svg, BinaryData::stopHover_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::stopActive_svg, BinaryData::stopActive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::stopActive_svg, BinaryData::stopActive_svgSize).get());
    
    m_loopButton.setImages(juce::Drawable::createFromImageData(BinaryData::LoopInactive_svg, BinaryData::LoopInactive_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::LoopHover_svg, BinaryData::LoopHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::LoopActive_svg, BinaryData::LoopActive_svgSize).get());
    
    m_relocateButton.setImages(juce::Drawable::createFromImageData(BinaryData::LocateInactive_svg, BinaryData::LocateInactive_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::LocateHover_svg, BinaryData::LocateHover_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::LocateActive_svg, BinaryData::LocateActive_svgSize).get());
    
    m_relocateButton.setEnabled(false);
    m_relocateButton.setAlpha(0);

    m_playButton.setTooltip(TRANS("Play"));
    m_loopButton.setTooltip(TRANS("Loop"));
    m_stopButton.setTooltip(TRANS("Stop"));
    m_relocateButton.setTooltip(TRANS("Locate File"));
    m_stopButton.setEnabled(false);
    
    m_playButton.onClick = [&](){
        if(playState == Playing)
        {
            changePlayState(Pausing);
        } else
        {
            changePlayState(Starting);
        }
    };

    m_stopButton.onClick = [&](){
        changePlayState(Stopping);
    };
    
    m_loopButton.onClick = [&](){
        m_isLooping = !m_isLooping;
    };
    
    m_relocateButton.onClick = [&]()
    {
        (m_fileToPlay.getFullPathName());
         m_tableModel.locateFile(m_fileToPlay);
    };
    
    
    
    addAndMakeVisible(m_playButton);
    addAndMakeVisible(m_stopButton);
    addAndMakeVisible(m_loopButton);
    addAndMakeVisible(m_relocateButton);
}


void TransportEditor::paint(juce::Graphics &g)
{
    auto waveformBounds = getLocalBounds();
    auto controls = waveformBounds.removeFromBottom(30);
    g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    g.fillRect(waveformBounds);
    g.setFont(juce::Font(25));
    
    if(!m_fileSelected)
    {
        g.setColour(juce::Colours::darkred);
        g.drawFittedText(TRANS("File Not Selected"), waveformBounds, juce::Justification::centred, 1);
        
    } else if (!m_fileIsValid)
    {
        paintRelocateFile(g, waveformBounds);
    } else
    {
        paintLoadedFile(g, waveformBounds);
    }
}

void TransportEditor::resized()
{
    auto bounds = getLocalBounds();
    auto controls = bounds.removeFromBottom(30);
    auto buttonMargin = controls.removeFromBottom(4);
    auto sliders = controls.removeFromLeft(getWidth() / 2 - 52.5);
    auto relocateButtonBottonMargin = bounds.removeFromBottom(getLocalBounds().getHeight() / 3.5);
    auto relocateButtonTopMargin = bounds.removeFromTop(getLocalBounds().getHeight() / 3.5);
    m_relocateButton.setBounds(bounds.removeFromRight(bounds.getWidth() / 2.5));
    
    m_playButton.setBounds(controls.removeFromLeft(35));
    m_stopButton.setBounds(controls.removeFromLeft(35));
    m_loopButton.setBounds(controls.removeFromLeft(35));
}

void TransportEditor::setFileToPlay(juce::File file)
{
    m_fileSelected = true;
    m_fileToPlay = file;
    if (playState != Stopped)
    {
        changePlayState(Stopping);
    }
    
    if(!isFileValid(file))
    {
        m_relocateButton.setEnabled(true);
        m_relocateButton.setAlpha(1);
        m_fileIsValid = false;
        m_thumbnail.clear();
        stopTimer();
    } else
    {
        m_relocateButton.setEnabled(false);
        m_relocateButton.setAlpha(0);
        m_thumbnail.setSource(new juce::FileInputSource(file));
        m_fileIsValid = true;
        if(!isTimerRunning()) startTimer(40);
    }
}

void TransportEditor::paintLoadedFile(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(juce::Colours::white);
    m_thumbnail.drawChannel(g, bounds, 0, m_thumbnail.getTotalLength(), m_thumbnail.getNumChannels(), 1.0f);
    m_thumbnail.drawChannels(g, bounds, 0, m_thumbnail.getTotalLength(), 1.0f);
}

void TransportEditor::changePlayState(PlayState newPlayState)
{
    if (playState != newPlayState)
    {
        playState = newPlayState;
        
        switch (playState)
        {
            case Stopped:
                m_stopButton.setEnabled(false);
                
                break;
                
            case Starting:
                
                break;
                
            case Playing:
                m_stopButton.setEnabled(true);
                break;
                
            case Pausing:
                
                break;
                
            case Paused:
                m_stopButton.setEnabled(true);
                break;
                
            case Stopping:
    
                break;
                
            default:
                jassertfalse;
                break;
                
        }
    }
}

void TransportEditor::paintRelocateFile(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(juce::Colours::darkred);
    g.drawFittedText(TRANS("File Not Found, Please Relocate:"), bounds, juce::Justification::centred, 1);
}

bool TransportEditor::isFileValid(juce::File fileToTest)
{
    auto file = fileToTest;
    
    if(!file.existsAsFile())
    {
        return false;
    }
    auto fileReader = m_formatManager.createReaderFor(fileToTest);
    if (fileReader== nullptr)
    {
        delete fileReader;
        return false;
    }
    delete fileReader;
    return true;
}

void TransportEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if(source == &m_thumbnail)
    {
        repaint();
    }
}

void TransportEditor::timerCallback()
{
    repaint();
}

void TransportEditor::mouseDown(const juce::MouseEvent& event)
{
    auto controlBounds = getLocalBounds().removeFromBottom(30);
    if(controlBounds.contains(event.getPosition())) return;
}

void TransportEditor::mouseDrag(const juce::MouseEvent& event)
{
    auto controlBounds = getLocalBounds().removeFromBottom(30);
    if(controlBounds.contains(event.getPosition())) return;
    
    if(event.mods.isCommandDown() && m_fileIsValid)
    {
        if(m_canDragFile)
        {
            m_canDragFile = false;
            m_tableModel.preventFileImport();
            juce::DragAndDropContainer::performExternalDragDropOfFiles(m_fileToPlay.getFullPathName(), false, this, [&]()
            {
                m_canDragFile = true;
                m_tableModel.allowFIleImport();
            });
        }
    }
}
