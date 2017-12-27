/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_extractor/src/EventExtractorDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// EventExtractorDlg.h : header file
//

#if !defined(AFX_EVENTEXTRACTORDLG_H__11E69A62_FADD_47CA_A6E7_980F7EC9B918__INCLUDED_)
#define AFX_EVENTEXTRACTORDLG_H__11E69A62_FADD_47CA_A6E7_980F7EC9B918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// EventExtractorDlg dialog

#include <vector>

#include "bus/mfc_extensions/src/bitmap_and_text_push_button/FooButton.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "core/data_access_interface/src/AlarmData.h"


namespace TA_Base_Core
{
    class ISubsystem;   // forward declaration
}



class EventExtractorDlg : public CDialog
{
// Construction
public:
	EventExtractorDlg(CWnd* pParent = NULL);	// standard constructor


// Dialog Data
	//{{AFX_DATA(EventExtractorDlg)
	enum { IDD = IDD_EVENTEXTRACTOR_DIALOG };
	CDateTimeCtrl	m_startTimeCtrl;
	CDateTimeCtrl	m_startDateCtrl;
	CDateTimeCtrl	m_endTimeCtrl;
	CDateTimeCtrl	m_endDateCtrl;
	CButton	m_OKButton;
	CEdit	m_fileNameEdit;
	TA_Base_Bus::ListCtrlSelNoFocus	m_subsystemList;
	FooButton	m_allEvents_Radio;
	FooButton	m_subsystem_Radio;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EventExtractorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

	// Static constants for identifying columns.
    static const std::string COLUMN_DATETIME;
    static const std::string COLUMN_SEVERITY;
    static const std::string COLUMN_ASSET;
    static const std::string COLUMN_DESCRIPTION;
    static const std::string COLUMN_VALUE;
    static const std::string COLUMN_MMS;
    static const std::string COLUMN_DSS;
    static const std::string COLUMN_AVL;
    static const std::string COLUMN_OPERATOR;
    static const std::string COLUMN_COMMENT;

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(EventExtractorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAllevents();
	afx_msg void OnSubsystem();
	afx_msg void OnPath();
	afx_msg void OnChangeFilename();
	afx_msg void OnDatetimechangeStartDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeStartTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeEndDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeEndTime(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnClearSelection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:


	bool m_isAllEvents;

	CString m_fileName;

	CTime	m_startTime;
	CTime	m_endTime;

	std::vector<TA_Base_Core::ISubsystem*> m_subsystems;

	/** 
      * checkButton
      *
      * Checks the radio/check button
      *
      * @param type The type of button - radio or check
      * @param itemId The resource Id
      * @param wantCheck True if want to check
      *
      */
    void checkButton(const FooButton::Type type, const int itemId, const bool wantCheck);


    /** 
      * enableControl
      *
      * Enables/disables the control
      *
      * @param itemId The resource Id
      * @param enable Enables by default.  Disables if false.
      *
      */
    void enableControl(const int itemId, const int enable = true);


	const std::string& getMmsStatusIdentifier(const std::string& mmsStatus);
	const std::string& getDssPlanStatusIdentifier(const std::string& dssStatus);
	const std::string& getAvlStatusIdentifier(const std::string& avlStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTEXTRACTORDLG_H__11E69A62_FADD_47CA_A6E7_980F7EC9B918__INCLUDED_)
