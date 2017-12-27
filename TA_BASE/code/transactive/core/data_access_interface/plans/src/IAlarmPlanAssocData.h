/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/IAlarmPlanAssocData.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * IAlarmPlanAssocData is an interface to the AlarmPlanMap table. It provides read-only
  * access for existing AlarmPlanAssociations
  */

#if !defined(IALARM_PLAN_ASSOC_DATA_H)
#define IALARM_PLAN_ASSOC_DATA_H

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IAlarmPlanAssocData : public IItem
	{
	public:
	
		/** 
		  * getAlarmType
		  *
		  * Returns the alarm type for this association
		  *
		  * @return std::string The alarm type
		  */
		virtual unsigned long getAlarmType() = 0;

		/** 
		  * getEntityKey
		  *
		  * Returns the entity key for this association
		  *
		  * @return unsigned long the entitykey
		  */
		virtual unsigned long getEntityKey() = 0;

		/** 
		  * getEntityTypeKey
		  *
		  * Returns the entity type key for this association
		  *
		  * @return unsigned long the entity type key
		  */
		virtual unsigned long getEntityTypeKey() = 0;

		/** 
		  * getPlanPath
		  *
		  * Returns the full path of the plan this association maps to.
          * The path is in the format "[/<category1>/<categoryN>]/<planName>"
		  *	
		  * @return std::string fully qualified plan path
		  */
		virtual std::string getPlanPath() = 0;

		/** 
		  * match
		  *
		  * this function will return true if the provided parameters match
		  * the association.
		  *
		  * @return bool 
		  *
		  * @param unsigned long& entityKey
		  * @param unsigned long& entityTypeKey
		  * @param std::string& alarmType
		  */
		virtual bool match( const unsigned long& entityKey, 
			const unsigned long& entityTypeKey,
			const unsigned long& alarmType) = 0;


		virtual ~IAlarmPlanAssocData() {}

	};

} // Closes TA_Base_Core

#endif // IALARM_PLAN_ASSOC_DATA_H

