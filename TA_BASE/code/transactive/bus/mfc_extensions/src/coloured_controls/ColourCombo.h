/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/coloured_controls/ColourCombo.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Extension of a CComboBox which provides functions to save & restore selection.
  * Also allows text colour of individual combo box items to be defined
  *
  */

#pragma once

#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
    // This guard is so that this MFC extension can be used in another
    // MFC extension DLL
    //
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) ColourCombo : public CComboBox
    #else
    class AFX_EXT_CLASS ColourCombo : public CComboBox
    #endif
    {
    // Construction
    public:
    	ColourCombo();
    
    // Attributes
    public:
    
    // Operations
    public:
    
        /**
         * saveSelection
         *
         * Saves (locally) the details of the current selection
         *
         * @see restoreSelection
         *
         */
        void    saveSelection();
    
        /**
         * restoreSelection
         *
         * Restores the selection attempting to use the previous data
         *  stored from saveSelection
         *
         * @return true if the selection was found and restored, false if defaultIndex
         *              was used (because last saved selection wasn't found) - meaning
         *              the selection has changed
         *
         * @param defaultIndex the index to select if there is no previous saved data
         *          or the previous saved data no longer corresponds to an item in combo
         *
         * @see saveSelection
         *
         */
        bool    restoreSelection(int defaultIndex);
    
        /**
         * mapItemDataToColour
         *
         * Maps an itemdata value with a colour
         *
         * @param itemData the item data to associate with the colour
         *
         * @param textColour the colour to use when drawing text of any items
         *      in this combo box that have an item data value corresponding to 'itemData'
         */
        void mapItemDataToColour(DWORD itemData, DWORD textColour);
    
    // Overrides
    	// ClassWizard generated virtual function overrides
    	//{{AFX_VIRTUAL(ColourCombo)
    	public:
    	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    	//}}AFX_VIRTUAL
    
    // Implementation
    public:
    	virtual ~ColourCombo();
    
    	// Generated message map functions
    protected:
    	//{{AFX_MSG(ColourCombo)
    	//}}AFX_MSG
    
    	DECLARE_MESSAGE_MAP()
    
    private:
    
        /**
         * getColourForItem
         *
         * @return the text colour for an item with the specified itemData value
         *
         * @param itemData the item data of item we're querying colour of
         *
         */
        DWORD getColourForItem(DWORD itemData);
    
        int m_savedIndex;
        DWORD   m_savedData;
        CString m_savedString;
    
        // Map of item data to colour map
        std::map<DWORD, DWORD>  m_colourMap;
    };
    
}   // namespace TA_Base_Bus
