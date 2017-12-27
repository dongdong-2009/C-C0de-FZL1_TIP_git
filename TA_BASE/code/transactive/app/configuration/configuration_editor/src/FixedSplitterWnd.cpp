/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/FixedSplitterWnd.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This inherits from CSplitterWnd and implements a splitter that cannot be resized.
  */
 
#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/FixedSplitterWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CFixedSplitterWnd

    CFixedSplitterWnd::CFixedSplitterWnd()
    {
        m_cxSplitter=0;
        m_cySplitter=0;

        m_cxBorderShare=0;
        m_cyBorderShare=0;

        // Width of the splitter bar
        m_cxSplitterGap=1;
        m_cySplitterGap=1;
    }


    CFixedSplitterWnd::~CFixedSplitterWnd()
    {
    }


BEGIN_MESSAGE_MAP(CFixedSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CFixedSplitterWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CFixedSplitterWnd message handlers

    void CFixedSplitterWnd::OnMouseMove(UINT nFlags, CPoint point) 
    {
        // This prevents the dragging of the splitter bar
	    CWnd::OnMouseMove(nFlags, point);
    }


    void CFixedSplitterWnd::OnInvertTracker(const CRect& rect)
    {
        // Override so normal behaviour does not occur
        // If this is not overridden then when the user clicks on the
        // resizing bar the program crashes.
    }


    void CFixedSplitterWnd::OnLButtonDown(UINT nFlags, CPoint point) 
    {
        // Override and pass the message straight to CWnd otherwise the splitter
        // changes the mouse cursor to be the resizing cursor.
        CWnd::OnLButtonDown(nFlags, point);
    }
}

