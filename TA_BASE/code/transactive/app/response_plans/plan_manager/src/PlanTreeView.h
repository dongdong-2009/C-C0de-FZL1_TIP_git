//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanTreeView.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANTREEVIEW_H__26115A01_7089_46F5_B187_1A32E6A0384F__INCLUDED_)
#define AFX_PLANTREEVIEW_H__26115A01_7089_46F5_B187_1A32E6A0384F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanTreeDoc.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanTreeView tree view

    class CPlanTreeView : public CTreeView
    {
    protected:
        CPlanTreeView();            // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CPlanTreeView)

    // Operations
    public:
        CPlanTreeDoc* GetDocument();

    protected:
        virtual HTREEITEM FindNode(const std::string& nodePath, bool expandTree = false);
        virtual HTREEITEM FindNode(TreeNodeId nodeId, bool expandTree = false);
        virtual HTREEITEM FindClosestAncestor(TreeNodeId nodeId);
        virtual HTREEITEM FindNodeOrClosestAncestor(TreeNodeId nodeId);
        virtual void DeleteNode(TreeNodeId nodeId, bool purgeEmptyAncestors = false);
        virtual void AddNode(TreeNodeId nodeId, HTREEITEM parentItem);
        virtual void SortChildren(HTREEITEM parentItem);
        virtual void CollapseBranch(HTREEITEM item);

        virtual void ProcessRparamExecPlanIdUpdate();
        virtual void ProcessRparamViewPlanIdUpdate();
        virtual void ProcessRparamLockCategoryUpdate();
        virtual void ProcessPlanApprovalStateUpdate(const TA_Base_Core::ApprovalStateUpdate &approvalStateUpdate);
        virtual void ProcessModifyNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId);
        virtual void ProcessInsertNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId);
        virtual void ProcessInsertNodeIntoRootUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId);
        virtual void ProcessMoveNodeUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId);
        virtual void ProcessMoveNodeToRootUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId);
        virtual void ProcessMoveNodeFromRootUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId);
        virtual void ProcessDeleteNodeUpdate(const TA_Base_Core::DeleteNodeUpdate &deleteNodeUpdate);
        virtual void ProcessDeleteNodeFromRootUpdate(const TA_Base_Core::NodeIds &nodeIds);
        virtual void ProcessDeleteNodeFromRootUpdate(TreeNodeId nodeId);
          virtual void ProcessRparamAssocAlarmIdUpdate ();
    private:
        bool m_guardTreeAccess; // Is tree access restricted to a locked category and its descendants


    // Overrides
    public:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanTreeView)
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        virtual void OnDraw(CDC* pDC);  // overridden to draw this view
        virtual void OnInitialUpdate(); // called first time after construct
        virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CPlanTreeView();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

    protected:

    // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanTreeView)
        afx_msg void OnItemDoubleClicked(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
         afx_msg void OnItemSelectionChanging(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnItemSelected(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnBeginItemLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnEndItemLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnDestroy();
        afx_msg void OnRenameCategory();
        afx_msg BOOL OnEditPlan(UINT cmdId);
        // }}AFX_MSG
        afx_msg LRESULT OnViewShown(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

    #ifndef _DEBUG  // debug version in PlanTreeView.cpp
    inline CPlanTreeDoc* CPlanTreeView::GetDocument()
       {
        FUNCTION_ENTRY( "GetDocument" );

        return (CPlanTreeDoc*)m_pDocument; }


    #endif

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANTREEVIEW_H__26115A01_7089_46F5_B187_1A32E6A0384F__INCLUDED_)
