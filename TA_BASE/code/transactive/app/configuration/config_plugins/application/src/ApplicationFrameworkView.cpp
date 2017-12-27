/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/application/src/ApplicationFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of applications
  * and the right side as a view for the user to configure application details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/application/src/StdAfx.h"
#include "app/configuration/config_plugins/application/src/ApplicationFrameworkView.h"

#include "app/configuration/config_plugins/application/src/ApplicationListView.h"
#include "app/configuration/config_plugins/application/src/ApplicationDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(ApplicationFrameworkView, CView)


    ApplicationFrameworkView::ApplicationFrameworkView()
    {
    }


    ApplicationFrameworkView::~ApplicationFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(ApplicationFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(ApplicationFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* ApplicationFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ApplicationDetailView);
    }


    CRuntimeClass* ApplicationFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ApplicationListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // ApplicationFrameworkView diagnostics

    #ifdef _DEBUG
    void ApplicationFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ApplicationFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



