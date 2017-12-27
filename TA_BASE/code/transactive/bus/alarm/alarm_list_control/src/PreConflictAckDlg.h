/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/PreConflictAckDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_PRECONFLICTACKDLG_H__68084FA2_9623_4D84_8F58_D1931B445A89__INCLUDED_)
#define AFX_PRECONFLICTACKDLG_H__68084FA2_9623_4D84_8F58_D1931B445A89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreConflictAckDlg.h : header file
//

#include "resource.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

/////////////////////////////////////////////////////////////////////////////
// PreConflictAckDlg dialog

	struct ConflictingTrains
	{
		int offendingTrain;
		std::string offendingTrainString;
		int owningTrain;
		std::string owningTrainString;
	};
	
class PreConflictAckDlg : public CDialog
{

	
	// Construction
public:
	ConflictingTrains GetAlarmParameters();
	PreConflictAckDlg(CWnd* pParent = NULL);   // standard constructor

	int DoModal(TA_Base_Bus::AlarmDataItem* details);


	
// Dialog Data
	//{{AFX_DATA(PreConflictAckDlg)
	enum { IDD = IDD_PRECONFLICT_ACK_DLG };
	CButton	m_ok;
	CButton	m_cancel;
	CButton	m_train1RadioButton;
	CButton	m_train2RadioButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PreConflictAckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PreConflictAckDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	ConflictingTrains decodeParameterString(std::string parameterString);

	TA_Base_Bus::AlarmDataItem* m_details;
	int m_train1;
	int m_train2;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRECONFLICTACKDLG_H__68084FA2_9623_4D84_8F58_D1931B445A89__INCLUDED_)
