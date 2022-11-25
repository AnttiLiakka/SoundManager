/*
  ==============================================================================

    TransportEditor.cpp
    Created: 25 Nov 2022 2:15:23pm
    Author:  Antti

  ==============================================================================
*/

#include "TransportEditor.h"

TransportEditor::TransportEditor(class SoundTableModel& tableModel) :m_tableModel(tableModel),
                                                                     m_thumbnailCache(5),
                                                                     m_thumbnail(512,
                                                                     m_formatManager, m_thumbnailCache),
                                                                     m_playButton("Play", juce::DrawableButton::ButtonStyle::ImageFitted),
                                                                     m_pauseButton("Pause", juce::DrawableButton::ButtonStyle::ImageFitted),
                                                                     m_stopButton("Stop", juce::DrawableButton::ButtonStyle::ImageFitted)
{
    
    m_formatManager.registerBasicFormats();
    
    
    m_playButton.setImages(juce::Drawable::createFromImageData(BinaryData::playInactive_svg, BinaryData::playInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playHover_svg, BinaryData::playHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::playActive_svg, BinaryData::playActive_svgSize).get());
    
    m_pauseButton.setImages(juce::Drawable::createFromImageData(BinaryData::pauseInactive_svg, BinaryData::pauseInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::pauseHover_svg, BinaryData::pauseHover_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::pauseActive_svg, BinaryData::pauseActive_svgSize).get());
    
    m_stopButton.setImages(juce::Drawable::createFromImageData(BinaryData::stopInactive_svg, BinaryData::stopInactive_svgSize).get(), juce::Drawable::createFromImageData(BinaryData::stopHover_svg, BinaryData::stopHover_svgSize).get(),juce::Drawable::createFromImageData(BinaryData::stopActive_svg, BinaryData::stopActive_svgSize).get());
    
    
    m_playButton.setTooltip("Play");
    m_pauseButton.setTooltip("Pause");
    m_stopButton.setTooltip("Stop");
    
    
    
    addAndMakeVisible(m_playButton);
    addAndMakeVisible(m_pauseButton);
    addAndMakeVisible(m_stopButton);
}


void TransportEditor::paint(juce::Graphics &g)
{
    
    auto waveformBounds = getLocalBounds();
    auto controls = waveformBounds.removeFromBottom(30);
    
    if(m_shouldBePainting)
    {
        paintLoadedFile(g, waveformBounds);
    } else
    {
        g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
        g.fillRect(waveformBounds);
        g.setColour(juce::Colours::darkred);
        g.drawFittedText(TRANS("File Not Selected"), waveformBounds, juce::Justification::centred, 1);
    }
    
}

void TransportEditor::resized()
{
    auto controls = getLocalBounds().removeFromBottom(30);
    auto buttonMargin = controls.removeFromBottom(4);
    auto sliders = controls.removeFromLeft(getWidth() / 2 - 52.5);
    
    m_playButton.setBounds(controls.removeFromLeft(35));
    m_pauseButton.setBounds(controls.removeFromLeft(35));
    m_stopButton.setBounds(controls.removeFromLeft(35));
}

void TransportEditor::setFileToPlay(juce::File file)
{
    m_shouldBePainting = false;
    if(file.exists())
    {
        m_fileToPlay = file;
    } else
    {
        DBG("FILE NOT VALID");
        return;
    }
    
    m_thumbnail.setSource(new juce::FileInputSource(file));
    
    m_shouldBePainting = true;
    repaint();
}

void TransportEditor::paintLoadedFile(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
    g.fillRect(bounds);
    g.setColour(juce::Colours::darkred);
    m_thumbnail.drawChannel(g, bounds, 0, m_thumbnail.getTotalLength(), m_thumbnail.getNumChannels(), 1.0f);
    m_thumbnail.drawChannels(g, bounds, 0, m_thumbnail.getTotalLength(), 1.0f);
    DBG(juce::String(m_thumbnail.getTotalLength()));
}
