/*
  ==============================================================================

    SoundTableModel.h
    Created: 21 Nov 2022 11:06:32am
    Author:  Antti

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MainComponent.h"
#include "SoundManager.h"
#include "DragAndDropTable.h"

class SoundTableModel : public juce::TableListBoxModel, public juce::ValueTree::Listener
{
    friend class MainComponent;
    friend class SoundManager;
    friend class DragAndDropTable;
    
    ///A juce label that can be put into a cell to keep the FileInfo description information
    struct CellLabel: public juce::Label
    {
        ///The row this label is in. Important to make sure that the correct description is edited and displayed
        int row;
        
        ///Sets the row member
        void setRow(const int rowNum)
        {
            row = rowNum;
        }
        
        ///Default Constructor
        CellLabel()
        {
        }
    };
    
public:
    
    ///The constructor, take in a reference to the main application and the ValueTree this class is a listener to
    SoundTableModel(class MainComponent& mainApp, class SoundManager& valueTree) : m_mainApp(mainApp),
                                                                                   m_valueTreeToListen(valueTree)
    {
        
    }
    ///Pure vitual function inherited from Juce TableListBoxModel. This functon is used to paint the background into a neutral colour
    void paintRowBackground(juce::Graphics& p, int rowNumber, int width, int height, bool rowIsSelected) override;
    ///Pure virtual function inherited from Juce TableListBoxModel. This function is used  to paint the cells with the information got from the ValueTree it is listening
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    ///Pure virtual function inherited from Juce TableListBoxModel. This function returns the number if rows should currently be on the table based on the information got from the ValueTree it is listening
    int  getNumRows() override;

    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& mouseEvent) override;
    
    void selectedRowsChanged(int lastRowSelected) override;
    
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component*                                             existingComponentToUpdate) override;
    
    void cellPopupAction(int selection, int rowNumber, int columnId, const juce::MouseEvent& mouseEvent);
    
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    
    void valueTreePropertyChanged(juce::ValueTree& parentTree, const juce::Identifier& property) override;
    
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
    
private:
    ///Reference to the Main Application
    class MainComponent& m_mainApp;
    ///Reference to the ValueTree this class is listening to
    class SoundManager& m_valueTreeToListen;
    ///This member is the last row that was selected
    int m_lastSelectedRow = 0;

    
    
    
    
    
    
    
    
    
    
    
    
};
