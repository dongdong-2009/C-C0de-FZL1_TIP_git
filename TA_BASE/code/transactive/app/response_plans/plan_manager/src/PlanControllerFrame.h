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

#if !defined(AFX_PLANCONTROLLERFRAME_H__D066DA69_9A98_47AA_8F60_B524663A5457__INCLUDED_)
#define AFX_PLANCONTROLLERFRAME_H__D066DA69_9A98_47AA_8F60_B524663A5457__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/TransActiveFrame.h"
#include "PlanControllerToolBarManager.h"
#include "PlanControllerDoc.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanControllerFrame frame

    class CPlanControllerFrame : public TA_Base_Bus::TransActiveFrame
    {
        DECLARE_DYNCREATE(CPlanControllerFrame)
    protected:
        CPlanControllerFrame();           // protected constructor used by dynamic creation
        virtual ~CPlanControllerFrame();

    // Operations
    public:
        CPlanControllerDoc *GetDocument();

    // Attributes
    private:
        CStatusBar m_statusBar;
        PlanControllerToolBarManager m_toolBarManager;

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanControllerFrame)
        public:
        virtual void ActivateFrame(int nCmdShow = -1);
        // }}AFX_VIRTUAL

    // Implementation
    public:
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanControllerFrame)
        afx_msg virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg virtual void OnUpdateToolbarView(CCmdUI* pCmdUI);
        afx_msg virtual void OnViewToolbar();
        afx_msg virtual void OnButtonDropDown(NMHDR* pNMHDR, LRESULT *pResult);
        // }}AFX_MSG
        afx_msg LRESULT OnPlanDetailPageSelectionChanged(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

    #ifndef _DEBUG  // debug version in PlanControllerFrame.cpp
    inline CPlanControllerDoc* CPlanControllerFrame::GetDocument()
       {
        FUNCTION_ENTRY( "GetDocument" );

        return (CPlanControllerDoc*)GetActiveDocument(); }


    #endif

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANCONTROLLERFRAME_H__D066DA69_9A98_47AA_8F60_B524663A5457__INCLUDED_)
