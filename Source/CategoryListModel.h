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

/// This class manages the list of categories
class CategoryListModel: public juce::ListBoxModel
{
    
    friend class MainComponent;
    friend class DragAndDropTable;
    
public:
    
    ///Default Constructor
    CategoryListModel(class MainComponent& mainApp): ListBoxModel(), m_mainApp(mainApp)
    {
        
    }
    
    ///Abstract function incherited from Juce ListBoxModel
    void paintListBoxItem (int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override;
    ///Abstract function inherited from Juce ListBoxMode
    int getNumRows () override;
    ///Adds a category into the listbox if it does not already exist in it
    void addCategoryToList(juce::String name);
    ///Returns true if a category already exists in the list
    bool categoryExists(juce::String category);
    ///Returns the number of categories in the list
    int numCategories();
    
private:
    
    ///A referece to the class that holds the juce Listbox this model handles
    class MainComponent& m_mainApp;
    ///Contains all of the category strings
    std::vector<juce::String> m_uniqueCategories;
    
};
