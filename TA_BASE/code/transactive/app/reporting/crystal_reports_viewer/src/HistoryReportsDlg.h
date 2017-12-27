/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/HistoryReportsDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_HISTORYREPORTSDLG_H__B1767829_1004_4243_A796_F6E2291495F9__INCLUDED_)
#define AFX_HISTORYREPORTSDLG_H__B1767829_1004_4243_A796_F6E2291495F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistoryReportsDlg.h : header file
//

#ifdef TRANSACTIVE_BUILD
namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        /////////////////////////////////////////////////////////////////////////////
        // HistoryReportsDlg dialog

        class HistoryReportsDlg : public CDialog
        {
        // Construction
        public:
	        HistoryReportsDlg(CWnd* pParent = NULL);   // standard constructor
	        CString m_cstrSelection;
	        CString m_cstrDirectory;
	        CString m_cstrFileSpec;
	        bool filesExist();

        // Dialog Data
	        //{{AFX_DATA(HistoryReportsDlg)
	        enum { IDD = IDD_HISTORY_DIALOG };
	        CListBox	m_lbHistoricalReports;
	        //}}AFX_DATA


        // Overrides
	        // ClassWizard generated virtual function overrides
	        //{{AFX_VIRTUAL(HistoryReportsDlg)
	        protected:
	        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	        //}}AFX_VIRTUAL

        // Implementation
        protected:
	        void populateListBox();

	        // Generated message map functions
	        //{{AFX_MSG(HistoryReportsDlg)
	        virtual BOOL OnInitDialog();
	        virtual void OnOK();
	        virtual void OnCancel();
	        afx_msg void OnDblclkReportsListing();
	        //}}AFX_MSG
	        DECLARE_MESSAGE_MAP()
        };

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTORYREPORTSDLG_H__B1767829_1004_4243_A796_F6E2291495F9__INCLUDED_)
