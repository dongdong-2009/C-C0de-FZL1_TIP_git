/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/SystemControllerFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of system controllers
  * and the right side as a view for the user to configure system controller details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/system_controller/src/StdAfx.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerFrameworkView.h"

#include "app/configuration/config_plugins/system_controller/src/SystemControllerListView.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(SystemControllerFrameworkView, CView)


    SystemControllerFrameworkView::SystemControllerFrameworkView()
    {
    }


    SystemControllerFrameworkView::~SystemControllerFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(SystemControllerFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(SystemControllerFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* SystemControllerFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(SystemControllerDetailView);
    }


    CRuntimeClass* SystemControllerFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(SystemControllerListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // SystemControllerFrameworkView diagnostics

    #ifdef _DEBUG
    void SystemControllerFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void SystemControllerFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



