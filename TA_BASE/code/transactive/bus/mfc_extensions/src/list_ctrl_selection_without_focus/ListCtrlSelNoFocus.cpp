/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.cpp $
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
  *
  * Added functionality to set item text colour based on item data or item index
  * Text colour retained when a selection made, but overriden by setNonSelectableGray.
  *
  * Added functionality to set item background colour based on item data or item index
  * Precedence is same as text colour, and settings for foreground and background colour
  * are fully independent (e.g. can set background colours based on index, and foreground
  * colours based on item data)
  */

#pragma warning (disable : 4786)

#include "../stdafx.h"
#include "ListCtrlSelNoFocus.h"


namespace TA_Base_Bus
{

    ListCtrlSelNoFocus::ListCtrlSelNoFocus()
        : 
        m_grayNonSelectable(true),
        m_forwardItemChangingRequests(FALSE),
        m_verticalScrollBarHidden(false),
        m_horizontalScrollBarHidden(false),
        m_originalRectPopulated(false),
        m_itemChangingRequestsToBlock(0),
        m_itemChangedRequestsToBlock(0),
        m_backgroundColourOverridesSelect(false),
        m_isInCustomDraw(false),
        m_toolTipCtrlCustomizeDone(FALSE),
        m_autoSizeColumn(-1),
        m_scrollWasVisible(FALSE)
    {
    }

    ListCtrlSelNoFocus::~ListCtrlSelNoFocus()
    {
    }


