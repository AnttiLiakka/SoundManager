/*
  ==============================================================================

    TransportEditor.cpp
    Created: 25 Nov 2022 2:15:23pm
    Author:  Antti

  ==============================================================================
*/

#include "MainComponent.h"

TransportEditor::TransportEditor(class SoundTableModel& tableModel, class MainComponent& mainApp, class TransportPlayer& player) :
                 m_tableModel(tableModel),
                 m_mainApp(mainApp),
                 m_player(player),
                 m_thumbnailCache(5),
                 m_thumbnail(512,
                 m_formatManager, m_thumbnailCache),
                 m_playButton(TRANS("Play"), juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_stopButton(TRANS("Stop"), juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_loopButton(TRANS("Loop"), juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_relocateButton(TRANS("Locate File"), juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_playheadPosition("PlayheadPosition"),
                 m_volumeSlider(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::NoTextBox),
                 playState(Stopped)
{
    
    m_formatManager.registerBasicFormats();
    
    m_audioDeviceManager.initialise(0, 2, nullptr, true);

    m_audioSettings = std::make_unique<juce::AudioDeviceSelectorComponent>(m_audioDeviceManager, 0, 0, 0, 2, false, false, false, true);
    
    m_thumbnail.addChangeListener(this);
    

    m_playButton.setImages(juce::Drawable::createFromImageData(BinaryData::playInactive_svg, BinaryData::playInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playHover_svg, BinaryData::playHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playActive_svg, BinaryData::playActive_svgSize).get());
    
    m_stopButton.setImages(juce::Drawable::createFromImageData(BinaryData::stopInactive_svg, BinaryData::stopInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::stopHover_svg, BinaryData::stopHover_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::stopActive_svg, BinaryData::stopActive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::stopActive_svg, BinaryData::stopActive_svgSize).get());
    
    m_loopButton.setImages(juce::Drawable::createFromImageData(BinaryData::LoopInactive_svg, BinaryData::LoopInactive_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::LoopHover_svg, BinaryData::LoopHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::LoopActive_svg, BinaryData::LoopActive_svgSize).get());
    
    m_relocateButton.setImages(juce::Drawable::createFromImageData(BinaryData::LocateInactive_svg, BinaryData::LocateInactive_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::LocateHover_svg, BinaryData::LocateHover_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::LocateActive_svg, BinaryData::LocateActive_svgSize).get());
    
    m_playheadPosition.setEditable(false);
    m_playheadPosition.setFont(juce::Font(20));

    m_volumeSlider.setRange(0.0f, 1.0f);
    m_volumeSlider.setTooltip(TRANS("Volume"));
    m_volumeSlider.setValue(0.5);
    m_volumeSlider.addListener(&m_player);
    
    m_volumeSlider.setColour(juce::Slider::trackColourId, juce::Colours::darkred);
    m_volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    
    m_playButton.setTooltip(TRANS("Play"));
    m_stopButton.setTooltip(TRANS("Stop"));
    m_loopButton.setTooltip(TRANS("Loop"));
    
    m_stopButton.setEnabled(false);
    m_relocateButton.setEnabled(false);
    m_relocateButton.setAlpha(0);
    
    m_playButton.onClick = [&](){
        playButtonClicked();
    };

    m_stopButton.onClick = [&](){
        stopButtonClicked();
    };
    
    m_loopButton.onClick = [&](){
        loopButtonClicked();
    };
    
    m_relocateButton.onClick = [&]()
    {
        (m_fileToPlay.getFullPathName());
         m_tableModel.locateFile(m_fileToPlay);
    };
    
    m_playButton.addShortcut(juce::KeyPress(juce::KeyPress::spaceKey));
    
    addAndMakeVisible(m_playButton);
    addAndMakeVisible(m_stopButton);
    addAndMakeVisible(m_loopButton);
    addAndMakeVisible(m_relocateButton);
    addAndMakeVisible(m_playheadPosition);
    addAndMakeVisible(m_volumeSlider);
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
    auto sliderMargin = sliders.removeFromRight(10);
    auto relocateButtonBottonMargin = bounds.removeFromBottom(getLocalBounds().getHeight() / 3.5);
    auto relocateButtonTopMargin = bounds.removeFromTop(getLocalBounds().getHeight() / 3.5);
    m_relocateButton.setBounds(bounds.removeFromRight(bounds.getWidth() / 2.5));
    
    m_playButton.setBounds(controls.removeFromLeft(35));
    m_stopButton.setBounds(controls.removeFromLeft(35));
    m_loopButton.setBounds(controls.removeFromLeft(35));
    m_playheadPosition.setBounds(controls.removeFromLeft(bounds.getWidth() / 3));
    m_volumeSlider.setBounds(sliders.removeFromRight(bounds.getWidth() / 3));
}

void TransportEditor::setFileToPlay(juce::File file)
{
    m_player.prepForNewFile();
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
    } else
    {
        m_relocateButton.setEnabled(false);
        m_relocateButton.setAlpha(0);
        loadAudioFile(file);
        m_fileIsValid = true;
    }
}

void TransportEditor::paintLoadedFile(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(juce::Colours::white);
    auto audioLenght = (float) m_numBufferSamples;
    auto audioPosition = (float) m_player.m_playPosition;
    m_thumbnail.drawChannels(g, bounds, 0, m_thumbnail.getTotalLength(), 1.0f);
    g.setColour(juce::Colours::darkred);
    auto drawPosition = (audioPosition / audioLenght) * (float) bounds.getWidth() + (float) bounds.getX();
    g.drawLine(drawPosition, (float) bounds.getY(), drawPosition, (float) bounds.getBottom(), 2.0f);
}

void TransportEditor::changePlayState(PlayState newPlayState)
{
    if (playState != newPlayState)
    {
        playState = newPlayState;
        
        switch (playState)
        {
            case Starting:
                if(!isTimerRunning()) startTimer(10);
                m_player.startPlayback();
                changePlayState(Playing);
                break;
                
            case Playing:
                m_stopButton.setEnabled(true);
                changePlayToPause();
                break;
                
            case Pausing:
                m_player.pausePlayback();
                changePlayState(Paused);
                break;
                
            case Paused:
                changePauseToPlay();
                break;
                
            case Stopping:
                m_player.stopPlayback();
                changePlayState(Stopped);
                break;
                
            case Stopped:
                m_playheadPosition.setText("0", juce::NotificationType::dontSendNotification);
                m_stopButton.setEnabled(false);
                changePauseToPlay();
                stopTimer();
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
    if(source == &m_player)
    {
        if (!m_player.m_isLooping)
        {
            m_player.m_playing = false;
            m_player.m_playPosSeconds = 0;
            changePlayState(Stopped);
        }
    }
    
}

void TransportEditor::timerCallback()
{
    repaint();
    m_playheadPosition.setText(juce::String(m_player.m_playPosSeconds), juce::NotificationType::dontSendNotification);
}

void TransportEditor::mouseDown(const juce::MouseEvent& event)
{
    
    auto controlBounds = getLocalBounds().removeFromBottom(30);
    if(controlBounds.contains(event.getPosition()) || !m_fileIsValid) return;
    if(!event.mods.isCommandDown())
    {
        auto ratio = m_numBufferSamples / getLocalBounds().getWidth();
        m_player.setPlayPosition(event.getMouseDownX() * ratio);
        repaint();
    }
}

void TransportEditor::mouseDrag(const juce::MouseEvent& event)
{
    auto controlBounds = getLocalBounds().removeFromBottom(30);
    if(controlBounds.contains(event.getPosition()) || !m_fileIsValid ) return;
    
    if(event.mods.isCommandDown())
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
    } else
    {
        changePlayState(Pausing);
        auto ratio = m_numBufferSamples / getLocalBounds().getWidth();
        m_player.setPlayPosition(event.getMouseDownX() * ratio + event.getDistanceFromDragStartX() * ratio);
        repaint();
        DBG(m_player.m_playPosition);
    }
}

void TransportEditor::openAudioSettings()
{
    
    m_audioSettings = std::make_unique<juce::AudioDeviceSelectorComponent>(m_audioDeviceManager, 0, 0, 0, 2, false, false, false, false);
    m_audioSettings->setSize(400, 600);
    juce::DialogWindow::showDialog("Audio settings",m_audioSettings.get() , &m_mainApp, juce::Colours::black, true);

}

void TransportEditor::loadAudioFile(juce::File file)
{
    auto fileReader = m_formatManager.createReaderFor(file);
    
    m_player.m_buffer.setSize(2, static_cast<int>(fileReader->lengthInSamples));
    fileReader->read(&m_player.m_buffer, 0, static_cast<int>(fileReader->lengthInSamples), 0, true, true);
    m_thumbnail.setSource(new juce::FileInputSource(file));
    m_numBufferSamples = m_player.m_buffer.getNumSamples();
    delete fileReader;
}

void TransportEditor::playButtonClicked()
{
    if(!m_fileSelected || !m_fileIsValid) return;
    if(playState == Playing)
    {
        changePlayState(Pausing);
    } else
    {
        changePlayState(Starting);
    }
}

void TransportEditor::stopButtonClicked()
{
    changePlayState(Stopping);
}

void TransportEditor::loopButtonClicked()
{
    m_player.m_isLooping = !m_player.m_isLooping;
    
    if(m_player.m_isLooping)
    {
        m_loopButton.setImages(juce::Drawable::createFromImageData(BinaryData::LoopActive_svg, BinaryData::LoopActive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::LoopHover_svg, BinaryData::LoopHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::LoopInactive_svg, BinaryData::LoopInactive_svgSize).get());
    } else
    {
        m_loopButton.setImages(juce::Drawable::createFromImageData(BinaryData::LoopInactive_svg, BinaryData::LoopInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::LoopHover_svg, BinaryData::LoopHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::LoopActive_svg, BinaryData::LoopActive_svgSize).get());
    }
}

void TransportEditor::changePlayToPause()
{
    m_playButton.setImages(juce::Drawable::createFromImageData(BinaryData::pauseInactive_svg, BinaryData::pauseInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::pauseHover_svg, BinaryData::pauseHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::pauseActive_svg, BinaryData::pauseActive_svgSize).get());
}

void TransportEditor::changePauseToPlay()
{
    m_playButton.setImages(juce::Drawable::createFromImageData(BinaryData::playInactive_svg, BinaryData::playInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playHover_svg, BinaryData::playHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playActive_svg, BinaryData::playActive_svgSize).get());
}
