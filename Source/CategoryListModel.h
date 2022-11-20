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
    
    ///Pure virtual function incherited from Juce ListBoxModel used to paint the listbox
    void paintListBoxItem (int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override;
    ///Pure virtual function inherited from Juce ListBoxModel returning the number of rows on the listbox
    int getNumRows () override;
    ///Virtual function inherited from juce ListBoxModel, this function is called when a row is clicked
    void listBoxItemClicked(int row, const juce::MouseEvent& mouseEvent) override;
    ///Virtual fucntion inherited from juce ListBoxMode, this functoin is called whenever a row is selected or deselected
    void selectedRowsChanged(int lastRowSelected) override ;
    ///Adds a category into the listbox if it does not already exist in it
    void addCategoryToList(juce::String name);
    ///Returns true if a category already exists in the list
    bool categoryExists(juce::String category);
    ///Returns the number of categories in the list
    int numCategories();
    ///Function used to delete a category(row) from the list
    void listPopupAction();
    
    
private:
    
    ///A referece to the class that holds the juce Listbox this model handles
    class MainComponent& m_mainApp;
    ///Contains all of the category strings
    std::vector<juce::String> m_uniqueCategories;
    ///Number of unique categories in the listbox
    int m_numCategories = 0;
    ///The row that is currectly selected
    int m_selectedRow;
    
};
