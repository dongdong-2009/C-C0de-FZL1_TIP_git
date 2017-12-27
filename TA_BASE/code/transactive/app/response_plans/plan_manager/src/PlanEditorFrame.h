//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanEditorFrame.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANEDITORFRAME_H__B4765144_6445_48D6_925A_257C11193D2A__INCLUDED_)
#define AFX_PLANEDITORFRAME_H__B4765144_6445_48D6_925A_257C11193D2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/TransActiveFrame.h"
#include "PlanEditorToolBarManager.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanEditorFrame frame

    class CPlanEditorFrame : public TA_Base_Bus::TransActiveFrame
    {
        DECLARE_DYNCREATE(CPlanEditorFrame)
    protected:
        CPlanEditorFrame();           // protected constructor used by dynamic creation
        virtual ~CPlanEditorFrame();

    // Operations
    public:


    // Attributes
    private:
        CStatusBar m_statusBar;
        PlanEditorToolBarManager m_toolBarManager;

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanEditorFrame)
        public:
        virtual void ActivateFrame(int nCmdShow = -1);
        // }}AFX_VIRTUAL

    // Implementation
        // Generated message map functions
        // {{AFX_MSG(CPlanEditorFrame)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnViewToolbar();
        afx_msg void OnUpdateToolbarView(CCmdUI* pCmdUI);
        // }}AFX_MSG
        afx_msg LRESULT OnPlanDetailPageSelectionChanged(WPARAM wParam, LPARAM lParam);
       DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANEDITORFRAME_H__B4765144_6445_48D6_925A_257C11193D2A__INCLUDED_)
