/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_severity/src/AlarmSeverityFrameworkView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of alarm severities
  * and the right side as a view for the user to configure alarm severity details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_severity/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_severity/src/AlarmSeverityFrameworkView.h"

#include "app/configuration/config_plugins/alarm_severity/src/AlarmSeverityListView.h"
#include "app/configuration/config_plugins/alarm_severity/src/AlarmSeverityDetailView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(AlarmSeverityFrameworkView, CFormView)


    AlarmSeverityFrameworkView::AlarmSeverityFrameworkView()
    {
    }


    AlarmSeverityFrameworkView::~AlarmSeverityFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(AlarmSeverityFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(AlarmSeverityFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* AlarmSeverityFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(AlarmSeverityDetailView);
    }


    CRuntimeClass* AlarmSeverityFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(AlarmSeverityListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // AlarmSeverityFrameworkView diagnostics

    #ifdef _DEBUG
    void AlarmSeverityFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void AlarmSeverityFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}


