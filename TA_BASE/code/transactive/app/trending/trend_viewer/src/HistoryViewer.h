/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/HistoryViewer.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// HistoryViewer.h : main header file for the HISTORYVIEWER application
//

#if !defined(AFX_HISTORYVIEWER_H__D0BB3C07_825C_11D6_B243_0050BAB25370__INCLUDED_)
#define AFX_HISTORYVIEWER_H__D0BB3C07_825C_11D6_B243_0050BAB25370__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"        // main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CHistoryViewerApp:
// See HistoryViewer.cpp for the implementation of this class
//

namespace TA_Base_App
{

        class CHistoryViewerApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
        CHistoryViewerApp();

    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CHistoryViewerApp)
        public:
        //}}AFX_VIRTUAL

    // Implementation

        //{{AFX_MSG(CHistoryViewerApp)
            // NOTE - the ClassWizard will add and remove member functions here.
            //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTORYVIEWER_H__D0BB3C07_825C_11D6_B243_0050BAB25370__INCLUDED_)





