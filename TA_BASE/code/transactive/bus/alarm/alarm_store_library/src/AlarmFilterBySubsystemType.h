/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmFilterBySubsystemType.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterBySubsystemType.h
//  Implementation of the Class AlarmFilterBySubsystemType
//  Created on:      26-Aug-2004
///////////////////////////////////////////////////////////

#if !defined(AlarmFilterBySubsystemType__INCLUDED_)
#define AlarmFilterBySubsystemType__INCLUDED_

#include "IAlarmFilter.h"
#include <string>

namespace TA_Base_Bus
{
	/**
	 * This class provides the filtering of an alarm based on asset name
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:04 PM
	 */
	class AlarmFilterBySubsystemType : public IAlarmFilter 
	{

		public:
			AlarmFilterBySubsystemType(EComparitor compareOp, const std::string &compareDescription);
			virtual ~AlarmFilterBySubsystemType();

		protected:
			/**
			 * Check if the specified alarm meets the filter requirements
			 * @param alarm
			 * 
			 */
			virtual bool includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);
			virtual bool includeAlarm(const TA_Base_Core::AlarmItem* alarm);
			virtual bool includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm);

			// Comparison operator used to check if the filter is the same as 
			// another.
			virtual bool operator == (const IAlarmFilter&) const;
		private:
			AlarmFilterBySubsystemType();
			AlarmFilterBySubsystemType(const AlarmFilterBySubsystemType& filter);
			AlarmFilterBySubsystemType& operator = (const AlarmFilterBySubsystemType& filter);
		private:
			std::string      m_compareSubsytemType;


	};
};

#endif // !defined(AlarmFilterBySubsystemType__INCLUDED_)

