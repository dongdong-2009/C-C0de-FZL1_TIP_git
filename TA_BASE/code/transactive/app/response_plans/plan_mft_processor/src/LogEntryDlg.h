//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/LogEntryDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#pragma once

#include "app/response_plans/plan_mft_processor/src/resource.h"
#include "app/response_plans/plan_mft_processor/src/InteractiveStepDlg.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CLogEntryDlg dialog

    class CLogEntryDlg : public CInteractiveStepDlg
    {
        DECLARE_DYNAMIC(CLogEntryDlg)
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
        CEdit      m_logMessageEdit;
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
        afx_msg void OnLogMsgChanged();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App
