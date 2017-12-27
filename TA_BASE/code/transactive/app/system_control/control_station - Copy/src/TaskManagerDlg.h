/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/TaskManagerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_TASKMANAGERDLG_H__9BE81733_6F60_11D5_B60E_0050BAB1D92B__INCLUDED_)
#define AFX_TASKMANAGERDLG_H__9BE81733_6F60_11D5_B60E_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskManagerDlg.h : header file
//
#include "app/system_control/control_station/src/resource.h"
#include "app/system_control/control_station/src/ProcessManager.h"


/////////////////////////////////////////////////////////////////////////////
// CTaskManagerDlg dialog

class CControlStationDlg;

class CTaskManagerDlg : public CDialog
{
// Construction
public:
	CTaskManagerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTaskManagerDlg)
	enum { IDD = IDD_TASK_MANAGER };
	CButton	m_addParamButton;
	CEdit	m_addParamEdit;
	CButton	m_terminateButton;
	CListCtrl	m_parametersList;
	CComboBox	m_processCombo;
	//}}AFX_DATA

	void setIsEditable(const bool isEditable) { m_isEditable = isEditable; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaskManagerDlg)
	afx_msg void OnSelchangeComboProcess();
	afx_msg void OnEndlabeleditListParameters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonTerminate();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButtonAddParam();
	afx_msg void OnUpdateEditAddParam();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    //TODO: Comment
    void loadProcessCombo();

    bool m_isEditable;

    ProcessManager::RunningApplicationMap* m_apps;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKMANAGERDLG_H__9BE81733_6F60_11D5_B60E_0050BAB1D92B__INCLUDED_)
