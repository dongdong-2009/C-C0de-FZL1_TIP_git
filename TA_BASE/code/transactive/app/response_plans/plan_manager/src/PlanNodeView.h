//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanNodeView.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANNODEVIEW_H__AAEAF6DB_9935_4A87_A387_2759257482C5__INCLUDED_)
#define AFX_PLANNODEVIEW_H__AAEAF6DB_9935_4A87_A387_2759257482C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanGeneralDetailPage.h"
#include "PlanStepDetailPage.h"
#include "PlanFlowchartPage.h"


namespace TA_Base_App
{
    class CPlanBrowserDoc;
    class PlanNode;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanNodeView form view

    class CPlanNodeView : public CScrollView
    {
    protected:
        CPlanNodeView();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CPlanNodeView)

    // Operations
    public:

    protected:
        CPlanBrowserDoc *GetDocument();
        PlanNode *GetPlanNode();


    // Attributes:
    private:
        CPropertySheet m_planPropertySheet;
        CPlanGeneralDetailPage m_generalDetailPage;
        CPlanStepDetailPage m_stepDetailPage;
        //CPlanFlowchartPage m_flowchartPage;


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanNodeView)
    public:
        virtual void OnDraw(CDC* pDC);
        virtual void OnInitialUpdate();
    protected:
        virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
        // }}AFX_VIRTUAL

    // Implementation
    protected:
       ~CPlanNodeView();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
        // {{AFX_MSG(CPlanNodeView)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        // }}AFX_MSG
        afx_msg LRESULT OnSelectActivePlanInstance(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnResetTreeNodeView(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnRefreshPlanStepList(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANNODEVIEW_H__AAEAF6DB_9935_4A87_A387_2759257482C5__INCLUDED_)
