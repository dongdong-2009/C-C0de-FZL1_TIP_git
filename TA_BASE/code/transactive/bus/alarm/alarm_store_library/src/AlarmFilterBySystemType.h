/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmFilterBySystemType.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterBySystemType.h
//  Implementation of the Class AlarmFilterBySystemType
//  Created on:      26-Aug-2004
///////////////////////////////////////////////////////////

#if !defined(AlarmFilterBySystemType__INCLUDED_)
#define AlarmFilterBySystemType__INCLUDED_

#include "IAlarmFilter.h"
#include <string>

namespace TA_Base_Bus
{
	/**
	 * This class provides the filtering of an alarm based on asset name
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:04 PM
	 */
	class AlarmFilterBySystemType : public IAlarmFilter 
	{

		public:
			AlarmFilterBySystemType(EComparitor compareOp, const std::string &compareDescription);
			virtual ~AlarmFilterBySystemType();

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
			AlarmFilterBySystemType();
			AlarmFilterBySystemType(const AlarmFilterBySystemType& filter);
			AlarmFilterBySystemType& operator = (const AlarmFilterBySystemType& filter);
		private:
			std::string      m_compareSystemType;


	};
};

#endif // !defined(AlarmFilterBySystemType__INCLUDED_)

