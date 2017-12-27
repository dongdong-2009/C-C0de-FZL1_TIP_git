/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/ArchiveManager.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * User interface for Archive Operations
  *
  */

#if !defined(AFX_ARCHIVEMANAGER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
#define AFX_ARCHIVEMANAGER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "bus/generic_gui/src/TransActiveWinApp.h"

namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CArchiveManagerApp:
    // See ArchiveManager.cpp for the implementation of this class
    //

    class CArchiveManagerApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
        /**
          * CArchiveManagerApp
          *
          * Standard constructor.
          */

        CArchiveManagerApp();

        /**
          * ~CArchiveManagerApp
          *
          * Standard destructor.
          */

        virtual ~CArchiveManagerApp() {};
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CArchiveManagerApp)
    public:
        //}}AFX_VIRTUAL
        
        // Implementation
        
        //{{AFX_MSG(CArchiveManagerApp)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

            
    private:
        //
        // Disable the copy constructor and assignment operator.
        //

        CArchiveManagerApp( const CArchiveManagerApp& );
        CArchiveManagerApp& operator=( const CArchiveManagerApp& );
    };
    
} // TA_Base_App


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVEMANAGER_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
