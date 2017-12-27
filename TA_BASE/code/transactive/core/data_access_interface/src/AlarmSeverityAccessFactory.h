/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmSeverityAccessFactory.h $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * AlarmSeverityAccessFactory is a singleton that is used to retrieve AlarmSeverity objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


#ifndef ALARM_SEVERITY_ACCESS_FACTORY_H
#define ALARM_SEVERITY_ACCESS_FACTORY_H

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"

namespace TA_Base_Core
{
	class AlarmSeverityAccessFactory
	{

		public:
            virtual ~AlarmSeverityAccessFactory() {};

			/**
			 * getInstance
			 *
			 * Creates and returns an instance of this object.
			 *
			 * @return A reference to an instance of an AlarmTypeAccessFactory object.
			 */
			static AlarmSeverityAccessFactory& getInstance();

			/**
			 * freeInstance
			 *
			 * deletes the object instance of this class.
			 *
			 * @return none.
			 */
			static void freeInstance();

			/**
			 * getSeverityByKey
			 *
			 * Returns the Alarm severity data associated with the specified key as a pointer
			 * to an object conforming to the IAlarmSeverityData interface.
			 *
			 * @param key The database key to the Alarm type to retrieve
			 *
			 * @return A pointer to an object conforming to the IAlarmTypeData interface.
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException A DataException will be thrown if there is no Alarm type
			 *            matching the supplied key, or if there is more than one Alarm type
			 *            matching the supplied key. 
			 */
			IAlarmSeverityData* getSeverityByKey(const unsigned long key, const bool readWrite = false);

			/**
			 * getAllAlarmSeverities
			 *
			 * Retrieves all specified severities and returns the associated data as a vector of  
			 * objects conforming to the IAlarmSeverityData interface.
			 *
			 * @return A vector of pointers to objects conforming to the IAlarmSeverityData interface.
			 * N.B.: It is the responsibility of the *client* to delete the returned IAlarmSeverityData 
			 * objects
			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException 
			 */
			std::vector<IAlarmSeverityData*> getAllAlarmSeverities(const bool readWrite = false);

		private:
			void getAlarmSeverityBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAlarmSeverityData*>& alarmSeverities, const bool readWrite);


		private:
            AlarmSeverityAccessFactory() {};
			AlarmSeverityAccessFactory( const AlarmSeverityAccessFactory& theAlarmSeverityAccessFactory);
			AlarmSeverityAccessFactory& operator=(const AlarmSeverityAccessFactory &);

            static AlarmSeverityAccessFactory* m_instance;

	};
} // closes TA_Base_Core

#endif // ALARM_SEVERITY_ACCESS_FACTORY_H
