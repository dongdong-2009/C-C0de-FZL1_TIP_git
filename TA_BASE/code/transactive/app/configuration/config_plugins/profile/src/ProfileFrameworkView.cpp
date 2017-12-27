/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile/src/ProfileFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of profiles
  * and the right side as a view for the user to configure profile details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/profile/src/StdAfx.h"
#include "app/configuration/config_plugins/profile/src/ProfileFrameworkView.h"

#include "app/configuration/config_plugins/profile/src/ProfileListView.h"
#include "app/configuration/config_plugins/profile/src/ProfileDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(ProfileFrameworkView, CView)


    ProfileFrameworkView::ProfileFrameworkView()
    {
    }


    ProfileFrameworkView::~ProfileFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(ProfileFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(ProfileFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* ProfileFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ProfileDetailView);
    }


    CRuntimeClass* ProfileFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ProfileListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // ProfileFrameworkView diagnostics

    #ifdef _DEBUG
    void ProfileFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ProfileFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



