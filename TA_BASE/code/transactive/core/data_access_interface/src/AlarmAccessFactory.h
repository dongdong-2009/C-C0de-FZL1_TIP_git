/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmAccessFactory.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * AlarmAccessFactory is a singleton that is used to retrieve AlarmData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


#if !defined(AlarmAccessFactory_E17A1C95_63D1_4d61_A546_CCF8D0E8A633__INCLUDED_)
#define AlarmAccessFactory_E17A1C95_63D1_4d61_A546_CCF8D0E8A633__INCLUDED_

#include <vector>
#include "core/data_access_interface/src/IDatabase.h"


namespace TA_Base_Core
{

    // Forward delaration to help the compiler avoid unnecessary compilation
    class IAlarmData;
	class IDatabase;

    class AlarmAccessFactory
    {

    public:
        virtual ~AlarmAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an AlarmAccessFactory object.
         */
        static AlarmAccessFactory& getInstance();

		/**
		 * freeInstance
		 *
		 * deletes the object instance of this class.
		 *
		 * @return none.
		 */
		static void freeInstance();

        /**
         * getAlarm
         *
         * Returns the Alarm associated with the specified key as a pointer
         * to an object conforming to the IAlarmData interface.
         *
         * @param key The database key to the Alarm to retrieve
         *
         * @return A pointer to an object conforming to the IAlarmData interface.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no Alarm 
         *            matching the supplied key, or if there is more than one Alarm 
         *            matching the supplied key. 
         */
        IAlarmData* getAlarm(const std::string& key);
        
        /**
         * getActiveAlarms
         *
         * Returns all active alarms (alarms that are either open or unacknowledged) as a vector of pointers
         * to objects conforming to the IAlarmData interface.
         *
         * @return A vector of pointers to objects conforming to the IAlarmData interface. If
         * no active alarms exist, vector.size() will be 0.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
        std::vector<IAlarmData*> getActiveAlarms();
		std::vector<IAlarmData*> getActiveAlarms(unsigned long maxCount);
  
        /**
         * getActiveAlarmsForLocation
         *
         * Returns all active alarms (alarms that are either open or unacknowledged) for a given location.
         * Alarm are returned as a vector of pointers to objects conforming to the IAlarmData interface.
         *
         * @param The key of the location for which alarms should be retrieved
         *
         * @return A vector of pointers to objects conforming to the IAlarmData interface. If
         * no active alarms exist, vector.size() will be 0.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException raised if the supplied location key is invalid
         */
        std::vector<IAlarmData*> getActiveAlarmsForLocation(unsigned long p_locationKey);
		std::vector<IAlarmData*> getActiveAlarmsForLocation(unsigned long p_locationKey, unsigned long maxCount);

        /**
         * createAlarm
         *
         * Returns a pointer to a new alarm instance.
         *
         * @return A pointer to an object conforming to the IAlarmData interface
         * that has not been initialised.
         */
        IAlarmData* createAlarm();

		/**
		 * getDBActiveAlarmCount
		 *
		 * Returns the quantity of active alarms in the DB
		 *
		 * @return unsigned int: Qty of active alarms
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
		 */
		unsigned int getDBActiveAlarmCount();

		/**
		 * getDBActiveAlarmCountForLocation
		 *
		 * Returns the quantity of active alarms in the DB
		 *
         * @param The key of the location for which alarms should be retrieved
		 * @return unsigned int: Qty of active alarms
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException raised if the supplied location key is invalid
		 */
		unsigned int getDBActiveAlarmCountForLocation(unsigned long locationKey);

    private:

        /**
          * runQueryForAlarms
          *
          * This method takes an sql query and returns a vector of IAlarmData pointers. This
          * is essentially here to avoid dupilcate code in the getActiveAlarms and 
          * getActiveAlarmsForLocation methods.
          *
          * @param The query to run
          *
          * @return A vector of pointers to IAlarmData objects
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute the database query
          *
          * @exception DataException raised if the supplied location key is invalid
          */
        void runQueryForAlarms(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAlarmData*>& alarms);

        AlarmAccessFactory() {};
        AlarmAccessFactory( const AlarmAccessFactory& theAlarmAccessFactory);
        AlarmAccessFactory& operator=(const AlarmAccessFactory &);
    
        static AlarmAccessFactory* m_instance;
    };
} // closes TA_Base_Core

#endif // !defined(AlarmAccessFactory_E17A1C95_63D1_4d61_A546_CCF8D0E8A633__INCLUDED_)
