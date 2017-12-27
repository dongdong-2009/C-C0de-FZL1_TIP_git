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

#if !defined(AFX_PLANINSTANCELISTCTRL_H__2D9A15A8_FC42_4F1B_86D0_C22830B8F2BA__INCLUDED_)
#define AFX_PLANINSTANCELISTCTRL_H__2D9A15A8_FC42_4F1B_86D0_C22830B8F2BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/active_plans_display/src/PlanActiveInstanceListCtrl.h"


namespace TA_Base_App
{
    class PlanInstance;
    class PlanNode;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanInstanceListCtrl

    class CPlanInstanceListCtrl : public TA_Base_Bus::CPlanActiveInstanceListCtrl
    {
    // Construction
    public:
        CPlanInstanceListCtrl(bool planPathVisible = false);
        virtual ~CPlanInstanceListCtrl();


    // Operations
    public:
        virtual void SetSelectedInstance(PlanInstanceSharedPtr planInstance);
        virtual PlanInstanceSharedPtr GetSelectedInstance();
        virtual PlanNode *GetSelectedPlan();

    protected:
        virtual void SelectItem(int item, bool selected = true);

    // Attributes
    private:


    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanInstanceListCtrl)
        virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
        // }}AFX_VIRTUAL

        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanInstanceListCtrl)
        // }}AFX_MSG
        afx_msg LRESULT OnActivePlanDetailUpdate(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANINSTANCELISTCTRL_H__2D9A15A8_FC42_4F1B_86D0_C22830B8F2BA__INCLUDED_)
