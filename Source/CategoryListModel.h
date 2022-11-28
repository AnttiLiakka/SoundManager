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

class SoundManager;
/// This class is the model for the MainComponents m_categoryList member. It updates the ListBox when new categories are added to it or removed from it.
class CategoryListModel: public juce::ListBoxModel, public juce::Label::Listener
{
    
    friend class MainComponent;
    friend class DragAndDropTable;
    friend class SoundTableModel;
    
public:
    
    ///The constructor, takes a reference to the MainComponent and SoundManager.
    CategoryListModel(class MainComponent& mainApp, SoundManager& valueTree ): ListBoxModel(), m_mainApp(mainApp),
                                                                                     m_valueTreeToListen(valueTree)
    {
        
    }
    
    ///Pure virtual function incherited from Juce ListBoxModel used to paint the listbox. It is overridden to paint the juce ListBox with the stock categories and categories on the ValueTree database.
    void paintListBoxItem (int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override;
    ///Pure virtual function inherited from Juce ListBoxModel returning the number of rows on the listbox. It is overridden to return the number of categories on the ListBox.
    int getNumRows () override;
    ///Virtual function inherited from juce ListBoxModel. This function is called when a row is clicked and it is overridden to filter and unfilter the table when categories are selected and unselected.
    void listBoxItemClicked(int row, const juce::MouseEvent& mouseEvent) override;
    ///Virtual function inherited from juce ListBoxMode. This function is called whenever a row is selected or deselected and it is overridden to update m_selectedRow to make sure that correct category is used to filter the table
    void selectedRowsChanged(int lastRowSelected) override ;
    ///This function adds a category into the listbox if it does not already exist in it
    void addCategoryToList(juce::String name);
    ///Returns true if a category already exists in the list
    bool categoryExists(juce::String category);
    ///Returns the number of categories in the list
    int numCategories();
    ///Function used to delete a category(row) from the list
    void listPopupAction();
    ///Pure virtual function inherited from Juce Label Listener. This function responds to changes in Labels this class is listening to and it is overridden to add categories to the list
    void labelTextChanged(juce::Label* labelThatHasChanged) override;
    
    
    
private:
    
    ///Reference to the MainComponent
    class MainComponent& m_mainApp;
    ///Reference to the SoundManager
    class SoundManager& m_valueTreeToListen;
    ///This member contains all of the category strings
    std::vector<juce::String> m_uniqueCategories;
    ///The row that is currectly selected
    int m_selectedRow;
    
};
