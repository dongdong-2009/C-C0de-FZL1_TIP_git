/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/ActionCreateIncidentReport.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class launches the Incident Manager with parameters that specify
  * the alarms to populate the report with.
  */

#if !defined(_ACTION_CREATE_INCIDENT_REPORT_H_)
#define _ACTION_CREATE_INCIDENT_REPORT_H_

#include "app/event/event_viewer/EventListComponent/src/IEventAction.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"
#include "app/event/event_viewer/src/resource.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ActionCreateIncidentReport : public IEventAction
{
    // Construction
    public:
	    ActionCreateIncidentReport();   // standard constructor
		virtual ~ActionCreateIncidentReport();

        virtual unsigned int getActionId();
        virtual void executeAction( Filter* filter );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_ACTION_CREATE_INCIDENT_REPORT_H_)
