/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/DutyNotificationDialog.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A dialog which lists:
  *  - the duties that this operator has previously requested and gained from operators holding the duties,
  *  - the duties that this operator has previously requested and denied by operators holding the duties, 
  *  - the duties that are lost to another operator.
  *
  * It is possible for the dialog to display the gained and denied requests together in a single dialog.
  */

#if !defined(AFX_DutyNotificationDialog_H__442816BD_264F_4F0F_B518_07825CA2C03E__INCLUDED_)
#define AFX_DutyNotificationDialog_H__442816BD_264F_4F0F_B518_07825CA2C03E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/system_control/control_station/src/DutyManager.h"

/////////////////////////////////////////////////////////////////////////////
// DutyNotificationDialog dialog

namespace TA_Base_Bus
{
    class SoundManagerProxy;    // forward declaration
}

class DutyNotificationDialog : public CDialog
{
// Construction
public:

    /** 
      * DutyNotificationDialog
      *
      * Constructor.  Takes in the DutyNotificationDetail which it will use
      * to initialise the text on the dialog.
      *
      * @param detail The details of the duty
      */
    DutyNotificationDialog(CWnd* pParent, DutyManager::DutyNotificationDetail* detail);   // constructor


    /** 
      * ~DutyNotificationDialog
      *
      * Destructor
      *
      */
    ~DutyNotificationDialog();


    /** 
      * create
      *
      * Call this to create a modeless instance
      *
      * @return True if successfully created the instance
      */
	BOOL create();


// Dialog Data
	//{{AFX_DATA(DutyNotificationDialog)
	enum { IDD = IDD_DUTY_POPUP_DIALOG };
	CRichEditCtrl	m_textCtrl;
	// TD19075++
	CRichEditCtrl	m_textCtrl2;
	CRichEditCtrl	m_textCtrl3;
	CRichEditCtrl	m_textCtrl4;
	// ++TD10975
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DutyNotificationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DutyNotificationDialog)
	virtual BOOL OnInitDialog();
	virtual LRESULT OnCancel(WPARAM,LPARAM);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void setView(int cnt); // TD19075

    enum EMessageType
    {
        GAINED,
        DENIED,
        LOST, 
		CHANGED // TD19075
    };

    /** 
      * formatContent
      *
      * Formats the content of the duty text.
      *
      * @param format The heading for the section which describes what is happening
      *               to the duty
      * @param data The duty details to be used in the text.
      *
      * @return A string that describes what is happening to a set of duty.
      *
      */
    CString formatContent(const EMessageType& format, const std::vector<DutyManager::DutyDetail>& data);
    
    // Parent of this dialog.  We'll post the user response back to this window
    CWnd* m_parent;

    // Stores the duty details
    DutyManager::DutyNotificationDetail* m_detail;

    // Pointer to the sound interface    
	TA_Base_Bus::SoundManagerProxy* m_soundInterface;

    // Format for the text
    static const CString GAIN_FORMAT;
    static const CString GAIN_DUTYLESS_FORMAT;
    static const CString DENIED_FORMAT;
    static const CString LOST_FORMAT;
    static const CString CHANGED_FORMAT; // TD19075
    static const CString LOST_DUTYLESS_FORMAT;
    static const CString LOCATION_FORMAT;
    static const CString SUBSYSTEM_FORMAT;
	static const CString CHANGE_DUTYLESS_FORMAT; // TD19075
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DutyNotificationDialog_H__442816BD_264F_4F0F_B518_07825CA2C03E__INCLUDED_)
