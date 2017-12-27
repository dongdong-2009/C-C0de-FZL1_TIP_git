//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanBrowserFrame.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANBROWSERFRAME_H__C1DF2E25_4C4E_4790_AF08_F526BEA7C853__INCLUDED_)
#define AFX_PLANBROWSERFRAME_H__C1DF2E25_4C4E_4790_AF08_F526BEA7C853__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class CMainFrame;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanBrowserFrame frame

    class CPlanBrowserFrame : public CMDIChildWnd
    {
        DECLARE_DYNCREATE(CPlanBrowserFrame)
    protected:
        CPlanBrowserFrame();           // protected constructor used by dynamic creation

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanBrowserFrame)
    public:
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        virtual void ActivateFrame(int nCmdShow);
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CPlanBrowserFrame();

    protected:
        CMainFrame *GetMainFrame();

        // Generated message map functions
        // {{AFX_MSG(CPlanBrowserFrame)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANBROWSERFRAME_H__C1DF2E25_4C4E_4790_AF08_F526BEA7C853__INCLUDED_)
