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

#if !defined(AFX_PLANTREEDOC_H__C12FC01D_7368_4455_88DA_15412D4B3105__INCLUDED_)
#define AFX_PLANTREEDOC_H__C12FC01D_7368_4455_88DA_15412D4B3105__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include "PlanManagerCommonDefs.h"
#include "PlanBrowserDoc.h"
#include "ITreeNodeFilter.h"


namespace TA_Base_App
{
    class TreeNode;
    class PlanNode;
    class RootNode;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanTreeDoc document

    class CPlanTreeDoc : public CPlanBrowserDoc,
                         public ITreeNodeFilter
    {
    // Construction
    protected:
        CPlanTreeDoc();        // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CPlanTreeDoc)

    // Operations
    public:
        virtual const std::string &GetRootNodeLabel();

        // TD18095,jianghp
        virtual void OnGenericGuiInitCompleted();
        // TD18095,jianghp
        void AddToCategoryViewNodes(TreeNode *treeNode);
        void RemoveFromCategoryViewNodes(TreeNode *treeNode);
        void PurgeCategoryViewNodes();
        bool ArePlanNodesSelected();
        TreeNodeList GetSelectedNodes();

        // ITreeNodeFilter interface
        virtual bool isRelevant(TreeNode &treeNode);

     private:
         // replace all new-line chars with spaces
         void ReplaceNLChars (CString& value);

    // Attributes
    private:
        typedef enum { CNA_COPY_PLAN, CNA_CUT_PLAN, CNA_NO_ACTION} ECopyNodeAction;

        ECopyNodeAction m_copyNodeAction;
        TreeNodeList m_copyNodes;

        typedef std::map<TreeNodeId, TreeNode *> TreeNodeMap;
        TreeNodeMap m_categoryViewNodeMap;  // Contains the nodes selected in the CategoryNodeView shown for
                                            // a category selected in the plan tree.


    // Overrides
    public:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanTreeDoc)
        virtual BOOL OnNewDocument();
        virtual void OnCloseDocument();
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CPlanTreeDoc();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

    // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanTreeDoc)
        afx_msg void OnUpdateTreeNodeCmd(CCmdUI* pCmdUI);
        afx_msg void OnEditPlan();
        afx_msg void OnControlPlan();
        afx_msg void OnApprovePlan();
        afx_msg void OnUnapprovePlan();
        afx_msg void OnSchedulePlan();
        afx_msg void OnNewPlan();
        afx_msg void OnDeletePlan();
        afx_msg void OnCopyPlan();
        afx_msg void OnPastePlan();
        afx_msg void OnCutPlan();
        afx_msg void OnNewCategory();
        afx_msg void OnDeleteCategory();
        afx_msg void OnDeleteCategoryOrPlan();
        afx_msg void OnEditStep();
        afx_msg void OnImportPlan();
        afx_msg void OnExportPlan();
        afx_msg void OnMakePlanVisible();
        afx_msg void OnMakePlanInvisible();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANTREEDOC_H__C12FC01D_7368_4455_88DA_15412D4B3105__INCLUDED_)
