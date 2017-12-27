/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/AlarmPlanAssocData.h $
 * @author:  Cameron Rochester
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * AlarmPlanAssocData is an implementation of IAlarmPlanAssocData. It holds the data specific to 
 * alarm plan associations.
 */

#if !defined(ALARM_PLAN_ASSOC_DATA_H)
#define ALARM_PLAN_ASSOC_DATA_H

#include <string>

#include "core/data_access_interface/plans/src/IAlarmPlanAssocData.h"

namespace TA_Base_Core
{
	class AlarmPlanAssocHelper;
	

	class AlarmPlanAssocData : public IAlarmPlanAssocData
	{

	public:

		/**
		 * AlarmPlanAssocData (constructor)
		 *
		 * Construct a AlarmPlanAssocData class for the entry represented by the specified data.
		 * The AlarmPlanAssocData will be constructed with valid data.
		 *
		 * @param key The key of this Alarm Plan Association in the database
		 * @param unsigned long entityKey
		 * @param unsigned long entityTypeKey
		 * @param const std::string& alarmType
		 * @param const std::string& planPath
         * @param time_t createTime 
         * @param time_t modifyTime
		 */
		AlarmPlanAssocData( unsigned long key, 
			                unsigned long entityKey,
			                unsigned long entityTypeKey,
			                unsigned long alarmType,
			                const std::string& planPath,
                            time_t createTime,
                            time_t modifyTime);


        /**
         * Constructor
         *
         * Constructs a AlarmPlanAssocData class for the entry specified by the key. the first call
         * to the object will populate the rest of the fields
         *
		 * @param key The key of this Alarm Plan Association in the database
         */
        AlarmPlanAssocData( unsigned long key );


		virtual ~AlarmPlanAssocData();
	
		/**
         * getKey
         *
         * Returns the key for this AlarmPlanAssocData
         *
         * @return The key for this AlarmPlanAssocData as an unsigned long.
         */
		virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();
		
        
        /** 
		  * getAlarmType
		  *
		  * Returns the alarm type for this association
		  *
		  * @return std::string The alarm type
		  */
		virtual unsigned long getAlarmType();

		/** 
		  * getEntityKey
		  *
		  * Returns the entity key for this association
		  *
		  * @return unsigned long the entitykey
		  */
		virtual unsigned long getEntityKey();

		/** 
		  * getEntityTypeKey
		  *
		  * Returns the entity type key for this association
		  *
		  * @return unsigned long the entity type key
		  */
		virtual unsigned long getEntityTypeKey();


		/** 
		  * getPlanPath
		  *
		  * Returns the full path of the plan this association maps to.
          * The path is in the format "[/<category1>/<categoryN>]/<planName>"
		  *	
		  * @return std::string fully qualified plan path
		  */
		virtual std::string getPlanPath();


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
			const unsigned long& alarmType);

		/**
         * invalidate
         *
         * Mark the data contained by this Alarm type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

	private:
		AlarmPlanAssocData& operator=(const AlarmPlanAssocData &);
		AlarmPlanAssocData( const AlarmPlanAssocData& theAlarmPlanAssoc);

		AlarmPlanAssocHelper* m_helper;
	};
} // closes TA_Base_Core

#endif // !defined(ALARM_PLAN_ASSOC_DATA_H)
