/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/mms_agent/src/MMSDatabaseCache.h $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime:  2011/09/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  * 
  * This class holds cached database information to save returning to the database for each retrieval.
  * I grabbed this class out of the Alarm List Control project. It was very close to what I needed as is
  * but to avoid any issues with modifications I have copied the code. 
  * Perhaps, time allowing we should make any required modifications and build against the orginal code.
  */

#ifndef MMSDATABASE_CACHE_WED_23_09_2011_H
#define MMSDATABASE_CACHE_WED_23_09_2011_H

#include <map>
#include <string>

#pragma warning(disable:4786)

#include "core/configuration_updates/src/IOnlineUpdatable.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class ILocation;
    class IOperator;
    class IAlarmSeverityData;
    class ISubsystem;
    class IEntityData;

    class ConfigUpdateDetails;
}


namespace TA_Base_Bus
{
    class MMSDatabaseCache : public virtual TA_Base_Core::IOnlineUpdatable
    {
    public:
        /**
         * Constructor
         */
        MMSDatabaseCache();


        /**
         * Destructor
         */
        virtual ~MMSDatabaseCache();

		/**
		* getInstance
		* This class is a singleton
		*/
		static MMSDatabaseCache* getInstance();

        /**
         * processUpdate
         *
         * This method meets the IOnlineUpdatable interface requirements
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         */
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


        /**
         * getLocationName
         *
         * Retrieves the location name for the specified key
         *
         * @param unsigned long - The pkey of the location
         *
         * @return string - The location name. An empty string if the location name cannot
         *                  be found
         */
        std::string getLocationName(unsigned long key);

        
        /**
         * getOperatorName
         *
         * Retrieves the full operator name ( description technically )for the 
		 * specified key. No truncation is performed
         *
         * @param unsigned long - The pkey of the operator
         *
         * @return string - The operator name. An empty string if the operator name cannot
         *                  be found
         */
        std::string getOperatorName(unsigned long key);


        /**
         * getSubsystemName
         *
         * Retrieves the subsystem name for the specified key
         *
         * @param unsigned long - The pkey of the subsystem
         *
         * @return string - The subsystem name. An empty string if the operator name cannot
         *                  be found
         */
        std::string getSubsystemName(unsigned long key);


        /**
         * getAlarmSeverityName
         *
         * Retrieves the alarm severity name for the specified key
         *
         * @param unsigned long - The pkey of the alarm severity
         *
         * @return string - The alarm severity name. An empty string if the operator name cannot
         *                  be found
         */
        std::string getAlarmSeverityName(unsigned long key);

        /**
         * getEntity
         *
         * This retrieves an entity from the cached map or directly from the database
         *
         * @return IEntityData& - This will return an entity object for the key passed in
         *
         * @exception DataException
         * @exception DatabaseException
         */
        TA_Base_Core::IEntityData& getEntity(unsigned long key);

        /**
         * getAlarmTypeKey
         *
         * When passed an alarm type name this returns the key
         *
         * @param string - Name of an alarm type
         * 
         * @return string - The key of the alarm type. Returns an empty string if no alarm type 
         *                  could be determined.
         */
        std::string getAlarmTypeKey(std::string alarmTypeName);


    private:
       
        /**
         * Copy Constructor
         * Hidden as it is not needed
         */
        MMSDatabaseCache(const MMSDatabaseCache&);


        /**
         * operator=
         * Hidden as it is not needed
         */
        MMSDatabaseCache& operator=(const MMSDatabaseCache&);
        

        /**
         * loadLocations
         * 
         * This loads all locations from the database and stores them in the map m_locations
         */
        void loadLocations();


        /**
         * loadOperators
         * 
         * This loads all operators from the database and stores them in the map m_operators
         */
        void loadOperators();


        /**
         * loadAlarmSeverities
         * 
         * This loads all alarm severities from the database and stores them in the map m_alarmSeverities
         */
        void loadAlarmSeverities();


        /**
         * loadSubsystems
         * 
         * This loads all subsystems from the database and stores them in the map m_subsystems
         */
        void loadSubsystems();


		


        std::map<unsigned long, TA_Base_Core::ILocation*>          m_locations;
        std::map<unsigned long, TA_Base_Core::IOperator*>          m_operators;
        std::map<unsigned long, TA_Base_Core::IAlarmSeverityData*> m_alarmSeverities;
        std::map<unsigned long, TA_Base_Core::ISubsystem*>         m_subsystems;

        TA_Base_Core::NonReEntrantThreadLockable                   m_entityMapLock;
        std::map<unsigned long, TA_Base_Core::IEntityData*>        m_entities;

        TA_Base_Core::NonReEntrantThreadLockable                   m_alarmTypeMapLock;
        std::map<std::string, std::string>                    m_alarmTypes;


		

		static MMSDatabaseCache* m_instance;

		//provides a thread lock
		static TA_Base_Core::ReEntrantThreadLockable m_singletonlock;

		bool m_enableOnlineUpdatable;
    };
}

#endif // DATABASE_CACHE_WED_11_02_2004_H
