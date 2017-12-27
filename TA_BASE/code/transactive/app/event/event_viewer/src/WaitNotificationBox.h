/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/WaitNotificationBox.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * WaitNotificationBox is an implementation of CDialog. It is displayed while the application closes so the user knows
 * the application is still active.
 */

#if !defined(AFX_WAITNOTIFICATIONBOX_H__5B1D5C44_DD1B_40FA_8CD3_0558934398C5__INCLUDED_)
#define AFX_WAITNOTIFICATIONBOX_H__5B1D5C44_DD1B_40FA_8CD3_0558934398C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaitNotificationBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// WaitNotificationBox dialog

class WaitNotificationBox : public CDialog
{
// Construction
public:
	WaitNotificationBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(WaitNotificationBox)
	enum { IDD = IDD_WAIT_FOR_CLOSE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WaitNotificationBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(WaitNotificationBox)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAITNOTIFICATIONBOX_H__5B1D5C44_DD1B_40FA_8CD3_0558934398C5__INCLUDED_)
