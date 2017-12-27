/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/location/src/LocationFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of locations
  * and the right side as a view for the user to configure location details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/location/src/StdAfx.h"
#include "app/configuration/config_plugins/location/src/LocationFrameworkView.h"

#include "app/configuration/config_plugins/location/src/LocationListView.h"
#include "app/configuration/config_plugins/location/src/LocationDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(LocationFrameworkView, CView)


    LocationFrameworkView::LocationFrameworkView()
    {
    }


    LocationFrameworkView::~LocationFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(LocationFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(LocationFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* LocationFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(LocationDetailView);
    }


    CRuntimeClass* LocationFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(LocationListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // LocationFrameworkView diagnostics

    #ifdef _DEBUG
    void LocationFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void LocationFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



