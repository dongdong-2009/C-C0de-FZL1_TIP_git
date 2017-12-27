//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanEditorDoc.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Plan Editor GUI is based on an SDI. This class is a representation of the document associated with the
// plan editor frame and its plan node view.

#if !defined(AFX_PLANEDITORDOC_H__B252484F_15AB_4A84_9B40_AF6117E95A2B__INCLUDED_)
#define AFX_PLANEDITORDOC_H__B252484F_15AB_4A84_9B40_AF6117E95A2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"
#include "PlanBrowserDoc.h"
#include "PlanNode.h"


namespace TA_Base_App
{
    class PlanStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanEditorDoc document

    class CPlanEditorDoc : public CPlanBrowserDoc
    {
    protected:
        CPlanEditorDoc();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CPlanEditorDoc)

    // Operations
    public:
        virtual void SetCurrentNode(TreeNode *node);
        virtual bool EditingEnabled() const;


    // Attributes
    private:
        typedef enum { SCA_CUT_STEP, SCA_COPY_STEP, SCA_NO_ACTION } EStepCopyAction;

        EStepCopyAction m_copyStepAction;
        PlanStepList m_copyStepList;


    // Overrides
    public:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanEditorDoc)
        virtual BOOL OnNewDocument();
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CPlanEditorDoc();
        virtual BOOL DoFileSave();

    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

    // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanEditorDoc)
        afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);
        afx_msg void OnUpdatePlanSave(CCmdUI* pCmdUI);
        afx_msg void OnPlanSave();
        afx_msg void OnStepEdit();
        afx_msg void OnStepDelete();
        afx_msg void OnStepNew();
        afx_msg void OnStepCopy();
        afx_msg void OnStepCut();
        afx_msg void OnStepPaste();
        afx_msg void OnFileExit();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANEDITORDOC_H__B252484F_15AB_4A84_9B40_AF6117E95A2B__INCLUDED_)
