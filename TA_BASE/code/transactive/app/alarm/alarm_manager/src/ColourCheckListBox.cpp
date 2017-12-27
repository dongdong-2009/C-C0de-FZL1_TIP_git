/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/ColourCheckListBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "ColourCheckListBox.h"

namespace TA_Base_App
{

    BEGIN_MESSAGE_MAP(CColourCheckListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CColourCheckListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    
    CColourCheckListBox::CColourCheckListBox()
    {
        m_ColorHighlightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
        m_ColorHighlight     = ::GetSysColor(COLOR_HIGHLIGHT);
	    m_cxExtent           = 0;
    }

    void CColourCheckListBox::addItemColours(const std::map<int, COLORREF>& itemColours)
    {
        m_itemColours        = itemColours;
    }

	void CColourCheckListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
	{
        COLORREF oldtextcolor;
		//COLORREF oldbackgroundcolor;

	    //CRect members to store the position of the items
	    CRect rItem;
	    CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);

	    if ((int)lpDrawItemStruct->itemID < 0)
	    {
	        // If there are no elements in the CListBox
	        // based on whether the list box has Focus  or not 
	        // draw the Focus Rect or Erase it,
	        if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && (lpDrawItemStruct->itemState & ODS_FOCUS))
	        {
		        dc->DrawFocusRect(&lpDrawItemStruct->rcItem);
	        }
	        else if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && !(lpDrawItemStruct->itemState & ODS_FOCUS)) 
	        {
		        dc->DrawFocusRect(&lpDrawItemStruct->rcItem); 
	        }
	       
            return;
	    }
        else
        {
            int selChange   = lpDrawItemStruct->itemAction & ODA_SELECT;
		    int focusChange = lpDrawItemStruct->itemAction & ODA_FOCUS;
		    int drawEntire  = lpDrawItemStruct->itemAction & ODA_DRAWENTIRE;

		    if (selChange || drawEntire)
		    {
			    BOOL sel = lpDrawItemStruct->itemState & ODS_SELECTED;

			    int nLen = CListBox::GetTextLen(lpDrawItemStruct->itemID);
			    if (nLen != LB_ERR)
			    {
                    // Get the item string
                    CString itemString;
                    CListBox::GetText(lpDrawItemStruct->itemID, itemString);

                    COLORREF itemColour(RGB(0, 0, 0)); // Default text colour is black
					//TD14950 azenitha++
					//As suggested by Frik the color association is okay to be removed
                    /*std::map<int, COLORREF>::iterator it = m_itemColours.find(lpDrawItemStruct->itemID);
                    if(it != m_itemColours.end())
                    {
                        itemColour = it->second;
                    }*/
					//TD14950 ++azenitha

					//TD15314++ wangyn
					std::map<int, COLORREF>::iterator it = m_itemColours.find(lpDrawItemStruct->itemID);
                    if(it != m_itemColours.end())
                    {
                        itemColour = it->second;
                    }
					//++TD15314

				    COLORREF textcolor = itemColour;
                    oldtextcolor = dc->SetTextColor(textcolor);
					
                    // The following is commented out but kept here in case we decide to make
                    // the background colour the alarm severity colour instead of the text.

                    //COLORREF backgroundcolor = sel ? m_ColorHighlight : itemColour;
					//oldbackgroundcolor = dc->SetBkColor(backgroundcolor);

					// fill the rectangle with the background color the fast way
					//dc->ExtTextOut(0, 0, ETO_OPAQUE, &lpDrawItemStruct->rcItem, NULL, 0, NULL);

					dc->TabbedTextOut(lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, itemString,
						strlen(itemString), 3, (LPINT)0, 0);

					CSize size;
					size = dc->GetOutputTextExtent(itemString);
					int nScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL);
					size.cx += nScrollBarWidth;	// in case of vertical scrollbar

					int cxExtent = (size.cx > m_cxExtent) ? size.cx : m_cxExtent;

					if (cxExtent > m_cxExtent)
					{
						m_cxExtent = cxExtent;
						SetHorizontalExtent(m_cxExtent+(m_cxExtent/32));
					}
				}
		    }

		    if (focusChange || (drawEntire && (lpDrawItemStruct->itemState & ODS_FOCUS)))
            {
			    dc->DrawFocusRect(&lpDrawItemStruct->rcItem);
            }
        }
	   
	}

}
