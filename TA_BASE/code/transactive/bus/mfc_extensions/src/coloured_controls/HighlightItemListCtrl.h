/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Helper class for a list controls that displays items that
  *     can have a special highlighted state - must 
  *     be attached to an owner drawn control to work
  *
  */

#pragma once

#include <string>
#include <set>

namespace TA_Base_Bus
{
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) HighlightItemListCtrl : public CListCtrl
    #else
    class AFX_EXT_CLASS HighlightItemListCtrl : public CListCtrl
    #endif
    {
    // Construction
    public:
    	HighlightItemListCtrl();
        virtual ~HighlightItemListCtrl();
    
    
        /**
         * registerHighlightData
         *
         * Tags a particular data value as being highlighted (can be called multiple times to tag
         *  multiple data values).  
         *
         * @param data the data value representing highlighted items
         *          (that is, matching a call to GetItemData())
         *
         */
        void    registerHighlightData(unsigned long data);

        /**
         * deregisterHighlightData
         *
         * Inverse operation of registerHighlightData.  If the specified data value
         *  is not recognized, this operation does nothing.
         */
        void    deregisterHighlightData(unsigned long data);

        /**
         * DeleteItemAndClearHighlightFlag
         *
         * Deletes the item at the specified index, but before doing so
         *  deregisters the highlight data associated with the index (if present)
         * In this way, if ItemData components are unique in the list, as they're
         *  deleted they leave the control in a valid state (without lingering
         *  highlight data settings)
         *
         * @pre nItem has an ItemData value that is unique within the control
         *
         * @see deregisterHighlightData
         */
        void DeleteItemAndClearHighlightFlag(int nItem);
        
        /**
         * clearHighlightDataSet
         *
         * Called to purge all the existing highlight information (for a refresh)
         *
         */
        void clearHighlightDataSet();

        /**
         * setHighlightColour
         *
         * @param fgColour the new colour to use for the text when drawing highlighted items
         * @param bgColour the background text colour for highlighted items
         */
        void    setHighlightColour(DWORD fgColour, DWORD bgColour = ::GetSysColor(COLOR_WINDOW));
        
    // Overrides
    	// ClassWizard generated virtual function overrides
    	//{{AFX_VIRTUAL(HighlightItemListCtrl)
    	//}}AFX_VIRTUAL
    
    	// Generated message map functions
    protected:
    
    	//{{AFX_MSG(HighlightItemListCtrl)
    	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    	//}}AFX_MSG
    
    	DECLARE_MESSAGE_MAP()
    
    private:
    
        /**
         * isHighlightedRow
         *
         * @param rowIndex the index of the row we're supposed to check
         * 
         * @return true if this row has been flagged as highlighted
         *
         */
        bool    isHighlightedRow(DWORD rowData) const;
    
        std::set<unsigned long> m_registeredData;

        DWORD   m_highlightForegroundColour;
        DWORD   m_highlightBackgroundColour;
    };

}   // namespace TA_Base_Bus
