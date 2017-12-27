//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanTreeFrame.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANTREEFRAME_H__A8C00AC9_BF98_4FA2_8427_30B719CC79FB__INCLUDED_)
#define AFX_PLANTREEFRAME_H__A8C00AC9_BF98_4FA2_8427_30B719CC79FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanBrowserFrame.h"
#include "PlanTreeDoc.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class CPlanTreeView;
    class PlanNode;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanTreeFrame frame

    class CPlanTreeFrame : public CPlanBrowserFrame
    {
        DECLARE_DYNCREATE(CPlanTreeFrame)
    public:
        CPlanTreeFrame();

    // Operations
    public:
        CPlanTreeDoc *GetDocument();

    protected:
        virtual CRuntimeClass *GetLeftPaneRuntimeClass();
        virtual CRuntimeClass *GetRightPaneRuntimeClass();
        virtual CPlanTreeView *GetLeftPane();
        virtual CView *GetRightPane();


    // Attributes
    protected:
        CSplitterWnd m_wndSplitter;


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanTreeFrame)
        public:
        virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CPlanTreeFrame();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif


    // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanTreeFrame)
        // }}AFX_MSG
        afx_msg LRESULT OnTreeNodeSelectionChanged(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

    #ifndef _DEBUG  // debug version in PlanTreeFrame.cpp
    inline CPlanTreeDoc* CPlanTreeFrame::GetDocument()
       {
        FUNCTION_ENTRY( "GetDocument" );

        return (CPlanTreeDoc*)GetActiveDocument(); }


    #endif


    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANTREEFRAME_H__A8C00AC9_BF98_4FA2_8427_30B719CC79FB__INCLUDED_)
