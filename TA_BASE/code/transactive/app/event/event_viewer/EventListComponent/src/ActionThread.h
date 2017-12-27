/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/EventListComponent/src/ActionThread.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * This class performs an action in a separate thread to the main thread. This is to ensure the action
 * does not freeze the main application's update.
 *
 * Borrowed from the Alarm List Control.
 */

#if !defined(AFX_ACTIONTHREAD_H__E16E2BFF_2ECB_4C32_A940_64B8DFC34A0C__INCLUDED_)
#define AFX_ACTIONTHREAD_H__E16E2BFF_2ECB_4C32_A940_64B8DFC34A0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ActionThread : public CWinThread
{
	DECLARE_DYNCREATE(ActionThread)
protected:
    /**
     * Constructor
     * 
     * protected constructor used by dynamic creation
     */
    ActionThread();


    /**
     * Destructor
     */
    virtual ~ActionThread();


public:
    static const UINT PERFORM_ACTION;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ActionThread)
    public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(ActionThread)
	    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    afx_msg void performThreadedAction( WPARAM wParam, LPARAM lParam );
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIONTHREAD_H__E16E2BFF_2ECB_4C32_A940_64B8DFC34A0C__INCLUDED_)
