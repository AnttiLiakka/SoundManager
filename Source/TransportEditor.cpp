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
                 m_thumbnailCache(5), m_thumbnail(512, m_formatManager, m_thumbnailCache),
                 m_playButton("Play", juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_stopButton("Stop", juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_loopButton("Loop", juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_relocateButton("Locate File", juce::DrawableButton::ButtonStyle::ImageFitted),
                 m_renderButton("Render", juce::DrawableButton::ButtonStyle::ImageFitted),
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
    
    m_renderButton.setImages(juce::Drawable::createFromImageData(BinaryData::RenderInactive_svg, BinaryData::RenderInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::RenderHover_svg, BinaryData::RenderHover_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::RenderActive_svg, BinaryData::RenderActive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::RenderActive_svg, BinaryData::RenderActive_svgSize).get());

    
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
    m_renderButton.setTooltip(TRANS("Render Selected Section"));
    
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
    
    m_renderButton.onClick = [&]()
    {
        if(m_sectionSelected) prepSelectionBuffer();
    };
    
    addAndMakeVisible(m_playButton);
    addAndMakeVisible(m_stopButton);
    addAndMakeVisible(m_loopButton);
    addAndMakeVisible(m_relocateButton);
    addAndMakeVisible(m_renderButton);
    addAndMakeVisible(m_playheadPosition);
    addAndMakeVisible(m_volumeSlider);
}

TransportEditor::~TransportEditor()
{
    if(m_tempFile.existsAsFile())
    {
        m_tempFile.deleteFile();
    }
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
    
    if(m_sectionSelected)
    {
        paintSelection(g, m_mouseDragStartPos, m_mouseDragDistance);
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
    m_renderButton.setBounds(controls.removeFromLeft(35));
    m_playheadPosition.setBounds(controls.removeFromLeft(bounds.getWidth() / 3));
    m_volumeSlider.setBounds(sliders.removeFromRight(bounds.getWidth() / 3));
}

void TransportEditor::setFileToPlay(juce::File file)
{
    //Stops audio playback and clears the buffer
    m_player.prepForNewFile();
    m_fileSelected = true;
    m_fileToPlay = file;
    //Removes the selected section drawing
    resetSelection();
    //Changes playState to correct state if needed
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

void TransportEditor::paintSelection(juce::Graphics& g, int startPos, int endPos)
{
    g.setColour(juce::Colours::darkred.withAlpha(0.5f));
    auto height = getLocalBounds().getHeight();
    juce::Rectangle<int> selectionBounds(startPos, getLocalBounds().getY(), endPos, height - 30);
    g.fillRect(selectionBounds);
}

void TransportEditor::resetSelection()
{
    m_selectionBuffer.clear();
    m_sectionSelected = false;
    m_mouseDragStartPos = 0;
    m_mouseDragDistance = 0;
    m_player.setEndPosition(m_numBufferSamples);
    repaint();
    m_sectionPlayActive = false;
    m_renderButton.setEnabled(true);
    m_renderButton.setTooltip(TRANS("Render Selected Section"));
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
                if(m_sectionSelected) setSelectionPlay();
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
                resetSelection();
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
    if (fileReader == nullptr)
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
        if(!m_sectionSelected)
        {
            m_player.setPlayPosition(0);
        } else
        {
            m_sectionPlayActive = false;
            setSelectionPlay();
        }
    }
    
}

void TransportEditor::timerCallback()
{
    repaint();
    //m_playheadPosition.setText(juce::String(m_player.m_playPosSeconds), juce::NotificationType::dontSendNotification);
    m_playheadPosition.setText(juce::String(m_player.m_playPosition), juce::NotificationType::dontSendNotification);
}

void TransportEditor::mouseDown(const juce::MouseEvent& event)
{
    auto controlBounds = getLocalBounds().removeFromBottom(30);
    if(controlBounds.contains(event.getPosition()) || !m_fileIsValid) return;
    if(!event.mods.isCommandDown())
    {
        resetSelection();
        auto ratio = m_numBufferSamples / getLocalBounds().getWidth();
        auto newPlayPos = event.getMouseDownX() * ratio;
        if(newPlayPos < 0) newPlayPos = 0;
        if(newPlayPos > m_numBufferSamples) newPlayPos = m_numBufferSamples;
        m_player.setPlayPosition(newPlayPos);
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
            if(m_sectionSelected && m_tempFileRendered)
            {
                m_canDragFile = false;
                m_tableModel.preventFileImport();
                juce::DragAndDropContainer::performExternalDragDropOfFiles(m_tempFile.getFullPathName(), false, this, [&]()
                {
                    m_canDragFile = true;
                    m_tableModel.allowFIleImport();
                });
            }
            
            else
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
    else if(event.mods.isAltDown())
    {
        m_sectionSelected = true;
        m_mouseDragStartPos = event.getMouseDownX();
        m_mouseDragDistance = event.getDistanceFromDragStartX();
        m_mouseDragEndPos = m_mouseDragStartPos + m_mouseDragDistance;
        repaint();
    }
    else
    {
        changePlayState(Pausing);
        auto ratio = m_numBufferSamples / getLocalBounds().getWidth();
        auto newPlayPos = event.getMouseDownX() * ratio + event.getDistanceFromDragStartX() * ratio;
        if (newPlayPos < 0) newPlayPos = 0;
        if (newPlayPos > m_numBufferSamples) newPlayPos = 0;
        m_player.setPlayPosition(newPlayPos);
        repaint();
    }
}

void TransportEditor::mouseUp(const juce::MouseEvent &event)
{
    if(m_sectionSelected) setSelectionPlay();
}

void TransportEditor::openAudioSettings()
{
    
    m_audioSettings = std::make_unique<juce::AudioDeviceSelectorComponent>(m_audioDeviceManager, 0, 0, 0, 2, false, false, false, false);
    m_audioSettings->setSize(400, 600);
    juce::DialogWindow::showDialog(TRANS("Audio settings"),m_audioSettings.get() , &m_mainApp, juce::Colours::black, true);

}

void TransportEditor::loadAudioFile(juce::File file)
{
    std::unique_ptr<juce::AudioFormatReader> fileReader (m_formatManager.createReaderFor(file));
    m_player.m_buffer.setSize(2, static_cast<int>(fileReader->lengthInSamples));
    
    auto duration = fileReader->lengthInSamples / fileReader->sampleRate;
    
    if (duration > 600)
    {
        m_renderWindow = std::make_unique<BufferingAlertWindow>(m_player.m_buffer);
        m_renderWindow->setOwner(this);
        m_renderWindow->launchThread();
        m_thumbnail.setSource(new juce::FileInputSource(file));
        m_numBufferSamples = m_player.m_buffer.getNumSamples();
        m_player.setEndPosition(m_numBufferSamples);
        
    } else
    {
        fileReader->read(&m_player.m_buffer, 0, static_cast<int>(fileReader->lengthInSamples), 0, true, true);
        m_thumbnail.setSource(new juce::FileInputSource(file));
        m_numBufferSamples = m_player.m_buffer.getNumSamples();
        m_player.setEndPosition(m_numBufferSamples);
    }
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

void TransportEditor::setSelectionPlay()
{
    if(m_sectionPlayActive) return;
    auto ratio = m_numBufferSamples / getLocalBounds().getWidth();
    auto startPos = m_mouseDragStartPos * ratio;
    auto endPos = m_mouseDragEndPos * ratio;
    
    if(endPos < 0)
    {
        endPos = 0;
    } else if(endPos > m_numBufferSamples)
    {
        endPos = m_numBufferSamples;
    }
    
    if(startPos < 0)
    {
        startPos = 0;
    } else if(startPos > m_numBufferSamples)
    {
        startPos = m_numBufferSamples;
    }
    
    if (endPos < startPos)
    {
        m_player.setPlayPosition(endPos);
        m_player.setEndPosition(startPos);
        
        m_selectionEnd = startPos;
        m_selectionStart = endPos;
        
    } else
    {
        m_player.setPlayPosition(startPos);
        m_player.setEndPosition(endPos);
        
        m_selectionEnd = endPos;
        m_selectionStart = startPos;
    }
    m_sectionPlayActive = true;
}

void TransportEditor::noFileSelected()
{
    m_fileSelected = false;
    m_relocateButton.setAlpha(0);
    m_thumbnail.clear();
}

void TransportEditor::prepSelectionBuffer()
{
    auto buffer = m_player.m_buffer;
    m_selectionBuffer.setSize(buffer.getNumChannels(), m_selectionEnd - m_selectionStart);
    m_selectionBuffer.copyFrom(0, 0, buffer, 0, m_selectionStart, m_selectionBuffer.getNumSamples());
    m_selectionBuffer.copyFrom(1, 0, buffer, 1, m_selectionStart, m_selectionBuffer.getNumSamples());
    createFileFromSelection();
}

void TransportEditor::createFileFromSelection()
{
    if (m_tempFile.existsAsFile()) m_tempFile.deleteFile();
    m_tempFileRendered = false;
    m_tempFile = m_mainApp.m_tempAudioFiles.getChildFile(m_fileToPlay.getFileName());
    
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    writer.reset(format.createWriterFor(new juce::FileOutputStream(m_tempFile), 44100, m_selectionBuffer.getNumChannels(), 24, {}, 0));
    
    if(writer != nullptr)
    {
        //writer->writeFromAudioSampleBuffer(m_selectionBuffer, 0, m_selectionBuffer.getNumSamples());
        
        int length = m_selectionBuffer.getNumSamples();
        int blockSize = 512;
        int numBlocks = length / blockSize;
        
        for (int i = 0; i < numBlocks; ++i)
        {
            int startPosition = blockSize * i;
            
            if(i != numBlocks - i)
            {
                writer->writeFromAudioSampleBuffer(m_selectionBuffer, startPosition, blockSize);
            } else
            {
                writer->writeFromAudioSampleBuffer(m_selectionBuffer, startPosition, length - blockSize * i);
            }
        }
        m_tempFileRendered = true;
        m_renderButton.setEnabled(false);
        m_renderButton.setTooltip(TRANS("Render complete, exporting now exports selected section"));
    }
}

void TransportEditor::setBuffer(juce::AudioBuffer<float> newBuffer)
{
    m_player.m_buffer = newBuffer;
}

juce::ApplicationCommandTarget* TransportEditor::getNextCommandTarget()
{
    return &m_mainApp;
}

void TransportEditor::getAllCommands(juce::Array<juce::CommandID> &commands)
{
    juce::Array<juce::CommandID> ids {KeyPressCommandIDs::Loop, KeyPressCommandIDs::PlayPause,
                                      KeyPressCommandIDs::Stop
                                        };
    commands.addArray(ids);
}

void TransportEditor::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result)
{
    switch (commandID)
    {
        case KeyPressCommandIDs::Loop:
            result.setInfo("Loop", "Toggle loop button", "Transport", 0);
            result.addDefaultKeypress('l', 0);
            break;
        case KeyPressCommandIDs::PlayPause:
            result.setInfo("Play/Pause", "Toggle PlayPause", "Transport", 0);
            result.addDefaultKeypress(juce::KeyPress::spaceKey, 0);
            break;
        case KeyPressCommandIDs::Stop:
            result.setInfo("Stop", "Press Stop", "Transport", 0);
            result.addDefaultKeypress(juce::KeyPress::backspaceKey, 0);
            break;
        default:
            break;
    }
}

bool TransportEditor::perform(const juce::ApplicationCommandTarget::InvocationInfo &info)
{
    switch (info.commandID)
    {
        case KeyPressCommandIDs::Loop:
            m_loopButton.triggerClick();
            break;
        case KeyPressCommandIDs::PlayPause:
            m_playButton.triggerClick();
            if(!hasKeyboardFocus(false)) grabKeyboardFocus();
            break;
        case KeyPressCommandIDs::Stop:
            if(m_stopButton.isEnabled()) m_stopButton.triggerClick();
            break;
        default:
            return false;
            break;
    }
    return true;
}