    BEGIN_MESSAGE_MAP(ListCtrlSelNoFocus, CListCtrl)
	    //{{AFX_MSG_MAP(ListCtrlSelNoFocus)
        ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGING, OnItemchanging)
        ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, OnItemchanged)
        ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
        ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnListChanged)
        ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnListChanged)
        ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
        ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void ListCtrlSelNoFocus::PreSubclassWindow() 
    {
        EnableToolTips(TRUE);
        CListCtrl::PreSubclassWindow();
    }

    void ListCtrlSelNoFocus::setItemChangeRequestForwarding(BOOL forwardItemChangingRequest) 
    { 
        m_forwardItemChangingRequests = forwardItemChangingRequest; 
    }


    void ListCtrlSelNoFocus::setNonSelectableGray(bool grayNonSelectable)
    {
        m_grayNonSelectable = grayNonSelectable;
    }


    void ListCtrlSelNoFocus::setBackgroundColourOverride(bool override)
    {
        m_backgroundColourOverridesSelect = override;
    }


    // Default implementation - everything is selectable
    bool ListCtrlSelNoFocus::isSelectable(int rowNumber)
    {
        return true;
    }


    // disables the selection of non-selectable items
    BOOL ListCtrlSelNoFocus::OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	    
        *pResult = 0;

        // Only interested in state changes
	    if (pNMListView->uChanged == LVIF_STATE) 
	    {        
            // if it is being selected
            if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
                 ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
            {
                // if the item is not selectable
                if ( !isSelectable( pNMListView->iItem ) )
                {
                    // stop the change of state
                    *pResult = -1;
                }
            }
	    }

        if (m_itemChangingRequestsToBlock > 0)
        {
            // decrement the count
            m_itemChangingRequestsToBlock--;

            // block the message
            return TRUE;
        }
        
        if (m_forwardItemChangingRequests)
        {
            // Even through the MSDN documentation says return TRUE from an ON_NOTIFY_REFLECT_EX
            // to allow the parent to process the message, we actually have to return FALSE...
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }


    BOOL ListCtrlSelNoFocus::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	    
        *pResult = 0;
        
        // Only interested in state changes (ie selection)
	    if (pNMListView->uChanged != LVIF_STATE) 
	    {        
		    return FALSE;
	    }


        if (m_itemChangedRequestsToBlock > 0)
        {
            // decrement the count
            m_itemChangedRequestsToBlock--;

            // block the message
            return TRUE;
        }

        
        // Even through the MSDN documentation says return TRUE from an ON_NOTIFY_REFLECT_EX
        // to allow the parent to process the message, we actually have to return FALSE...
        return FALSE;
    }


    // draws non-selectable items gray
    // and draws selected items the selected colour when focus is lost
    void ListCtrlSelNoFocus::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
    {
        m_isInCustomDraw = true;

	    *pResult = CDRF_DODEFAULT;
        NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

        // used to keep track of item state between prepaint and postpaint
        static bool itemIsHighlighted = false;

        switch (pLVCD->nmcd.dwDrawStage)
        {
        case CDDS_PREPAINT:

            // We want item prepaint notifications, so...
            *pResult = CDRF_NOTIFYITEMDRAW;
            break;

        case CDDS_ITEMPREPAINT:
            {
                // let windows do the drawing
                *pResult = CDRF_DODEFAULT;

                // get the current item being drawn
                int row = pLVCD->nmcd.dwItemSpec;

                if (IsWindowEnabled())
                {
                    // store the state of this row for the post paint
                    itemIsHighlighted = ( GetItemState(row, LVIS_SELECTED) != 0);

                    // if the item is selected
                    if (itemIsHighlighted)
                    {
                        // set the colours to the highlight colours
                        pLVCD->clrText = getForegroundColourForItem(row);
                        pLVCD->clrTextBk = getBackgroundColourForItem(row);
                    
                        // suppress the messages generated from this change
                        m_itemChangingRequestsToBlock++;
                        m_itemChangedRequestsToBlock++;

                        // Turn off listview highlight otherwise it uses the system colors
                        // out of focus this is black on grey
                        SetItemState(row, ~LVIS_SELECTED, LVIS_SELECTED);

                        // We want item post-paint notifications, so...
                        *pResult |= CDRF_NOTIFYPOSTPAINT;
                    }
                    else if ( (m_grayNonSelectable) && !isSelectable(row) )
                    {
                        // if we are graying out non selectable items
                        // and this item is not selectable
                        // set the colour to the system disabled colour.
                        pLVCD->clrText = GetSysColor(COLOR_GRAYTEXT);
                    }
                    else
                    {
                        // Normal state, apply text colour to item
                        pLVCD->clrText = getForegroundColourForItem(row);
                        pLVCD->clrTextBk = getBackgroundColourForItem(row);
                    }
                }
                else
                {
                    itemIsHighlighted = false;
                }
            }
            break;

        case CDDS_ITEMPOSTPAINT:
            {
                // if we deselected the item in prepaint
                if (itemIsHighlighted)
                {
                    int row = pLVCD->nmcd.dwItemSpec;

                    // suppress the messages generated from this change
                    m_itemChangingRequestsToBlock++;
                    m_itemChangedRequestsToBlock++;

                    // reselect it again
                    SetItemState(row, LVIS_SELECTED, LVIS_SELECTED);
                }

                *pResult = CDRF_DODEFAULT;
            }
            break;

        default:
            *pResult = CDRF_DODEFAULT;
            break;
        }

        m_isInCustomDraw = false;
        return;
    }


    void ListCtrlSelNoFocus::setItemColour(
        DWORD value, 
        DWORD colour, 
        EValueType valueType,
        EColourType colourType)
    {
        ColourMap::iterator itrToUpdate;

        switch (valueType)
        {
        case I_ITEMDATA:
            itrToUpdate = m_itemDataColours.find(value);
            
            if (m_itemDataColours.end() == itrToUpdate)
            {
                // Insert new item
                ColourSetting newItem;
                itrToUpdate = m_itemDataColours.insert(ColourMap::value_type(value, newItem)).first;
            }  
            ASSERT(itrToUpdate != m_itemDataColours.end());         
            break;
        case I_INDEX:
            itrToUpdate = m_itemIndexColours.find(value);
            
            if (m_itemIndexColours.end() == itrToUpdate)
            {
                // Insert new item
                ColourSetting newItem;
                itrToUpdate = m_itemIndexColours.insert(ColourMap::value_type(value, newItem)).first;
            }
            ASSERT(itrToUpdate != m_itemIndexColours.end());
            break;       
        default:
            ASSERT(FALSE);
        }

        // Update state of item        
        switch (colourType)
        {
        case CT_FOREGROUND:
            itrToUpdate->second.foreground.hasBeenSet = true;
            itrToUpdate->second.foreground.colour = colour;
            break;
        case CT_BACKGROUND:
            itrToUpdate->second.background.hasBeenSet = true;
            itrToUpdate->second.background.colour = colour;
            break;
        default:
            ASSERT(FALSE);
        }
    }


    void ListCtrlSelNoFocus::clearItemColourSettings()
    {
        m_itemDataColours.clear();
        m_itemIndexColours.clear();
    }
    

    DWORD ListCtrlSelNoFocus::getForegroundColourForItem(int nItem)
    {
        ColourMap::iterator itr = m_itemIndexColours.find(nItem);

        if (itr != m_itemIndexColours.end() && itr->second.foreground.hasBeenSet)
        {
            // Since item index colours take precedence, return this
            // regardless of contents of m_itemDataColours map
            return itr->second.foreground.colour;
        }

        // Check item data now
        DWORD itemData = GetItemData(nItem);

        itr = m_itemDataColours.find(itemData);

        if (itr != m_itemDataColours.end() && itr->second.foreground.hasBeenSet)
        {
            return itr->second.foreground.colour;
        }
         
        // Otherwise no specific colour set, use default system colours
        bool itemIsSelected = (LVIS_SELECTED == GetItemState(nItem, LVIS_SELECTED) );

        if (itemIsSelected)
        {
            // Selected item
            return GetSysColor(COLOR_HIGHLIGHTTEXT);
        }
        else
        {
            // Normal item
            return GetSysColor(COLOR_WINDOWTEXT);
        }
    }


    DWORD ListCtrlSelNoFocus::getBackgroundColourForItem(int nItem)
    {             
        // Otherwise no specific colour set, use default system colours
        bool itemIsSelected = (LVIS_SELECTED == GetItemState(nItem, LVIS_SELECTED) );

        // Only return the custom background colour if item not selected
        // (unless background colour overrides selected colour)
        if (m_backgroundColourOverridesSelect || !itemIsSelected)
        {             
            ColourMap::iterator itr = m_itemIndexColours.find(nItem);
    
            if (itr != m_itemIndexColours.end() && itr->second.background.hasBeenSet)
            {
                // Since item index colours take precedence, return this
                // regardless of contents of m_itemDataColours map
                return itr->second.background.colour;
            }
    
            // Check item data now
            DWORD itemData = GetItemData(nItem);
    
            itr = m_itemDataColours.find(itemData);
    
            if (itr != m_itemDataColours.end() && itr->second.background.hasBeenSet)
            {
                return itr->second.background.colour;
            }
        }

        if (itemIsSelected)
        {
            // Selected item
            return GetSysColor(COLOR_HIGHLIGHT);
        }
        else
        {
            // Normal item
            return GetSysColor(COLOR_WINDOW);
        }
    }


    void ListCtrlSelNoFocus::setScrollBarVisibility(bool verticalScrollVisible /* = true */, bool horizontalScrollVisible /*= false*/)
    {
        m_verticalScrollBarHidden = !verticalScrollVisible;
        m_horizontalScrollBarHidden = !horizontalScrollVisible;


        // decide which scrollbars are hidden
        int which = ~SB_BOTH;

        // both
        if (m_verticalScrollBarHidden && m_horizontalScrollBarHidden)
        {
            which = SB_BOTH;
        }
        // just vertical
        else if (m_verticalScrollBarHidden)
        {
            which = SB_VERT;
        }
        // just horizontal
        else if (m_horizontalScrollBarHidden)
        {
            which = SB_HORZ;
        }

        int cxvs, cyvs;

        // make sure the original rect is retained
        // for everytime the scrollbar stuff is recalculated
        // otherwise the list shrinks every time
        if (!m_originalRectPopulated)
        {
            GetClientRect(&m_originalRect); // Get client width and height
            m_originalRectPopulated = true;
        }

        // make a copy to mess up
        RECT ierect = m_originalRect;

        cxvs = GetSystemMetrics (SM_CXVSCROLL); //Get the system metrics - VERT
        cyvs = GetSystemMetrics (SM_CYVSCROLL); //Get the system metrics - HORZ

        if(which==SB_HORZ) cxvs=0; //Set VERT to zero when choosen HORZ
        if(which==SB_VERT) cyvs=0; //Set HORZ to zero when choosen VERT

        // Here we set the position of the window to the clientrect + the size of the scrollbars
        SetWindowPos(NULL, ierect.left, ierect.top, ierect.right+cxvs, ierect.bottom+cyvs, SWP_NOMOVE | SWP_NOZORDER);

        // Here we modify the rect so the right part is subbed from the rect.
        if(which==SB_BOTH||which==SB_HORZ) ierect.bottom -= ierect.top;
        if(which==SB_BOTH||which==SB_VERT) ierect.right -= ierect.left;

        // Just to be safe that the left/top corner is 0...
        ierect.top = 0;
        ierect.left = 0;

        HRGN iehrgn = NULL; // This range is created base on which scrollbar that is going to be removed!

        // The -2 is probably a border of some kind that we also need to remove. I could not find any good
        // metrics that gave me an 2 as an answer. So insted we makes it static with -2.
        if(which==SB_BOTH) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right-2, ierect.bottom-2);
        if(which==SB_HORZ) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right, ierect.bottom-2);
        if(which==SB_VERT) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right-2, ierect.bottom);

        // After the range has been made we add it...
        SetWindowRgn (iehrgn, TRUE);
    }


    void ListCtrlSelNoFocus::clearItemColour(DWORD value, EValueType valueType)
    {
        switch (valueType)
        {
        case I_ITEMDATA:
            m_itemDataColours.erase(value);
            break;
        case I_INDEX:
            m_itemIndexColours.erase(value);
            break;       
        default:
            ASSERT(FALSE);
        }
    }


    BOOL ListCtrlSelNoFocus::DeleteColourAndItem(int nItem)
    {   
        DWORD itemData = GetItemData(nItem);

        if (m_itemDataColours.end() != m_itemDataColours.find(itemData))
        {
            // Special check, failure indicates user is almost certainly
            // going to encounter undesirable behaviour (having
            // colour settings mysteriously cleared)
            for (int i = 0; i < GetItemCount(); ++i )
            {   
                if (nItem != i)
                {
                    // Confirm this item data not a match with any other item
                    ASSERT (GetItemData(i) != itemData);            
                }
            }
        }

        clearItemColour(nItem,              I_INDEX);
        clearItemColour(itemData,           I_ITEMDATA);
        
        return DeleteItem(nItem);
    }

    void ListCtrlSelNoFocus::setAutoSizeColumn(int pIndex, BOOL pResize)
    {
        m_autoSizeColumn = pIndex;
        if (pIndex > 0 && pResize)
        {
            autoResizeColumn(pIndex);
        }
    }

    int ListCtrlSelNoFocus::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
    {
        int row, col;
        RECT cellrect;
        row = CellRectFromPoint(point, &cellrect, &col );
    
        if ( row == -1 ) 
            return -1;
    
        pTI->hwnd = m_hWnd;
        pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);

        pTI->lpszText = LPSTR_TEXTCALLBACK;
    
        pTI->rect = cellrect;
    
        return pTI->uId;
    }


    void ListCtrlSelNoFocus::OnListChanged(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        if (m_autoSizeColumn >= 0)
        {
            BOOL hasScroll = GetItemCount() > GetCountPerPage();

            if (hasScroll != m_scrollWasVisible)
            {
                m_scrollWasVisible = hasScroll;
                if (!hasScroll) // hide scrollbar so we get correct size
                {
                    ShowScrollBar(SB_VERT, FALSE);
                }
                autoResizeColumn(m_autoSizeColumn);
            }
        }

	    *pResult = 0;
    }

    void ListCtrlSelNoFocus::autoResizeColumn(int pIndex)
    {
        // Make sure that the ListView is in LVS_REPORT
        if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
            return;

        CHeaderCtrl* headerCtrl = GetHeaderCtrl();
        if (headerCtrl != NULL)
        {
            RECT rc;
            GetClientRect(&rc);
            int colCount = headerCtrl->GetItemCount();
        
            if (pIndex < colCount)
            {
                for (int i = 0; i < colCount; ++i)
                {
                    if (i != pIndex)
                    {
                        rc.right -= GetColumnWidth(i);
                    }
                }
            
                if (rc.right < 0) rc.right = 0;
            
                SetColumnWidth(pIndex, rc.right);
            }
        }
    }

    // CellRectFromPoint	- Determine the row, col and bounding rect of a cell
    // Returns		- row index on success, -1 otherwise
    // point		- point to be tested.
    // cellrect		- to hold the bounding rect
    // col			- to hold the column index
    int ListCtrlSelNoFocus::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const
    {
        int colnum;
    
        // Make sure that the ListView is in LVS_REPORT
        if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
            return -1;
    
        // Get the top and bottom row visible
        int row = GetTopIndex();
        int bottom = row + GetCountPerPage();
        if( bottom > GetItemCount() )
            bottom = GetItemCount();
    
        // Get the number of columns
        CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
        int nColumnCount = pHeader->GetItemCount();
    
        // Loop through the visible rows
        for( ;row <=bottom;row++)
        {
            // Get bounding rect of item and check whether point falls in it.
            CRect rect;
            GetItemRect( row, &rect, LVIR_BOUNDS );
            if( rect.PtInRect(point) )
            {
                // Now find the column
                for( colnum = 0; colnum < nColumnCount; colnum++ )
                {
                    int colwidth = GetColumnWidth(colnum);
                    if( point.x >= rect.left 
                        && point.x <= (rect.left + colwidth ) )
                    {
                        RECT rectClient;
                        GetClientRect( &rectClient );
                        if( col ) *col = colnum;
                        rect.right = rect.left + colwidth;
                    
                        // Make sure that the right extent does not exceed
                        // the client area
                        if( rect.right > rectClient.right ) 
                            rect.right = rectClient.right;
                        *cellrect = rect;
                        return row;
                    }
                    rect.left += colwidth;
                }
            }
        }
        return -1;
    }

    BOOL ListCtrlSelNoFocus::OnToolTipText(UINT, NMHDR * pNMHDR, LRESULT * pResult)
    {
        UINT nID = pNMHDR->idFrom;
    
        if( nID == 0 )
        {
            // Notification in NT from automatically
            return FALSE;   	// created tooltip
        }
    
        if (!m_toolTipCtrlCustomizeDone)
        {
#if _MFC_VER >= 0x0700 // MFC Version 7 and above
			AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
#else
			_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
#endif
            CToolTipCtrl *pToolTip = pThreadState->m_pToolTip;
            // Set max tip width in pixel.
            // you can change delay time, tip text or background
            //   color also. enjoy yourself!
            pToolTip->SetMaxTipWidth(300);
            m_toolTipCtrlCustomizeDone = TRUE;
        }
    
        // need to handle both ANSI and UNICODE versions of the message
        TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
        TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
        CString strTipText;
    
        int row = ((nID - 1) >> 10) & 0x3fffff ;
        int col = (nID - 1) & 0x3ff;
        strTipText = GetItemText( row, col );
    
    #ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(m_toolTipText, strTipText, MAX_TOOLTIP_LENGTH);
            pTTTA->lpszText = m_toolTipText;
        }
        else
        {
             _mbstowcsz(reinterpret_cast<wchar_t*>(m_toolTipText), strTipText, MAX_TOOLTIP_LENGTH);
             pTTTW->lpszText = reinterpret_cast<wchar_t*>(m_toolTipText);
        }
    #else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(reinterpret_cast<char*>(m_toolTipText), strTipText, MAX_TOOLTIP_LENGTH);
            pTTTA->lpszText = reinterpret_cast<char*>(m_toolTipText);
        }
        else
        {
            lstrcpyn(m_toolTipText, strTipText, MAX_TOOLTIP_LENGTH);
            pTTTW->lpszText = m_toolTipText;
        }

    #endif

        *pResult = 0;
    
        return TRUE;    // message was handled
    }

} // TA_Base_Bus

