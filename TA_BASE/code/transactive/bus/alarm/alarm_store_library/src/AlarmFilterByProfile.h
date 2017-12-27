/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByProfile.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByProfile.h
//  Implementation of the Class AlarmFilterByProfile
//  Created on:      24-Jan-2004 04:19:27 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmFilterByProfile_089D57C3_4F68_4bd9_8E05_B2FCD0E2E83F__INCLUDED_)
#define AlarmFilterByProfile_089D57C3_4F68_4bd9_8E05_B2FCD0E2E83F__INCLUDED_

#include "IAlarmFilter.h"
#include <string>	
#include <set>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Bus
{
    class RightsLibrary; // Forward declaration

	// yanrong++: reduce access to rights agent
	/**
	 * This class used to cache locationKey and subsystemKey pair,
	 * only use this pair to check right, use no longer resource key (associated entity key).
	 * For the same pair only check once, and store the result with m_isVisible.
	 */
	class AlarmRightsQuery
	{
	public:
		AlarmRightsQuery(unsigned long locationKey, unsigned long subsystemKey, bool isVisible);

		bool operator==(const AlarmRightsQuery& rhs) const;
		bool operator< (const AlarmRightsQuery& rhs) const;

		unsigned long m_locationKey;
		unsigned long m_subsystemKey;
		bool m_isVisible;
	};
	// ++yanrong.

	/**
	 * This filter will only include alarms that are from a resource which is under
	 * the control of the current operator.
	 * @version 1.0
	 * @created 24-Jan-2004 04:19:27 PM
	 */
	class AlarmFilterByProfile : public IAlarmFilter 
	{
		public:
			explicit AlarmFilterByProfile(const std::string &sessionID);
			virtual ~AlarmFilterByProfile();

			/**
			 * Sets the rights library to use in deciding whether or not
			 * alarms will be visible in the list
			 * @param rightsLibrary
			 */
			void AlarmFilterByProfile::setRightsLibrary(TA_Base_Bus::RightsLibrary* rightsLibrary);
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

			// yanrong++: reduce access to rights agent. called when duty changed.
			virtual void reset(); 
			// ++yanrong.

		private:

			AlarmFilterByProfile();
			AlarmFilterByProfile(const AlarmFilterByProfile& theAlarmFilterByProfile);
			AlarmFilterByProfile& operator = (const AlarmFilterByProfile& theAlarmFilterByProfile);
		private:
			std::string				  m_sessionID;
			TA_Base_Bus::RightsLibrary*   m_rightsLibrary;

			// yanrong++: reduce access to rights agent.
			// cache loc/sub pair to reduce access to rights agent.
			bool rightsLookup(unsigned long locationKey, unsigned long subsystemKey, bool& isVisible);
			void rightsAdd(unsigned long locationKey, unsigned long subsystemKey, bool isVisible);

			typedef std::set<AlarmRightsQuery> AlarmRithsCache;
			AlarmRithsCache m_cache;
			TA_Base_Core::NonReEntrantThreadLockable m_lock;
			//bool m_enable_profile_cache;
			// ++yanrong
	};
};
#endif // !defined(AlarmFilterByProfile_089D57C3_4F68_4bd9_8E05_B2FCD0E2E83F__INCLUDED_)
