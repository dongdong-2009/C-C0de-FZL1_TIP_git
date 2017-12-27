/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmTypeAccessFactory.h $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/21 00:07:03 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * AlarmTypeAccessFactory is a singleton that is used to retrieve AlarmTypeData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


#if !defined(AlarmTypeAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_)
#define AlarmTypeAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/ConfigAlarmType.h"

namespace TA_Base_Core
{
	class AlarmTypeAccessFactory
	{

		public:
            virtual ~AlarmTypeAccessFactory() {};

			/**
			 * getInstance
			 *
			 * Creates and returns an instance of this object.
			 *
			 * @return A reference to an instance of an AlarmTypeAccessFactory object.
			 */
			static AlarmTypeAccessFactory& getInstance();		
			
			/**
			 * freeInstance
			 *
			 * deletes the object instance of this class.
			 *
			 * @return none.
			 */
			static void freeInstance();

			/**
			 * getAlarmType
			 *
			 * Returns the Alarm type associated with the specified key as a pointer
			 * to an object conforming to the IAlarmTypeData interface.
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
            IAlarmTypeData* getAlarmTypeByKey(const std::string& key, const bool readWrite);

			/**
			 * getAlarmType
			 *
			 * Returns the Alarm type associated with the specified key as a pointer
			 * to an object conforming to the IAlarmTypeData interface.
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
            IAlarmTypeData* getAlarmTypeByKey(unsigned long key, const bool readWrite);

            /**
             * createAlarmType
             * 
             * Creates a new configuration alarm type object.
             *
             * @return The configuration alarm type object as a pointer
             *         to an IConfigAlarmType object.
             *
             * @exception DatabaseException Thrown if there is a database problem.
             * @exception DataException Thrown if there is bad data.
             */
            IConfigAlarmType* AlarmTypeAccessFactory::createAlarmType(
                const std::string name,
                const std::string description,
                unsigned long severityKey,
	            bool toBePrinted,
		        bool toBePersisted,
				bool isSystemAlarm);

            /**
             * copyAlarmType
             * 
             * Creates a new configuration AlarmType object using the AlarmType
             * passed in
             *
             * @param IConfigAlarmType* The alarm type to copy
             *
             * @return The configuration alarm type object as a pointer to an
             *         IConfigAlarmType object.
             *
             * @exception DatabaseException Thrown if there is a database problem.
             */
            IConfigAlarmType* copyAlarmType(const IConfigAlarmType* alarmTypeToCopy);


			/**
			 * getAlarmType
			 *
			 * Returns the Alarm type associated with the specified type name as a pointer
			 * to an object conforming to the IAlarmTypeData interface.
			 *
			 * @param typeName The name of the Alarm type to retrieve
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
			 *            matching the supplied name, or if there is more than one Alarm type
			 *            matching the supplied name. 
			 */
			IAlarmTypeData* getAlarmType(const std::string& typeName, const bool readWrite);

			/**
			 * getAlarmTypeByDisplayName
			 *
			 * Returns the Alarm type associated with the specified displayName as a pointer
			 * to an object conforming to the IAlarmTypeData interface.
			 *
			 * @param displayName The displayName of the Alarm type to retrieve
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
			 *            matching the supplied name, or if there is more than one Alarm type
			 *            matching the supplied name. 
			 */
			IAlarmTypeData* getAlarmTypeByDisplayName(const std::string& displayName,const bool readWrite);
			/**
 			 * getAllAlarmTypes
 			 *
 			 * Loads all the alarm types from the database, returning the objects as a vector
 			 *
			 * @param	readWrite	True if the entity is writable
			 *
 			 * @return	vector of AlarmTypeObjects
 			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException A DataException will be thrown if there is no Alarm type
			 *            matching the supplied name, or if there is more than one Alarm type
			 *            matching the supplied name. 
			 */
			std::vector< IAlarmTypeData*> getAllAlarmTypes(const bool readWrite);

            /**
 			 * getAllAlarmTypesAndAttributes
 			 *
 			 * Loads all the alarm types and their attributes using one SQL statement, returning 
             * the objects as a vector
 			 *
			 * @return	vector of AlarmTypeObjects
 			 *
			 * @exception DatabaseException A DatabaseException will be thrown if there are
			 *            any problems in communicating with the database. The possilble 
			 *            reasons are:
			 *            1) Invalid connection string (as stored in RunParams)
			 *            2) Database is not/cannot be opened
			 *            3) Error while attempting to execute a database query
			 * @exception DataException A DataException will be thrown if there is no Alarm type
			 *            matching the supplied name, or if there is more than one Alarm type
			 *            matching the supplied name. 
			 */
			std::vector< IAlarmTypeData*> getAllAlarmTypesAndAttributes();

		private:

			/**
			 * getAlarmTypeBySql
			 *
			 * Returns the Alarm type associated with the specified sql statement as a pointer
			 * to an object conforming to the IAlarmTypeData interface.
			 *
			 * @param sql The sql statement of the Alarm type to retrieve
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
			 *            matching the supplied name, or if there is more than one Alarm type
			 *            matching the supplied name. 
			 */

			void getAlarmTypeBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAlarmTypeData*>& alarmTypes, const bool readWrite);

		private:
            AlarmTypeAccessFactory() {};
			AlarmTypeAccessFactory( const AlarmTypeAccessFactory& theAlarmTypeAccessFactory);
			AlarmTypeAccessFactory& operator=(const AlarmTypeAccessFactory &);

            static AlarmTypeAccessFactory* m_instance;

	};
} // closes TA_Base_Core

#endif // !defined(AlarmTypeAccessFactory_12717F2B_FDE6_4e6f_8352_53C89BB711C4__INCLUDED_)
