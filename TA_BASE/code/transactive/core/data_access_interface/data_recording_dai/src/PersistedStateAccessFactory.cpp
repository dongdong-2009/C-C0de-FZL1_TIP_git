/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/PersistedStateAccessFactory.cpp $
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

#include <memory>
#include <sstream>

#include "core/data_access_interface/data_recording_dai/src/PersistedStateAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/DataRecordingDAI.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "boost/lexical_cast.hpp"

namespace TA_Base_Core
{
    PersistedStateAccessFactory* PersistedStateAccessFactory::s_singleton = NULL;
    TA_Base_Core::NonReEntrantThreadLockable PersistedStateAccessFactory::s_singletonLock;

    PersistedStateAccessFactory& PersistedStateAccessFactory::getInstance()
    {
        if ( s_singleton == NULL )
        {
            TA_Base_Core::ThreadGuard guard( s_singletonLock );
            if ( s_singleton == NULL )
            {
                s_singleton = new PersistedStateAccessFactory();
            }
        }
        return *s_singleton;
    }


    PersistedStatePtr PersistedStateAccessFactory::getPersistedState( unsigned long entityKey, unsigned long entityTypeKey ) 
    {
        unsigned long typeKey = entityTypeKey;
        if ( typeKey == 0 )
        {
            // Determine entity's type
            TA_Base_Core::EntityAccessFactory& entityAccess = TA_Base_Core::EntityAccessFactory::getInstance();
            std::auto_ptr<TA_Base_Core::IEntityData> entityData( entityAccess.getEntity( entityKey ) );
            typeKey = entityData->getTypeKey();
        }
        std::map<unsigned long, std::string>::iterator typeIter = m_typeNameMap.find( typeKey );
        TA_ASSERT( typeIter != m_typeNameMap.end(), "Could not locate entity type in map" );

        const ColumnIdMap& properties = getPropertiesForType( typeKey );
        return PersistedStatePtr( new PersistedState( entityKey, typeIter->second, properties ) );
    }


    std::map<unsigned long, PersistedStatePtr> PersistedStateAccessFactory::getPersistedStatesOfType( unsigned long typeKey )
    {
        FUNCTION_ENTRY( "getPersistedStatesOfType" );

        FUNCTION_EXIT;
        return getPersistedStatesOfTypeWhere( typeKey );
    }
    
    std::map<unsigned long, PersistedStatePtr> PersistedStateAccessFactory::getPersistedStatesOfTypeAtLocation( unsigned long typeKey, unsigned long locationKey )
    {
        FUNCTION_ENTRY( "getPersistedStatesOfTypeAtLocation" );
        std::stringstream whereSql;
        if ( locationKey > 0 )
        {
            whereSql << "WHERE " << DataRecordingDAI::FIELD_ENTITYKEY << " IN ( SELECT PKEY FROM entity WHERE LOCATIONKEY=" << locationKey << " AND DELETED=0 )";
        }

        FUNCTION_EXIT;
        return getPersistedStatesOfTypeWhere( typeKey, whereSql.str() );
    }
    
    
    std::map<unsigned long, PersistedStatePtr> PersistedStateAccessFactory::getPersistedStatesOfTypeForAgent( unsigned long typeKey, unsigned long agentKey )
    {
        FUNCTION_ENTRY( "getPersistedStatesOfTypeForAgent" );
        std::stringstream whereSql;
        if ( agentKey > 0 )
        {
            whereSql << "WHERE " << DataRecordingDAI::FIELD_ENTITYKEY << " IN ( SELECT PKEY FROM entity WHERE AGENTKEY=" << agentKey << " AND DELETED=0 )";
        }

        FUNCTION_EXIT;
        return getPersistedStatesOfTypeWhere( typeKey, whereSql.str() );
    }


