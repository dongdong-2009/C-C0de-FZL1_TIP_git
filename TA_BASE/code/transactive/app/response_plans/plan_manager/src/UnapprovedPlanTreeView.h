//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/UnapprovedPlanTreeView.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_UNAPPROVEDPLANTREEVIEW_H__50AA12A5_905E_416D_9CD8_8DBA4897788A__INCLUDED_)
#define AFX_UNAPPROVEDPLANTREEVIEW_H__50AA12A5_905E_416D_9CD8_8DBA4897788A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanTreeView.h"


namespace TA_Base_App
{
    class CUnapprovedPlanDoc;


    /////////////////////////////////////////////////////////////////////////////
    // CUnapprovedPlanTreeView view

    class CUnapprovedPlanTreeView : public CPlanTreeView
    {
    protected:
        CUnapprovedPlanTreeView();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CUnapprovedPlanTreeView)

    // Operations
    public:
        CUnapprovedPlanDoc* GetDocument();

    protected:
        virtual void ProcessPlanApprovalStateUpdate(const TA_Base_Core::ApprovalStateUpdate &approvalStateUpdate);
        virtual void ProcessInsertNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId);
        virtual void ProcessDeleteNodeUpdate(const TA_Base_Core::DeleteNodeUpdate &deleteNodeUpdate);


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CUnapprovedPlanTreeView)
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CUnapprovedPlanTreeView();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
    protected:
        // {{AFX_MSG(CUnapprovedPlanTreeView)
        afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    #ifndef _DEBUG  // debug version in UnapprovedPlanTreeView.cpp
    inline CUnapprovedPlanDoc* CUnapprovedPlanTreeView::GetDocument()
        {
        FUNCTION_ENTRY( "GetDocument" );

        return (CUnapprovedPlanDoc*)CPlanTreeView::GetDocument(); }


    #endif

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_UNAPPROVEDPLANTREEVIEW_H__50AA12A5_905E_416D_9CD8_8DBA4897788A__INCLUDED_)
