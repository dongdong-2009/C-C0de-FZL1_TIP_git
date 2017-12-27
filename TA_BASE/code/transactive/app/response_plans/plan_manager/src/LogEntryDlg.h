//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#if !defined(AFX_LOGENTRYDLG_H__11B47202_8D95_467B_9CD3_24EB045A4F0F__INCLUDED_)
#define AFX_LOGENTRYDLG_H__11B47202_8D95_467B_9CD3_24EB045A4F0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStepDlg.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CLogEntryDlg dialog

    class CLogEntryDlg : public CInteractiveStepDlg
    {
    // Construction/destruction
    public:
        CLogEntryDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~CLogEntryDlg();

    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Attributes
    private:
        bool m_isLogEntryMandatory;


    // Dialog Data
    private:
        // {{AFX_DATA(CLogEntryDlg)
        enum { IDD = IDD_LOG_ENTRY_DLG };
        CString    m_query;
        CRichEditCtrl    m_logMessageEdit;
        CButton    m_okBtn;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CLogEntryDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        // }}AFX_VIRTUAL

        // Generated message map functions
        // {{AFX_MSG(CLogEntryDlg)
        afx_msg void OnDestroy();
        afx_msg void OnLogMsgFilterNotify(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnLogMsgChanged();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_LOGENTRYDLG_H__11B47202_8D95_467B_9CD3_24EB045A4F0F__INCLUDED_)
