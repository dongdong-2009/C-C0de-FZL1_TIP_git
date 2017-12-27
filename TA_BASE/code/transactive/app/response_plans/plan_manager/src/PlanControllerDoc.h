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

#if !defined(AFX_PLANCONTROLLERDOC_H__9138BB3D_69C1_4505_B5ED_37F0E3A30D4A__INCLUDED_)
#define AFX_PLANCONTROLLERDOC_H__9138BB3D_69C1_4505_B5ED_37F0E3A30D4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanBrowserDoc.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanControllerDoc document

    class CPlanControllerDoc : public CPlanBrowserDoc
    {
    protected:
        CPlanControllerDoc();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CPlanControllerDoc)

    // Operations
    public:
        virtual bool ControlEnabled() const;


    // Attributes
    private:


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanControllerDoc)
    public:
        virtual BOOL OnNewDocument();
        virtual void OnCloseDocument();
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CPlanControllerDoc();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanControllerDoc)
        afx_msg void OnUpdatePlanControlCmd(CCmdUI* pCmdUI);
        afx_msg void OnStartPlan();
        afx_msg void OnCustomisePlan();
        afx_msg void OnStopPlan();
        afx_msg void OnPausePlan();
        afx_msg void OnResumePlan();
        afx_msg void OnTakeControlOfPlan();
        afx_msg void OnReleaseControlOfPlan();
        afx_msg void OnSchedulePlan();
        afx_msg void OnCustomiseStep();
        afx_msg void OnFileExit();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANCONTROLLERDOC_H__9138BB3D_69C1_4505_B5ED_37F0E3A30D4A__INCLUDED_)
