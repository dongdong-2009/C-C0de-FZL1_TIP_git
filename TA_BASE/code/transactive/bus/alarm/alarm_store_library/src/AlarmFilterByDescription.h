/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByDescription.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByDescription.h
//  Implementation of the Class AlarmFilterByDescription
//  Created on:      26-Aug-2004
///////////////////////////////////////////////////////////

#if !defined(AlarmFilterByDescription__INCLUDED_)
#define AlarmFilterByDescription__INCLUDED_

#include "IAlarmFilter.h"
#include <string>

namespace TA_Base_Bus
{
	/**
	 * This class provides the filtering of an alarm based on asset name
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:04 PM
	 */
	class AlarmFilterByDescription : public IAlarmFilter 
	{

		public:
			AlarmFilterByDescription(EComparitor compareOp, const std::string &compareDescription);
			virtual ~AlarmFilterByDescription();

		protected:
			/**
			 * Check if the specified alarm meets the filter requirements
			 * @param alarm
			 * 
			 */
			virtual bool includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);
			virtual bool includeAlarm(const TA_Base_Core::AlarmItem* alarm);
			virtual bool includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm); //++Noel

			// Comparison operator used to check if the filter is the same as 
			// another.
			virtual bool operator == (const IAlarmFilter&) const;
		private:
			AlarmFilterByDescription();
			AlarmFilterByDescription(const AlarmFilterByDescription& filter);
			AlarmFilterByDescription& operator = (const AlarmFilterByDescription& filter);
		private:
			std::string      m_compareDescription;


	};
};

#endif // !defined(AlarmFilterByDescription__INCLUDED_)
