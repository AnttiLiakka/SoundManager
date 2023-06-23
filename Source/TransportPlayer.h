/*
  ==============================================================================

    TransportProcessor.h
    Created: 28 Nov 2022 9:39:33am
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TransportEditor.h"
#include "GainModule.h"

///This class is responsible of playing the sound files on the table. It is controlled by TransportEditor
class TransportPlayer : public juce::AudioSource, public juce::ChangeBroadcaster, public juce::Slider::Listener
{
    friend class TransportEditor;
public:
    ///The constructor, takes in a reference to the TransportEditor controlling it.
    TransportPlayer(class TransportEditor& editor);
    ///The destructor.
    ~TransportPlayer();
    ///Pure virtual function inherited from juce AudioSource. This function tells the source to prepare for playing. It is used to set the m_sampleRate member so that the m_playPosSeconds has the correct value.
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    ///Pure virtual function inherited from juce AudioSource. This function fetches blocks of audio data and it is used to apply effects such as volume alteration on the m_buffer.
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    ///Pure virtual function inherited from juce AudioSource. This function allows the source to release anything it no longer needs after playback has stopped.
    void releaseResources() override;
    ///Pure virtual function inherited from juce Slider Listener. This function is called when the user interacts with the volume slider and it updates m_volume accordingly.
    void sliderValueChanged(juce::Slider *slider) override;
    ///This function is called when a new sound file is selected on the table.It sets m_playing to false, clears the m_buffer and sets the m_playPosition to 0.
    void prepForNewFile();
    ///This function is called when the TransportEditors playbutton is pressed. it sets m_playing to true.
    void startPlayback();
    ///This function is called when the TransportEditors pausebutton is pressed. it sets m_playing to false.
    void pausePlayback();
    ///This function is called when the TransportEditors stopbutton is pressed. it sets m_playing to false, m_playPosition to 0 and m_playPosSeconds to 0.
    void stopPlayback();
    ///This function is used to set new value to m_playPosition.
    void setPlayPosition(int newPosition);
    ///This function is used to set new value to m_endPosition.
    void setEndPosition(int newEndPosition);
private:
    ///Reference to the TransportEditor controlling this class.
    class TransportEditor& m_editor;
    ///The audiobuffer containing the information of the selected audio file.
    juce::AudioBuffer<float> m_buffer;
    /// This member is used to stream audio from this class to the soundcard.
    juce::AudioSourcePlayer m_player;

    GainModule m_gainModule;
    ///Current play position. Defaulted to 0.
    std::atomic<int> m_playPosition { 0 };
    ///Sample rate
    double m_sampleRate = 48000;
    ///Current play position in seconds, This member is displayed in the TransportEditors playbackPosition label.
    std::atomic<float> m_playPosSeconds { 0 };
    ///Whether audio should be playing or not. Defaulted to false.
    std::atomic<bool> m_playing { false };
    ///Whether audio should be looping or not. Defaulted to false.
    std::atomic<bool> m_isLooping { false };
    ///This int represets what sample the playback should stop. Defaulted to 0.
    std::atomic<int> m_endPosition { 0 };
    
};






