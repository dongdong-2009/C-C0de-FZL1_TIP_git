/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/EventViewer.h $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2013/05/31 17:03:01 $
 * Last modified by:  $Author: lu.xiangmei $
 * 
 * EventViewer is the TransactiveWinApp derived main application class. This is the class that bootstraps the application.
 */

#if !defined(AFX_EVENTVIEWER_H__80C1DC2F_2204_4CCF_AEEA_44E407EBB90C__INCLUDED_)
#define AFX_EVENTVIEWER_H__80C1DC2F_2204_4CCF_AEEA_44E407EBB90C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <string>

#include "app/event/event_viewer/src/resource.h"		// main symbols

#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// EventViewerApp:
// See EventViewer.cpp for the implementation of this class
//


// Define messages for communication with filter dialog
#define WM_USER_FILTER_DLG_CLOSED              WM_USER + 0x100
#define WM_USER_FILTER_DLG_APPLY               WM_USER + 0x101
#define WM_USER_FILTER_DLG_CANCEL              WM_USER + 0x102
#define WM_USER_DISPLAY_SHUTDOWN_BOX           WM_USER + 0x103
#define WM_USER_CLOSE_MAIN_WINDOW              WM_USER + 0x104
#define WM_USER_FILTER_DLG_NAMED_FILTER_CHANGE WM_USER + 0x105
#define WM_USER_UPDATE_EVENTFILTERS_RIGHTS	   WM_USER + 0X106 //TD17143
#define WM_USER_FILTER_DLG_CLOSED_NOFILTERUPDATED_NORELOAD  WM_USER + 0X107
#define WM_USER_FILTER_DLG_CLOSED_FILTERUPDATED_NORELOAD  WM_USER + 0X108
#define WM_USER_FILTER_DLG_CLOSED_FILTERUPDATED_RELOAD  WM_USER + 0X109

class EventViewerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	EventViewerApp();

    static const std::string RPARAM_ALARMID;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EventViewerApp)
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(EventViewerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL InitInstance();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTVIEWER_H__80C1DC2F_2204_4CCF_AEEA_44E407EBB90C__INCLUDED_)
