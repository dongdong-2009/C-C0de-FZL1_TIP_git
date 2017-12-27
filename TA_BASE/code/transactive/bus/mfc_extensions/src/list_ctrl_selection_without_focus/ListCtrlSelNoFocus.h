/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ListCtrlSelNoFocus is a list control that shows its
  * selections in the system highlight colour even when
  * it loses focus.
  *
  * This also provides the ability to set whether an item is selectable
  * or not, while overriding the custom draw this is also useful
  * functionality.
  *
  * Non-selectable items will be drawn the system disabled colour (by default)
  * Set whether you want this with setNonSelectableGray().
  *
  * To determine whether individual items are selectable,
  * override isSelectable.
  *
  * Added functionality to set item text colour based on item data or item index
  * Text colour retained when a selection made, but overriden by setNonSelectableGray.
  *
  * Scroll bars can also be repeatedly hidden and shown.
  *
  * If this list is to be inherited from, ensure that if you are handling itemchanging and item changed
  * messages, you call the handlers for this class first - otherwise they are never called.
  * Only proceed handling the message if the handlers return FALSE, otherwise, ignore the message.
  *
  * Added functionality to set item background colour based on item data or item index
  * Precedence is same as text colour, and settings for foreground and background colour
  * are fully independent (e.g. can set background colours based on index, and foreground
  * colours based on item data)
  */

#if !defined(LIST_CTRL_SEL_NO_FOCUS_H)
#define LIST_CTRL_SEL_NO_FOCUS_H

#include <map>

