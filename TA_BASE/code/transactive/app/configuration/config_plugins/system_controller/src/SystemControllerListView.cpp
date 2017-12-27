/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/SystemControllerListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all system controllers available to be configured. This view must communicate
  * with the SystemControllerDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/system_controller/src/StdAfx.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerListView.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerComponent.h"
#include "app/configuration/config_plugins/system_controller/src/resource.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(SystemControllerListView, CListView)


    SystemControllerListView::SystemControllerListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(SystemControllerComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_SYS_CONTROLLER) );
    }


    SystemControllerListView::~SystemControllerListView()
    {
    }


BEGIN_MESSAGE_MAP(SystemControllerListView, AbstractListView)
	//{{AFX_MSG_MAP(SystemControllerListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void SystemControllerListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // SystemControllerListView diagnostics

    #ifdef _DEBUG
    void SystemControllerListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void SystemControllerListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

