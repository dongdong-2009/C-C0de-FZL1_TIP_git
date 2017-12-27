/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller_groups/src/SCGroupFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of SC groups
  * and the right side as a view for the user to configure SC group details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/system_controller_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/system_controller_groups/src/SCGroupFrameworkView.h"

#include "app/configuration/config_plugins/system_controller_groups/src/SCGroupListView.h"
#include "app/configuration/config_plugins/system_controller_groups/src/SCGroupDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(SCGroupFrameworkView, CView)


    SCGroupFrameworkView::SCGroupFrameworkView()
    {
    }


    SCGroupFrameworkView::~SCGroupFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(SCGroupFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(SCGroupFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* SCGroupFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(SCGroupDetailView);
    }


    CRuntimeClass* SCGroupFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(SCGroupListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // SCGroupFrameworkView diagnostics

    #ifdef _DEBUG
    void SCGroupFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void SCGroupFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



