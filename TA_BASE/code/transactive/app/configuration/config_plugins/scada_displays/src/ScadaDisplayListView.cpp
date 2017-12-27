/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_displays/src/ScadaDisplayListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all Scada displays available to be configured. This view must communicate
  * with the ScadaDisplayDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada_displays/src/StdAfx.h"
#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayListView.h"
#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayComponent.h"
#include "app/configuration/config_plugins/scada_displays/src/resource.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(ScadaDisplayListView, CListView)


    ScadaDisplayListView::ScadaDisplayListView()
    {
        m_componentIcon = LoadIcon(GetModuleHandle(ScadaDisplayComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_SCADA_DISPLAY) );
    }


    ScadaDisplayListView::~ScadaDisplayListView()
    {
    }


BEGIN_MESSAGE_MAP(ScadaDisplayListView, AbstractListView)
	//{{AFX_MSG_MAP(ScadaDisplayListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ScadaDisplayListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // ScadaDisplayListView diagnostics

    #ifdef _DEBUG
    void ScadaDisplayListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void ScadaDisplayListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

