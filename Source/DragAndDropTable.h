/*
  ==============================================================================

    DragAndDropTable.h
    Created: 3 Nov 2022 7:16:50pm
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DragAndDropTable : public juce::TableListBox, public juce::DragAndDropContainer
{
public:
    DragAndDropTable() : TableListBox("table", nullptr), DragAndDropContainer()
    {

    }
    ~DragAndDropTable()
    {

    }

private:


};
