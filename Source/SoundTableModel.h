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

///This class is the model for DragAndDropTable. it listens to a juce ValueTree holding the database for the table and updates the model when the tree changes
class SoundTableModel : public juce::TableListBoxModel, public juce::ValueTree::Listener
{
    friend class MainComponent;
    friend class SoundManager;
    friend class DragAndDropTable;
    friend class CategoryListModel;
    
    ///A juce Label that can be put into a cell to keep the FileInfo description information
    struct CellLabel: public juce::Label
    {
        ///The row this label is in. Important to make sure that the correct description is edited and displayed
        int row;
        
        ///Sets the row member
        void setRow(const int rowNum)
        {
            row = rowNum;
        }
        
        ///Default constructor
        CellLabel()
        {
        }
    };
    
public:
    
    ///The constructor, takes in a reference to the main application and the juce ValueTree this class is a listener to
    SoundTableModel(class MainComponent& mainApp, SoundManager& valueTree) : m_mainApp(mainApp),
                                                                                   m_valueTreeToListen(valueTree)
    {
        
    }
    ///Pure vitual function inherited from Juce TableListBoxModel. This functon is used to paint the background and it is overridden to paint the background black.
    void paintRowBackground(juce::Graphics& p, int rowNumber, int width, int height, bool rowIsSelected) override;
    ///Pure virtual function inherited from Juce TableListBoxModel. This function is used  to paint the cells and it is overriden to paint the cells  with the information got from visible childen on the ValueTree it is listening.
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    ///Pure virtual function inherited from Juce TableListBoxModel. This function returns the number if rows on the table and it is overridden to return the number of visible children currently on the m_valueTreeToListen.
    int  getNumRows() override;
    ///Virtual function inherited from Juce TableListBoxModel. This function is called when one of the cells is clicked and it is overridden to open popupmenus that are used to delete items, add and delete categories from items.
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& mouseEvent) override;
    ///Virtual function inherited from Juce TableListBoxModel. This function is called when rows are deselected or selected and it is overridden to update the m_lastRowSelected member to make sure correct row is targeted.
    void selectedRowsChanged(int lastRowSelected) override;
    ///Virtual function inherited from Juce TableListBoxModel. This function is used to create or update a component to go in a cell. It is overridden to create and update the description label on column five.
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component*                                             existingComponentToUpdate) override;
    ///This function is used to handle popupmenu action for category adding and item deletion.
    void cellPopupAction(int selection, int rowNumber, int columnId, const juce::MouseEvent& mouseEvent);
    ///Virtual function inherited from Juce ValueTree Listener. This function is used to update the Listener when a child is added to m_valueTreeToListen. It is overridden to update the table when a child is added.
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    ///Virtual function inherited from Juce ValueTree Listener. This function is used to update the Listener when a property is changed in m_valueTreeToListen. It is overridden to update the table when visibility or categories of the tree changes,
    void valueTreePropertyChanged(juce::ValueTree& parentTree, const juce::Identifier& property) override;
    ///Virtual function inherited from Juce ValueTree Listener. This function is used to update the Listener when a child is removed from m_valueTreeToListen. It is overridden to update the table when visibility or categories of the tree changes,
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
    ///This function is called when the user clicks the locate button in the TransportEditor and it is calling the setNewFilePath function of the m_valueTreeToListen
    void locateFile(juce::File file);
    ///This functon is called by m_valueTreeToListen if user successfully locates a file. It calls the TransportEditors setFileToPlay function so that user does not have to click the row again to get the waveform to show up.
    void reloadWaveform();
    ///This function is called when a file is being exported via dragging from the TransportEditor. It sets the DragAndDropTables m_acceptingFiles boolean to false and therefore prevents dragged files from being imported back into the application.
    void preventFileImport();
    ///This functoin is called when a drag export action from TransportEditor has finished. it sets the DragAndDropTables m_acceptingFiles boolean to true and making it possible to import files into the table via drag and drop.
    void allowFIleImport();
    
private:
    ///Reference to the MainComponent
    class MainComponent& m_mainApp;
    ///Reference to the ValueTree this class is listening to
    class SoundManager& m_valueTreeToListen;
    ///This member is the last row that was selected
    int m_lastSelectedRow = 0;

    
    
    
    
    
    
    
    
    
    
    
    
};
