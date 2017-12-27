/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/DatabaseCache.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class holds cached database information to save returning to the database for each retrieval.
  */

#ifndef DATABASE_CACHE_WED_11_02_2004_H
#define DATABASE_CACHE_WED_11_02_2004_H

#include <map>
#include <string>

#include "core/data_access_interface/src/IAlarmSeverityData.h"

#include "core/configuration_updates/src/IOnlineUpdatable.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

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
    class DatabaseCache : public virtual TA_Base_Core::IOnlineUpdatable
    {
    public:
        /**
         * Constructor
         */
        DatabaseCache();


        /**
         * Destructor
         */
        virtual ~DatabaseCache();


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
         * Retrieves the operator name for the specified key
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
         * getAlarmSeverityColour
         *
         * Retrieves the alarm severity colour for the specified key
         *
         * @param unsigned long - The pkey of the alarm severity
         * @param EColourType - The type of colour required
         *
         * @return COLORREF - The colour to use to display the alarm severity
         */
        COLORREF getAlarmSeverityColour(unsigned long key, TA_Base_Core::IAlarmSeverityData::EColourType type);


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
        DatabaseCache(const DatabaseCache&);


        /**
         * operator=
         * Hidden as it is not needed
         */
        DatabaseCache& operator=(const DatabaseCache&);
        

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
    };
}

#endif // DATABASE_CACHE_WED_11_02_2004_H
