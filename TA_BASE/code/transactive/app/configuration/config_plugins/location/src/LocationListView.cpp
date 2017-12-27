/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/location/src/LocationListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all locations available to be configured. This view must communicate
  * with the LocationDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/location/src/StdAfx.h"
#include "app/configuration/config_plugins/location/src/LocationListView.h"
#include "app/configuration/config_plugins/location/src/LocationComponent.h"
#include "app/configuration/config_plugins/location/src/resource.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(LocationListView, CListView)


    LocationListView::LocationListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(LocationComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_LOCATION) );
    }


    LocationListView::~LocationListView()
    {
    }


BEGIN_MESSAGE_MAP(LocationListView, AbstractListView)
	//{{AFX_MSG_MAP(LocationListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void LocationListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // LocationListView diagnostics

    #ifdef _DEBUG
    void LocationListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void LocationListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

