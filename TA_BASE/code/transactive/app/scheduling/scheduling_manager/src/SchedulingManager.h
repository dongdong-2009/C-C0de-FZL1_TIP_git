/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManager.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class inherits from TransActiveWinApp.
  */

#if !defined(AFX_SCHEDULINGMANAGER_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_)
#define AFX_SCHEDULINGMANAGER_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/scheduling/scheduling_manager/src/resource.h"       // main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CSchedulingManagerApp:
// See SchedulingManager.cpp for the implementation of this class
//
namespace TA_Base_App
{
    class CSchedulingManagerApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
	    CSchedulingManagerApp();

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSchedulingManagerApp)
	    public:
	    virtual BOOL InitInstance();
	    //}}AFX_VIRTUAL

    // Implementation
	    //{{AFX_MSG(CSchedulingManagerApp)
		    // NOTE - the ClassWizard will add and remove member functions here.
		    //    DO NOT EDIT what you see in these blocks of generated code !
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULINGMANAGER_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_)
