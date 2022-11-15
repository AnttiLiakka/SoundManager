/*
  ==============================================================================

    CategoryList.h
    Created: 15 Nov 2022 2:42:15pm
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MainComponent.h"
#include "DragAndDropTable.h"


class CategoryListModel: public juce::ListBoxModel
{
    friend class MainComponent;
    friend class DragAndDropTable;
public:
    CategoryListModel(class MainComponent& mainApp): ListBoxModel(), m_mainApp(mainApp)
    {
        
    }
    
    void paintListBoxItem (int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override;
    int getNumRows () override;
    
    void addCategoryToList(juce::String name);
    
    bool categoryExists(juce::String category);
    
    int numCategories();
    
private:
    
    class MainComponent& m_mainApp;
    
    std::vector<juce::String> m_uniqueCategories;
};