    std::map<unsigned long, PersistedStatePtr> PersistedStateAccessFactory::getPersistedStatesOfTypeWhere( unsigned long typeKey, const std::string& whereSql )
    {
        FUNCTION_ENTRY( "getPersistedStatesOfTypeWhere" );

        const ColumnIdMap& properties = getPropertiesForType( typeKey );

        std::map<unsigned long, PersistedStatePtr> persistedStates;

        if ( ! properties.empty() )
        {
            //  
            // Construct SQL statement for retrieving values
            //
            std::map<unsigned long, std::string>::iterator typeIter = m_typeNameMap.find( typeKey );
            TA_ASSERT( typeIter != m_typeNameMap.end(), "Could not locate entity type in map" );

            std::vector<std::string> columnNames;
            std::stringstream sql;
           /* sql << "SELECT TO_CHAR(" << DataRecordingDAI::FIELD_SOURCETIME << ",'YYYYMMDDHH24MISS'),"
                <<  DataRecordingDAI::FIELD_ENTITYKEY << ","
                <<  "TO_CHAR(" << DataRecordingDAI::FIELD_UPDATETIME << ",'YYYYMMDDHH24MISS'),"
                <<  DataRecordingDAI::FIELD_QUALITY;*/
            
            columnNames.push_back( DataRecordingDAI::FIELD_SOURCETIME );
            columnNames.push_back( DataRecordingDAI::FIELD_ENTITYKEY );
            columnNames.push_back( DataRecordingDAI::FIELD_UPDATETIME );
            columnNames.push_back( DataRecordingDAI::FIELD_QUALITY );

            ColumnIdMap::const_iterator propertyIter = properties.begin();
            for( ; propertyIter != properties.end(); ++propertyIter )
            {
                sql << "," << propertyIter->second;
                columnNames.push_back( propertyIter->second );
            }

			std::string strTableName(DataRecordingDAI::getReadTableName( typeIter->second ));
			// make sure the table name is in lowercase
			const char* pszTableName = strlwr(const_cast<char*>(strTableName.c_str()));

            /*sql << " FROM " << DataRecordingDAI::getReadTableName( typeIter->second );
            if ( false == whereSql.empty() )
            {
                sql << " " << whereSql;
            }*/

            TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_live_property_Oracle_SELECT_27551,sql.str(),
//			std::string strSql  = databaseConnection->prepareSQLStatement(DR_LIVE_PROPERTY_SELECT_27551,sql.str(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_LIVE_PROPERTY_SELECT_27551,sql.str(),
				pszTableName, whereSql);
            TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );			
            if ( data != NULL )
            {
                do
                {
                    for( unsigned int i = 0; i < data->getNumRows(); ++i )
                    {
                        try
                        {
                            PersistedState::ValueMapPtr propertyValueMap( new std::map<std::string, std::string>() );

                            unsigned long entityKey = boost::lexical_cast<unsigned long>( 
                                                            data->getStringData( i, DataRecordingDAI::FIELD_ENTITYKEY ) );

                            time_t sourceTime = data->getStringData( i, DataRecordingDAI::FIELD_SOURCETIME ).empty() ? 
                                                0 : data->getDateData( i, DataRecordingDAI::FIELD_SOURCETIME );

                            time_t updateTime = data->getStringData( i, DataRecordingDAI::FIELD_UPDATETIME ).empty() ?
                                                0 : data->getDateData( i, DataRecordingDAI::FIELD_UPDATETIME );

                            std::string tempVal = data->getStringData( i, DataRecordingDAI::FIELD_QUALITY );
                            int quality = tempVal.empty() ? 0 : boost::lexical_cast<int>( tempVal );

                            propertyIter = properties.begin();
                            for( ; propertyIter != properties.end(); ++propertyIter )
                            {
                                std::string propertyValue = data->getStringData( i, propertyIter->second );
                                propertyValueMap->insert( 
                                    std::map<std::string, std::string>::value_type( propertyIter->first, propertyValue ) );
                            }
                    
                            PersistedStatePtr state( new PersistedState( entityKey, typeIter->second, properties, propertyValueMap, 
                                                                         sourceTime, updateTime, quality ) );
                            
                            persistedStates.insert( std::map<unsigned long, PersistedStatePtr>::value_type( entityKey, state ) );
                        }
                        catch( const boost::bad_lexical_cast& )
                        {
                            std::stringstream errorMessage;
                            errorMessage << "Error converting Quality('" 
                                         << data->getStringData( 0,  DataRecordingDAI::FIELD_QUALITY ) << "')"
                                         << " for entity " << data->getStringData( i, DataRecordingDAI::FIELD_ENTITYKEY );
                            TA_THROW( DataException( errorMessage.str().c_str(), DataException::INVALID_VALUE, sql.str() ) );
                        }
                    }
                    delete data;
                    data = NULL;
                }
                while( databaseConnection->moreData( data ) );
            }
        }
                            
        FUNCTION_EXIT;
        return persistedStates;
    }


    const PersistedStateAccessFactory::ColumnIdMap& PersistedStateAccessFactory::getPropertiesForType( unsigned long typeKey )
    {
        TypePropertiesMap::const_iterator iter = m_propertiesMap.find( typeKey );
        if ( iter != m_propertiesMap.end() )
        {
            return iter->second;
        }
        else
        {
            std::vector<ILivePropertyDataPtr> properties = LivePropertyAccessFactory::getInstance().getLivePropertiesByType( typeKey );
            std::vector<ILivePropertyDataPtr>::iterator propertyIter = properties.begin();

            const TypePropertiesMap::iterator& newMapEntry = m_propertiesMap.insert( 
                                TypePropertiesMap::value_type( typeKey, ColumnIdMap() ) ).first;

            for( ; propertyIter != properties.end(); ++propertyIter )
            {
                ILivePropertyDataPtr& property = *propertyIter;
                TA_ASSERT( property->getTypeKey() == typeKey, "LiveProperty has incorrect type key" );

                newMapEntry->second.insert( ColumnIdMap::value_type( property->getIdentifier(), property->getColumn() ) );
            }


            return newMapEntry->second;
        }
    }
                

    PersistedStateAccessFactory::PersistedStateAccessFactory()
		: m_typeNameMap(TA_Base_Core::EntityAccessFactory::getInstance().getEntityTypes())
    {
        
    }


    PersistedStateAccessFactory::~PersistedStateAccessFactory()
    {
        // Nothing
    }
}
        