namespace TA_Base_Bus
{
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) ListCtrlSelNoFocus : public CListCtrl
    #else
    class AFX_EXT_CLASS ListCtrlSelNoFocus : public CListCtrl
    #endif
    {

    public:

	    ListCtrlSelNoFocus();
	    virtual ~ListCtrlSelNoFocus();

    public:


        /*
         * setItemChangeRequestForwarding
         * 
         * By default, an LVN_ITEMCHANGING notification is handled within this control (and the parent
         *   control is not given the opportunity to handle it).
         *   Setting the input parameter to TRUE means we handle the LVN_ITEMCHANGING
         *   request ourselves, giving us the opportunity to bypass the non selectable
         *   semantics defined in this class (if required)
         *
         * @param forwardItemChangingRequest - TRUE to forward LVN_ITEMCHANGING requests on
         *
         */
        void setItemChangeRequestForwarding(BOOL forwardItemChangingRequest);
    
        enum EValueType
        {
            I_ITEMDATA,
            I_INDEX
        };

        enum EColourType
        {
            CT_FOREGROUND,
            CT_BACKGROUND
        };


        /**
         * setItemColour
         *
         * Sets the text colour for a specific item.  It is possible to introduce conflicting
         *  colours (when setting colours by item data AND item index).  Upon duplicate
         *  colours set for a single item, the colour set for the item index takes precedence.
         * Preferably set colours using I_ITEMDATA, so colouring can remain order independent
         *  (if the control is sorted, colour settings won't need to be updated)
         *
         * @param value the value identifying the item(s) this colour
         *          should be applied to (meaning depends on valueType)
         * @param colour the foreground or background colour 
         * @param valueType specifies what 'value' should be interpreted as, either
         *          an item data value, or an item index
         * @param colourType defines the textColour classification (foreground or background)
         *
         * @see clearItemColourSettings (for resetting colours during list refresh)
         */
        void setItemColour(DWORD value, 
                            DWORD colour, 
                            EValueType valueType = I_ITEMDATA,
                            EColourType colourType = CT_FOREGROUND);

        /**
         * clearItemColour
         *
         * Inverse operation of setItemColour, clears both the foreground and
         *      the background colour settings associated with the item
         * NOTE: Careful if you call setItemColour multiple times with
         *          same item data key value, referencing counting it not performed..
         *          so one call to this function will clear all colour settings
         *          for other items that may have same item data
         *
         */
        void clearItemColour(DWORD value, EValueType valueType = I_ITEMDATA);

        /**
         * DeleteColourAndItem
         *
         * Clears the colours settings for the specified item (if present)
         * then deletes it.
         * @pre Iff there is an item data colour associated with the item at the specified
         *      index, then the item data for this item doesn't match item data for any
         *       other item 
         *      (if it did, we would be clearing the colour setting for the other item)
         *  
         * @param nItem the item to delete
         * @return CListCtrl::DeleteItem(nItem)
         */
        BOOL DeleteColourAndItem(int nItem);

        /**
         * clearItemColourSettings
         *
         * Clears all colour information set for all items in this list
         *
         */
        void clearItemColourSettings();


        /** 
          * setScrollBarVisibility
          *
          * Sets the scroll bar visiblity, this can be called repeatedly
          * without shrinking the viewable area like other implementations.
          *
          * @param verticalScrollVisible    whether the vertical scroll bar is visible
          * @param horizontalScrollVisible  whether the horizontal scroll bar is visible
          */
        void setScrollBarVisibility(bool verticalScrollVisible = true,
                                    bool horizontalScrollVisible = false);


        /** 
          * setNonSelectableGray
          *
          * Set whether to draw non-selectable items gray or the normal colour
          *
          * @param grayNonSelectable    whether to gray non-selectable items.
          */
        void setNonSelectableGray(bool grayNonSelectable);

        /**
         * setBackgroundColourOverride
         *
         * Changes the default behaviour for the background colour used when 
         *  drawing selected items.  By default, the override is false (if an
         *  item is selected, the default system background colour is used)
         *
         * @param override use true to have the background colour override the selected item colour
         *
         */
        void setBackgroundColourOverride(bool override);
        
        /**
         * @return true if this list control is within it's custom draw function
         *          (hence can detect when an OnCustomDraw-borne selection change occurred)
         */
        bool isWithinCustomDraw() { return m_isInCustomDraw; }

        /**
         * setAutoSizeColumn
         *
         * Allow a column to be resized automatically to fit the client area
         *
         * @param pResize use TRUE to resize now
         */
        void setAutoSizeColumn(int pIndex, BOOL pResize = TRUE);

        virtual int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;


    // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ListCtrlSelNoFocus)
    protected:
        virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Generated message map functions
    protected:

        
	//{{AFX_MSG(ListCtrlSelNoFocus)
    afx_msg BOOL OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg BOOL OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnListChanged( NMHDR * pNotifyStruct, LRESULT* result );
    afx_msg BOOL OnToolTipText( UINT, NMHDR * pNMHDR, LRESULT * pResult );
	//}}AFX_MSG
	    
    DECLARE_MESSAGE_MAP()

    protected:
        
        /**
         * getForegroundColourForItem
         *
         * @return the text colour that should be used for drawing the specified item
         * @param nItem defines the item index
         */
        virtual DWORD getForegroundColourForItem(int nItem);

        /**
         * getBackgroundColourForItem
         *
         * @return the background colour that should be used for drawing the specified item
         * @param nItem defines the item index
         */
        virtual DWORD getBackgroundColourForItem(int nItem);

        /** 
          * isSelectable
          *
          * Given the row number, return true if this item
          * should be selectable.
          *
          * Override to use this functionality, by default - all are selectable.
          *
          * @param rowNumber the row number.
          *
          * @return true if the item can be selected
          */
        virtual bool isSelectable(int rowNumber);

    private:
        
        struct ColourSetting
        {
            struct Colour
            {
                Colour() : colour(0), hasBeenSet(false) {}
                DWORD colour;
                bool hasBeenSet;
            };
            Colour foreground;
            Colour background;
        };
        typedef std::map<DWORD, ColourSetting> ColourMap;
        ColourMap m_itemDataColours;
        ColourMap m_itemIndexColours;

        bool m_grayNonSelectable;
        BOOL m_forwardItemChangingRequests;

        // these are used to hide scrollbars
        bool m_verticalScrollBarHidden;
        bool m_horizontalScrollBarHidden;
        // the original client rect
        RECT m_originalRect;
        bool m_originalRectPopulated;

        // this is used to block the item changed (and item changing) messages
        // generated when drawing selected items.
        volatile int m_itemChangingRequestsToBlock;
        volatile int m_itemChangedRequestsToBlock;

        /// Set to true to have the background colour override the selected item
        /// background colour (otherwise, background colour is ignored if item selected)
        bool    m_backgroundColourOverridesSelect;

        bool    m_isInCustomDraw;

        enum
        {
            MAX_TOOLTIP_LENGTH = 255
        };
        
        void autoResizeColumn(int pIndex);
        int m_autoSizeColumn;
        BOOL m_scrollWasVisible;

        int CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const;
        
        TCHAR m_toolTipText[MAX_TOOLTIP_LENGTH + 1];
        BOOL m_toolTipCtrlCustomizeDone;
        

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_Base_Bus

#endif // !defined(LIST_CTRL_SEL_NO_FOCUS_H)
