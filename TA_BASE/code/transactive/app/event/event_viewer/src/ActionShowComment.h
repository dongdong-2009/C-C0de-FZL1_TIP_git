/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/ActionShowComment.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class holds the comment dialog that allows the user to view comments. This is a
  * modified version of the dialog used in the Alarm Manager.
  */

#if !defined(AFX_COMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_)
#define AFX_COMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_

#include "app/event/event_viewer/EventListComponent/src/IEventAction.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"
#include "app/event/event_viewer/src/resource.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DisplayItem;

    class ActionShowComment : public IEventAction, public CDialog
    {
    // Construction
    public:
	    ActionShowComment(CWnd* pParent = NULL);   // standard constructor
		virtual ~ActionShowComment();

    // Dialog Data
	    //{{AFX_DATA(ActionShowComment)
        enum { IDD = IDD_SHOW_COMMENT };
	    CString	m_comment;
        //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ActionShowComment)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
        virtual unsigned int getActionId();
        virtual void executeAction( Filter* filter );

    protected:

        // Generated message map functions
        //{{AFX_MSG(ActionShowComment)
        virtual BOOL OnInitDialog();
        //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMENTDIALOG_H__3B556F26_E4E8_47F8_885F_E1D0DD5F8E5D__INCLUDED_)
