/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile_groups/src/ProfileGroupsFrameworkView.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of profile groups
  * and the right side as a view for the user to configure profile group details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/profile_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/profile_groups/src/ProfileGroupsFrameworkView.h"

#include "app/configuration/config_plugins/profile_groups/src/ProfileGroupsListView.h"
#include "app/configuration/config_plugins/profile_groups/src/ProfileGroupsDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(ProfileGroupsFrameworkView, CView)


    ProfileGroupsFrameworkView::ProfileGroupsFrameworkView()
    {
    }


    ProfileGroupsFrameworkView::~ProfileGroupsFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(ProfileGroupsFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(ProfileGroupsFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* ProfileGroupsFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ProfileGroupsDetailView);
    }


    CRuntimeClass* ProfileGroupsFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ProfileGroupsListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // ProfileFrameworkGroupsView diagnostics

    #ifdef _DEBUG
    void ProfileGroupsFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ProfileGroupsFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



