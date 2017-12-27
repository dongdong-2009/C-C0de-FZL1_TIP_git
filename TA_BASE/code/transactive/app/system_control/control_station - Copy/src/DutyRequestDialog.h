/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/DutyRequestDialog.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A dialog which lists requested duties and allow the operator to accept or 
  * deny requests.  The dialog will automatically accept the request if the 
  * operator does not reply before timeout.
  *
  */


#if !defined(AFX_DUTYREQUESTDIALOG_H__3162273A_F542_47BE_89BD_E280F1D6CF0D__INCLUDED_)
#define AFX_DUTYREQUESTDIALOG_H__3162273A_F542_47BE_89BD_E280F1D6CF0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/system_control/control_station/src/DutyManager.h"

/////////////////////////////////////////////////////////////////////////////
// DutyRequestDialog dialog

namespace TA_Base_Bus
{
    class SoundManagerProxy;    // forward declaration
}

class DutyRequestDialog : public CDialog
{
// Construction
public:

    /** 
      * DutyRequestDialog
      *
      * Constructor.  Takes in the DutyDetail which it will use
      * to initialise the text on the dialog.
      *
      * @param pParent The parent of this dialog.  DON'T pass in NULL as we're going
      *                to pass back the user response through this
      * @param detail The details of the duty
      *
      */
    DutyRequestDialog(CWnd* pParent, DutyManager::DutyRequestDetail* detail);


    /** 
      * ~DutyRequestDialog
      *
      * Destructor
      *
      */
    ~DutyRequestDialog();


    /** 
      * create
      *
      * Call this to create a modeless instance
      *
      * @return True if successfully created the instance
      */
	BOOL create();

// Dialog Data
	//{{AFX_DATA(DutyRequestDialog)
	enum { IDD = IDD_DUTY_REQUEST_DIALOG };
	CButton	m_okCtrl;
	CButton	m_cancelCtrl;
	CStatic	m_timeCtrl;
	CRichEditCtrl	m_textCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DutyRequestDialog)
	protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DutyRequestDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAccept();
	afx_msg void OnDeny();
	afx_msg LRESULT OnCancel(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    /** 
      * formatContent
      *
      * Formats the content of the duty text.
      *
      * @return A string that describes what is happening to a set of duty.
      *
      */
    CString formatContent();

    // Parent of this dialog.  We'll post the user response back to this window
    CWnd* m_parent;

    // Stores the duty details
    DutyManager::DutyRequestDetail* m_detail;

    // The count down value that will be displayed on the dialog box
    unsigned int m_countDown;

    // This is the id of the timeout timer in the dialog box
    static const UINT TIMEOUT_TIMER;

    // default timout 
    static const unsigned int DutyRequestDialog::DEFAULT_TIMER_TIMEOUT;

    // Pointer to the sound interface    
	TA_Base_Bus::SoundManagerProxy* m_soundInterface;

    // Format for the text
    CString PRE_TIMEOUT_FORMAT;
    CString POST_TIMEOUT_FORMAT;

	//TD15112++
	// Format for the text
    static const CString LOCATION_FORMAT;
    static const CString SUBSYSTEM_FORMAT;
	//++TD15112
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUTYREQUESTDIALOG_H__3162273A_F542_47BE_89BD_E280F1D6CF0D__INCLUDED_)
