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

#if !defined(AFX_MAINFRAME_H__63E68CB2_4BFA_47A6_A7A1_1A542DCD7DE6__INCLUDED_)
#define AFX_MAINFRAME_H__63E68CB2_4BFA_47A6_A7A1_1A542DCD7DE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)
#include <map>
#include <string>

#include "bus/generic_gui/src/AbstractGUIApplication.h"
#include "bus/generic_gui/src/TransActiveMDIFrame.h"
#include "PlanManagerCommonDefs.h"
#include "MainToolBarManager.h"
#include "ExtStatusControlBar.h"

namespace TA_Base_App
{
    class CPlanBrowserFrame;
    class PlanManagerCorbaDefImpl;


    /////////////////////////////////////////////////////////////////////////////
    // The main frame of the application

    class CMainFrame : public TA_Base_Bus::TransActiveMDIFrame
    {
        DECLARE_DYNAMIC(CMainFrame)

    // Operations
    public:
        CMainFrame(TA_Base_Bus::AbstractGUIApplication* appGUI);
        void AddPlanBrowserFrame(int planViewCommandID, CPlanBrowserFrame *planBrowserFrame);
        CPlanBrowserFrame *GetPlanBrowserFrame(int planViewCommandID);

        void initCorbaObj();
		void SetMainTitle( const std::string& strTitle );
        void SetBaseTitle( const std::string& strTitle );

    // Attributes
    private:
        CMainFrame();
        CExtStatusControlBar m_statusBar;
        CProgressCtrl m_progress;
        MainToolBarManager m_toolBarManager;

        typedef std::map<int, CPlanBrowserFrame *> PlanBrowserFrameMap;
        PlanBrowserFrameMap m_planBrowserFrameMap;

        PlanManagerCorbaDefImpl *m_planManagerServant;

    // Implementation
    public:
        virtual ~CMainFrame();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

    // Overrides
     protected:
       // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CMainFrame)
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        // }}AFX_VIRTUAL

    // Generated message map functions
    protected:
        // {{AFX_MSG(CMainFrame)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnAppAbout();
        afx_msg void OnViewToolbar();
        afx_msg void OnViewPlan(UINT nCommandID);
        afx_msg void OnUpdatePlanView(CCmdUI* pCmdUI);
        afx_msg void OnUpdateToolbarView(CCmdUI* pCmdUI);
        afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg LRESULT onInitCorba(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnTreeNodeSelectionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnCategoryViewSelectionChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnSessionIdChanged(WPARAM wParam, LPARAM lParam);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:
        TA_Base_Bus::AbstractGUIApplication* m_appGUI;
        std::string m_strBaseTitle;
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_MAINFRAME_H__63E68CB2_4BFA_47A6_A7A1_1A542DCD7DE6__INCLUDED_)
