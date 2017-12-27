//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ApprovedPlanDoc.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_APPROVEDPLANDOC_H__21F28B68_ED89_4DE5_9E13_286B2B2FFA52__INCLUDED_)
#define AFX_APPROVEDPLANDOC_H__21F28B68_ED89_4DE5_9E13_286B2B2FFA52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanTreeDoc.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CApprovedPlanDoc document

    class CApprovedPlanDoc : public CPlanTreeDoc
    {
    protected:
        CApprovedPlanDoc();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CApprovedPlanDoc)

    // Operations
    public:
        virtual const std::string &GetRootNodeLabel();

        // ITreeNodeFilter interface
        virtual bool isRelevant(TreeNode &treeNode);


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CApprovedPlanDoc)
    public:
        virtual BOOL OnNewDocument();
        virtual void OnCloseDocument();
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CApprovedPlanDoc();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
    protected:
        // {{AFX_MSG(CApprovedPlanDoc)
            // NOTE - the ClassWizard will add and remove member functions here.
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_APPROVEDPLANDOC_H__21F28B68_ED89_4DE5_9E13_286B2B2FFA52__INCLUDED_)
