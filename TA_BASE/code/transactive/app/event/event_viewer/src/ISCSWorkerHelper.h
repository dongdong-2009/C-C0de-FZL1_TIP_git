/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSWorkerHelper.h $
  * @author xiangmei.lu
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/10/19 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ISCSWorkerHelper is a helper class for ISCSRealTimeWorker and ISCSHistoricalWorker
  *
  */

#pragma once

#include "FilterCriteria.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"


namespace TA_Base_App
{
	class ISCSWorkerHelper
	{
	public:
		ISCSWorkerHelper(void);
		~ISCSWorkerHelper(void);
		static void populatePublicFilter(FilterCriteria filterCriteria, 
			TA_Base_Core::CombinedEventAccessFactory::EventFilter& filter );
		static bool iscsWillNeverMeetFilter(FilterCriteria& filter);
	};
};

