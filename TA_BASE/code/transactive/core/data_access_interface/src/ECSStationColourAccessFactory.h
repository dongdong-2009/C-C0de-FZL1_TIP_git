/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ECSStationColourAccessFactory.h $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * ECSStationColourAccessFactory is a singleton that is used to retrieve AlarmStation objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


#ifndef ECS_STATION_COLOUR_ACCESS_FACTORY_H
#define ECS_STATION_COLOUR_ACCESS_FACTORY_H

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IECSStationColourData.h"

namespace TA_Base_Core
{
	class ECSStationColourAccessFactory
	{

		public:
            virtual ~ECSStationColourAccessFactory() {};

			/**
			 * getInstance
			 *
			 * Creates and returns an instance of this object.
			 *
			 * @return A reference to an instance of an AlarmTypeAccessFactory object.
			 */
			static ECSStationColourAccessFactory& getInstance();

			
			/**
			 * getStationByKey
			 *
			 * Returns the Alarm Station data associated with the specified key as a pointer
			 * to an object conforming to the IAlarmStationData interface.
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
			IECSStationColourData* getStationByKey(const unsigned long key, const bool readWrite = false);

			/**
			 * getAllAlarmSeverities
			 *
			 * Retrieves all specified severities and returns the associated data as a vector of  
			 * objects conforming to the IAlarmStationData interface.
			 *
			 * @return A vector of pointers to objects conforming to the IAlarmStationData interface.
			 * N.B.: It is the responsibility of the *client* to delete the returned IAlarmStationData 
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
			std::vector<IECSStationColourData*> getAllECSStationColours(const bool readWrite = false);


		private:
            ECSStationColourAccessFactory() {};
			ECSStationColourAccessFactory( const ECSStationColourAccessFactory& theECSStationColourAccessFactory);
			ECSStationColourAccessFactory& operator=(const ECSStationColourAccessFactory &);
			
			void getECSStationColourBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IECSStationColourData*>& ecsStationColours, const bool readWrite);


            static ECSStationColourAccessFactory* m_instance;

	};
} // closes TA_Base_Core

#endif // ECS_STATION_COLOUR_ACCESS_FACTORY_H
