//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepPastePositionDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class represents a simple dialog displayed to the operator when pasting steps within a plan.
// It enables the operator to pick the paste position wrt currently selected step(s), i.e. before or after.
// If the currently selected step is the End Step, the paste after option is disabled.
//

#if !defined(AFX_PLANSTEPPASTEPOSITIONDLG_H__328C1C47_1149_4CB5_AD84_3B735AFF5E68__INCLUDED_)
#define AFX_PLANSTEPPASTEPOSITIONDLG_H__328C1C47_1149_4CB5_AD84_3B735AFF5E68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepPastePositionDlg dialog

    class CPlanStepPastePositionDlg : public CDialog
    {
    // Operations
    public:
        CPlanStepPastePositionDlg(bool endStepSelected, CWnd* pParent = NULL);   // standard constructor

        typedef enum { PPT_PASTE_BEFORE, PPT_PASTE_AFTER } EPastePositionType;

        EPastePositionType getPastePosition() { return m_pastePosition; }


    // Attributes
    private:
        bool m_endStepSelected;
        EPastePositionType m_pastePosition;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepPastePositionDlg)
        enum { IDD = IDD_PLAN_STEP_PASTE_DLG };
            // NOTE: the ClassWizard will add data members here
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepPastePositionDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepPastePositionDlg)
        virtual void OnOK();
    virtual BOOL OnInitDialog();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPPASTEPOSITIONDLG_H__328C1C47_1149_4CB5_AD84_3B735AFF5E68__INCLUDED_)
