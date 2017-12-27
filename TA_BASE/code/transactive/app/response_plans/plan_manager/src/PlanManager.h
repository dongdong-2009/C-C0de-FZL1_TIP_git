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

#if !defined(AFX_PLANMANAGER_H__7E1B538E_910C_4539_86CB_5556804818FE__INCLUDED_)
#define AFX_PLANMANAGER_H__7E1B538E_910C_4539_86CB_5556804818FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "Resource.h"       // main symbols


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanManagerApp

    class CPlanManagerApp : public TA_Base_Bus::TransActiveWinApp
    {
    // Operations
    public:
        CPlanManagerApp();
        ~CPlanManagerApp();

    // Attributes
    private:
        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanManagerApp)
    public:
        virtual BOOL InitInstance();
        virtual int ExitInstance();
        // }}AFX_VIRTUAL

    // Implementation
        // {{AFX_MSG(CPlanManagerApp)
            // NOTE - the ClassWizard will add and remove member functions here.
            // DO NOT EDIT what you see in these blocks of generated code !
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANMANAGER_H__7E1B538E_910C_4539_86CB_5556804818FE__INCLUDED_)
