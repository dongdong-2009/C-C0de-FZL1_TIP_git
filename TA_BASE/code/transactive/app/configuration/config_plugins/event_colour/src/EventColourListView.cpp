/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourListView.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all event colours available to be configured. This view must communicate
  * with the EventColourDetailView as this will show the details of the item selected in this view.
  */

// EventColourListView.cpp: implementation of the EventColourListView class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)

#include "app/configuration/config_plugins/event_colour/src/StdAfx.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourListView.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourComponent.h"
#include "app/configuration/config_plugins/event_colour/src/resource.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(EventColourListView, CListView)

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	EventColourListView::EventColourListView()
	{
        m_componentIcon = LoadIcon(GetModuleHandle(EventColourComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_MESSAGE_TYPE) );
	}

	EventColourListView::~EventColourListView()
	{
	}

	BEGIN_MESSAGE_MAP(EventColourListView, AbstractListView)
	//{{AFX_MSG_MAP(EventColourListView)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


    void EventColourListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // EventColourListView diagnostics

    #ifdef _DEBUG
    void EventColourListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void EventColourListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

} //close namespace