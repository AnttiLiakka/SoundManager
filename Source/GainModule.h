/*
  ==============================================================================

    Gain.h
    Created: 18 Jun 2023 7:35:12pm
    Author:  antti

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class GainModule
{
public:

    GainModule();
    ~GainModule();

    float processSample(const float sampleToProcess) const;

    void SetGain(const double newGain) ;

    double GetGain() const;

private:

    std::atomic<double> m_gain{ 0 };
};