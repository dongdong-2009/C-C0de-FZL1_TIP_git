/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This provides a fixed column header control (for a CListCtrl).
  * The user cant resize columns, and the resize cursor isnt shown,
  * you cant simply disable the header in case you want a clickable sorting header.
  *
  * To use this, in the OnCreate, PreSubclassWindow or OnInitDialog just call subclassHeader
  * passing the existing header control (using GetHeaderCtrl).
  */

#include "../stdafx.h"
#include "FixedHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

    FixedHeaderCtrl::FixedHeaderCtrl()
    {}


    FixedHeaderCtrl::~FixedHeaderCtrl()
    {}


    BEGIN_MESSAGE_MAP(FixedHeaderCtrl, CHeaderCtrl)
	    //{{AFX_MSG_MAP(FixedHeaderCtrl)
        ON_NOTIFY_REFLECT(HDN_BEGINTRACKA, onBeginTrack)
        ON_NOTIFY_REFLECT(HDN_BEGINTRACKW, onBeginTrack)
        ON_NOTIFY_REFLECT(HDN_DIVIDERDBLCLICKA, onDividerDblClick)
        ON_NOTIFY_REFLECT(HDN_DIVIDERDBLCLICKW, onDividerDblClick)
	    ON_WM_SETCURSOR()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL FixedHeaderCtrl::subclassHeader(CHeaderCtrl *pHeader)
    {	
	    ASSERT( pHeader && pHeader->GetSafeHwnd());

	    if (!SubclassWindow(pHeader->GetSafeHwnd()))
	    {
		    return FALSE;
	    }

        return TRUE;
    }


    void FixedHeaderCtrl::onBeginTrack(NMHDR* pNMHDR, LRESULT* pResult)
    {
        // returning true to prevent tracking (resizing column)
        *pResult = TRUE;
    }


    void FixedHeaderCtrl::onDividerDblClick(NMHDR* pNMHDR, LRESULT* pResult)
    {
        // returning true to prevent tracking (resizing column)
        *pResult = TRUE;
    }


    BOOL FixedHeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
    {
        // return true to keep the current pointer
        return TRUE;
    }

    
    void FixedHeaderCtrl::expandColumnToFitListWidth(CListCtrl& listCtrl, DWORD columnIndex, bool accountForScrollBar)
    {
        int nColumnCount = listCtrl.GetHeaderCtrl()->GetItemCount();
        
        ASSERT(columnIndex < (DWORD)nColumnCount);

        if (nColumnCount > 0)
        {
            // Determine the amount of space to be fill in by extending the last column
            CRect rc;
            listCtrl.GetClientRect(rc);
            int headerTotalWidth = rc.Width();
            
            if (accountForScrollBar)
            {
                headerTotalWidth -= GetSystemMetrics(SM_CXVSCROLL);
            }

            int sumColumnWidths = 0;
            for (int i = 0; i < nColumnCount; ++i )
            {
                sumColumnWidths += listCtrl.GetColumnWidth(i);                
            }

            // If column widths span off the end of the list control, we can't do anything about
            // the width of the target column in this list            
            int paddingWidth = headerTotalWidth - sumColumnWidths;
                
            if (paddingWidth > 0)
            {
                DWORD newWidth = listCtrl.GetColumnWidth(columnIndex) + paddingWidth;
                listCtrl.SetColumnWidth(columnIndex, newWidth);
            }
        }

    }

	// lizettejl
	// TD 13832 - start
	void FixedHeaderCtrl::OnMouseMove( UINT nFlags, CPoint point )
	{
		// initialise the m_hiddenHdRect
		if (m_hiddenHdRect.IsRectEmpty() != 0)
		{
			CRect rcH;
			GetItemRect(0, &rcH);
			// Reset the rect of the hidden column 0
			m_hiddenHdRect.top = rcH.top-5;
			m_hiddenHdRect.bottom = rcH.bottom+5;
			m_hiddenHdRect.left = rcH.left-5;
			m_hiddenHdRect.right = rcH.right+10;
		}

		m_hiddenHdRect.NormalizeRect();
		if ( m_hiddenHdRect.IsRectEmpty() == 0 && m_hiddenHdRect.PtInRect(point) )
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		else
		{
			CWnd::OnMouseMove(nFlags, point);
		}
	}
	// TD 13832 - end
    

} // TA_Base_Bus
