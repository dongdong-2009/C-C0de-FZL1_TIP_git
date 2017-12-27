/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.cpp $
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

#include "../stdafx.h"

#pragma warning (disable : 4786 4284)

#include "bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

    static const long HIGHLIGHT_FG_COLOUR = RGB(196, 0, 0);
    
    HighlightItemListCtrl::HighlightItemListCtrl()
    :
    m_highlightForegroundColour(HIGHLIGHT_FG_COLOUR),
    m_highlightBackgroundColour(::GetSysColor(COLOR_WINDOW))
    {
    }
    
    
    HighlightItemListCtrl::~HighlightItemListCtrl()
    {
    }
    
    
    BEGIN_MESSAGE_MAP(HighlightItemListCtrl, CListCtrl)
    	//{{AFX_MSG_MAP(HighlightItemListCtrl)
      	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)		
    	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()
    
    
    void HighlightItemListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
    {
    	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
    
    	switch(lplvcd->nmcd.dwDrawStage)
    	{
    	case CDDS_PREPAINT:
    		// ask for item notifications.
    		*pResult = CDRF_NOTIFYITEMDRAW;          
    		break;
    
    	case CDDS_ITEMPREPAINT:        
    
            if(isHighlightedRow(lplvcd->nmcd.dwItemSpec) != 0)
            {
                // If this item is flagged, make it special colour
                lplvcd->clrText = m_highlightForegroundColour;
                lplvcd->clrTextBk = m_highlightBackgroundColour;
                *pResult = CDRF_NEWFONT;
            }
            else
            {
                *pResult = CDRF_DODEFAULT;        
            }
            
    		break;
    
    	default:
    		*pResult = CDRF_DODEFAULT;
            break;
    	}    
    
        return;
    }
    

    void HighlightItemListCtrl::registerHighlightData(unsigned long locationKey)
    {
        m_registeredData.insert(std::set<unsigned long>::value_type(locationKey));
    }
    
    
    void HighlightItemListCtrl::deregisterHighlightData(unsigned long data)
    {
        std::set<unsigned long>::iterator itr = m_registeredData.find(data);

        if (m_registeredData.end() != itr)
        {
            m_registeredData.erase(itr);
        }
    }


    void HighlightItemListCtrl::DeleteItemAndClearHighlightFlag(int nItem)
    {
        // Perform internal check to confirm no other items are associated
        // with this piece of item data (it's unlikely 
        DWORD itemData = GetItemData(nItem);

        for (int i = 0; i < GetItemCount(); ++i )
        {
            if (nItem != i)
            {
                // Confirm not a match
                ASSERT (GetItemData(i) != itemData);            
            }
        }

        deregisterHighlightData(itemData);
            
        // Now may remove the item
        DeleteItem(nItem);
    }


    bool HighlightItemListCtrl::isHighlightedRow(DWORD rowIndex) const
    {
        unsigned long searchKey = GetItemData(rowIndex);
    
        // Return true if the location key exists anywhere in special locations set
        return m_registeredData.end() != m_registeredData.find(searchKey);
    }

    
    void HighlightItemListCtrl::clearHighlightDataSet() 
    { 
        m_registeredData.clear(); 
    }


    void HighlightItemListCtrl::setHighlightColour(DWORD fgColour, DWORD bgColour) 
    { 
        m_highlightForegroundColour = fgColour; 
        m_highlightBackgroundColour = bgColour;
    }

}   // namespace TA_Base_Bus
