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

#if !defined(AFX_LAUNCHGUIAPPDLG_H__AAC7FD87_5B36_44A5_8F08_4A587EF48B23__INCLUDED_)
#define AFX_LAUNCHGUIAPPDLG_H__AAC7FD87_5B36_44A5_8F08_4A587EF48B23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStepDlg.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CLaunchGuiAppDlg dialog

    class CLaunchGuiAppDlg : public CInteractiveStepDlg
    {
    // Construction
    public:
        CLaunchGuiAppDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);


    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CLaunchGuiAppDlg)
        enum { IDD = IDD_LAUNCH_GUI_APP_DLG };
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CLaunchGuiAppDlg)
    public:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CLaunchGuiAppDlg)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_App

#endif // !defined(AFX_LAUNCHGUIAPPDLG_H__AAC7FD87_5B36_44A5_8F08_4A587EF48B23__INCLUDED_)
