#ifndef PERSISTED_STATE_ACCESS_FACTORY_H
#define PERSISTED_STATE_ACCESS_FACTORY_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/PersistedStateAccessFactory.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Creates PersistedState objects, providing read access
 * to state data.
 *
 */

#include <map>
#include <string>

#include "core/data_access_interface/data_recording_dai/src/PersistedState.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{

    class PersistedStateAccessFactory
    {
    public:

        /**
         * getInstance
         *
         * @return the singleton instance of PersistedStateAccessFactory
         *
         */
        static PersistedStateAccessFactory& getInstance();


        /**
         * getPersistedState
         *
         * Retrieves the persisted state of a specific entity
         *
         * @param   entityKey       pkey of entity to get state for
         * @param   entityTypeKey   pkey of entity's type. If this is
         *                          0, then PersistedStateAccessFactory will
         *                          look up the entity's type in the database.
         *
         * @return  a PersistedState object for the specified entity
         *
         */
        PersistedStatePtr getPersistedState( unsigned long entityKey, unsigned long entityTypeKey = 0 );


        /**
         * getPersistedStatesOfType
         *
         * Retrieves all persisted states for a specific entity type
         *
         * @param   typeKey         pkey of entity type to get states for
         *
         * @return map of Persisted State objects, indexed by entity key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) Invalid data retrieved from database
         *
         */
        std::map<unsigned long, PersistedStatePtr> getPersistedStatesOfType( unsigned long typeKey );


        /**
         * getPersistedStatesOfTypeAtLocation
         *
         * Retrieves all persisted states for a specific entity type
         *
         * @param   typeKey         pkey of entity type to get states for
         * @param   locationKey     additional specifier of entity location 
         *                          to get states for
         *
         * @return map of Persisted State objects, indexed by entity key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) Invalid data retrieved from database
         *
         */
        std::map<unsigned long, PersistedStatePtr> getPersistedStatesOfTypeAtLocation( unsigned long typeKey, unsigned long locationKey );

        
        /**
         * getPersistedStatesOfTypeForAgent
         *
         * Retrieves all persisted states for a specific entity type owned by an agent
         *
         * @param   typeKey         pkey of entity type to get states for
         * @param   agentKey        additional specifier of agent to get states for
         *
         * @return map of Persisted State objects, indexed by entity key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) Invalid data retrieved from database
         *
         */
        std::map<unsigned long, PersistedStatePtr> getPersistedStatesOfTypeForAgent( unsigned long typeKey, unsigned long agentKey );


    private:

        /**
         * Singleton class - private constructor/destructor
         *
         */
        PersistedStateAccessFactory();

        ~PersistedStateAccessFactory();


        /**
         * Prevent copying
         */
        PersistedStateAccessFactory( const PersistedStateAccessFactory& copyMe );
        PersistedStateAccessFactory& operator=( const PersistedStateAccessFactory& copyMe );


        /** 
          * getPersistedStatesOfTypeWhere
          *
          * Common implementation for the public methods above.
          *
          * @param typeKey the type of persistence
          * @param whereSql for conditional retrieval of data
          *
          * @return map of Persisted State objects, indexed by entity key
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possible
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) Invalid data retrieved from database
          *
          */
        std::map<unsigned long, PersistedStatePtr> getPersistedStatesOfTypeWhere( unsigned long typeKey, const std::string& whereSql = "" );


        typedef std::map<std::string, std::string> ColumnIdMap;
        typedef std::map<unsigned long, ColumnIdMap> TypePropertiesMap;

        /**
         * getPropertiesForType
         *
         * @param  typeKey  pkey for an entity type
         * @return the column name <-> property id map for the specified type
         *
         */
        const ColumnIdMap& getPropertiesForType( unsigned long typeKey );

        TypePropertiesMap                               m_propertiesMap;
        std::map<unsigned long, std::string>            m_typeNameMap;

        static PersistedStateAccessFactory*             s_singleton;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

    };
}
        
#endif // PERSISTED_STATE_ACCESS_FACTORY_H
