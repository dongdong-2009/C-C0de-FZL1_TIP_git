/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourFrameworkView.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of event colours
  * and the right side as a view for the user to configure event colour details.
  */

// EventColourFrameworkView.cpp: implementation of the EventColourFrameworkView class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/event_colour/src/StdAfx.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourFrameworkView.h"

#include "app/configuration/config_plugins/event_colour/src/EventColourListView.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourDetailView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(EventColourFrameworkView, CFormView)

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	EventColourFrameworkView::EventColourFrameworkView()
	{
	}

	EventColourFrameworkView::~EventColourFrameworkView()
	{
	}

	BEGIN_MESSAGE_MAP(EventColourFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(EventColourFrameworkView)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

    
    CRuntimeClass* EventColourFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(EventColourDetailView);
    }


    CRuntimeClass* EventColourFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(EventColourListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // EventColourFrameworkView diagnostics

    #ifdef _DEBUG
    void EventColourFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void EventColourFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

} //close namespace
