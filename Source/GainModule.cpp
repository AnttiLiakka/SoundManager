/*
  ==============================================================================

    Gain.cpp
    Created: 18 Jun 2023 7:35:12pm
    Author:  antti

  ==============================================================================
*/

#include "GainModule.h"


GainModule::GainModule()
{

}

GainModule::~GainModule()
{

}



float GainModule::processSample(const float sampleToProcess) const
{
    return sampleToProcess * m_gain.load();
}


void GainModule::SetGain(const double newGain)
{
    m_gain.store(newGain);
}

double GainModule::GetGain() const
{
    return m_gain.load();
}



