//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CategoryNodeView.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_CATEGORYNODEVIEW_H__F2270A61_6B13_4F81_8449_21F1CCDB26FB__INCLUDED_)
#define AFX_CATEGORYNODEVIEW_H__F2270A61_6B13_4F81_8449_21F1CCDB26FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"
#include "CategoryListCtrl.h"


namespace TA_Base_App
{
    class CPlanTreeDoc;
    class CategoryNode;


    /////////////////////////////////////////////////////////////////////////////
    // CCategoryNodeView view

    class CCategoryNodeView : public CView
    {
    protected:
        CCategoryNodeView();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CCategoryNodeView)

     // Operations
    public:
        CPlanTreeDoc *GetDocument();
        CategoryNode *GetCategoryNode();
        CCategoryListCtrl& GetListCtrl();

    protected:
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


    // Attributes
    private:
        CCategoryListCtrl m_categoryListCtrl;


    // Implementation
    protected:
        virtual ~CCategoryNodeView();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CCategoryNodeView)
    protected:
        virtual void OnDraw(CDC* pDC);      // overridden to draw this view
        virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
        virtual void PostNcDestroy();
        // }}AFX_VIRTUAL

    // Generated message map functions
    protected:
        // {{AFX_MSG(CCategoryNodeView)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnCategoryListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnCategoryListItemDblclk(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG
        afx_msg LRESULT OnViewShown(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnResetTreeNodeView(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_CATEGORYNODEVIEW_H__F2270A61_6B13_4F81_8449_21F1CCDB26FB__INCLUDED_)
