// ListHeaderCtrl.cpp: implementation of the ListHeaderCtrl class.
//
//////////////////////////////////////////////////////////////////////
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/ListHeaderCtrl.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This provides a column header control (for a CListCtrl).
  * The user cant selected resize columns, and the resize cursor isnt shown,
  * you cant simply disable the header in case you want a clickable sorting header.
  *
  * To use this, in the OnCreate, PreSubclassWindow or OnInitDialog just call subclassHeader
  * passing the existing header control (using GetHeaderCtrl).
  */

#include "bus\alarm\alarm_list_control\src\ListHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	ListHeaderCtrl::ListHeaderCtrl()
	{ 
		m_hiddenHdRect.top = 0;
		m_hiddenHdRect.bottom = 0;
		m_hiddenHdRect.left = 0;
		m_hiddenHdRect.right = 0;
	}

	ListHeaderCtrl::~ListHeaderCtrl()
	{}

    BEGIN_MESSAGE_MAP(ListHeaderCtrl, CHeaderCtrl)
	    //{{AFX_MSG_MAP(ListHeaderCtrl)
        ON_NOTIFY_REFLECT(HDN_BEGINTRACKA, onBeginTrack)
        ON_NOTIFY_REFLECT(HDN_BEGINTRACKW, onBeginTrack)
		ON_NOTIFY_REFLECT(HDN_TRACKA, OnTrack)
		ON_NOTIFY_REFLECT(HDN_TRACKW, OnTrack)
	    //ON_WM_SETCURSOR()
		ON_WM_MOUSEMOVE()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL ListHeaderCtrl::subclassHeader(CHeaderCtrl *pHeader)
    {	
	    ASSERT( pHeader && pHeader->GetSafeHwnd());

	    if (!SubclassWindow(pHeader->GetSafeHwnd()))
	    {
		    return FALSE;
	    }

        return TRUE;
    }


    void ListHeaderCtrl::onBeginTrack(NMHDR* pNMHDR, LRESULT* pResult)
    {
		NMHEADER* hdr = reinterpret_cast<NMHEADER*>( pNMHDR );
		if (hdr->iItem == 0)
		{
			// set the cursor to column normal
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			// returning true to prevent tracking (resizing column)
			*pResult = TRUE;
		}
		else
		{
			// set the cursor to column divide
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
			// return false to continue the tracking
			*pResult = FALSE;
		}

    }


	void ListHeaderCtrl::OnTrack(NMHDR* pNMHDR, LRESULT* pResult)
	{
		NMHEADER* hdr = reinterpret_cast<NMHEADER*>( pNMHDR );
		if (hdr->iItem == 0)
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			*pResult = TRUE;
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
			*pResult = FALSE;
		}

	}

    /*BOOL ListHeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
    {
        // return false to continue the cursor pointer
        return FALSE;
    }*/

	void ListHeaderCtrl::OnMouseMove( UINT nFlags, CPoint point )
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
    
    void ListHeaderCtrl::expandColumnToFitListWidth(CListCtrl& listCtrl, DWORD columnIndex, bool accountForScrollBar)
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

} //end of namespace
