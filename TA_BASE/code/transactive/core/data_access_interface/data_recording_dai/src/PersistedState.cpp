/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/PersistedState.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Provides READ-ONLY access to the Data Recording persisted
 * state tables.
 *
 */

#include <vector>
#include <memory>
#include <sstream>

#include "core/data_access_interface/data_recording_dai/src/PersistedState.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    PersistedState::PersistedState( unsigned long entityKey, const std::string& typeName, 
                                    const std::map<std::string, std::string>& columnMap )
    : m_columnMap( columnMap ), 
	  m_entityKey( entityKey ), 
	  m_valid( false ), 
	  m_typeName( typeName ),
	  m_sourceTime(time(0)),
	  m_updateTime(time(0)),
	  m_quality(0),
	  m_valueMap(ValueMapPtr( new std::map<std::string,std::string>() ))
    {

    }
    

    PersistedState::PersistedState( unsigned long entityKey, const std::string& typeName,   
                                    const std::map<std::string, std::string>& columnMap,
                                    ValueMapPtr valueMap, time_t sourceTime, 
                                    time_t updateTime, int quality )
    : m_columnMap( columnMap ), m_valueMap( valueMap ), m_entityKey( entityKey ), m_sourceTime( sourceTime ),
      m_updateTime( updateTime ), m_quality( quality ), m_valid( true ), m_typeName( typeName )
    {
        // Nothing else
    }


    PersistedState::~PersistedState()
    {
        // Nothing
    }

    unsigned long PersistedState::getEntityKey() const
    {
        return m_entityKey;
    }


    time_t PersistedState::getSourceTime()
    {
        testIsValid();
        return m_sourceTime;
    }


    time_t PersistedState::getUpdateTime()
    {
        testIsValid();
        return m_updateTime;
    }


    int PersistedState::getQuality()
    {
        testIsValid();
        return m_quality;
    }


    void PersistedState::invalidate()
    {
        m_valid = false;
    }


    void PersistedState::reload()
    {
        FUNCTION_ENTRY( "reload" );

        //
        // Construct the SQL statement for retrieving
        //
        std::vector<std::string> columnNames;
        std::stringstream sql;

        /*sql << "SELECT TO_CHAR(" << DataRecordingDAI::FIELD_SOURCETIME << ",'YYYYMMDDHH24MISS'),"
            <<  "TO_CHAR(" << DataRecordingDAI::FIELD_UPDATETIME << ",'YYYYMMDDHH24MISS'),"
            <<  DataRecordingDAI::FIELD_QUALITY;*/

        columnNames.push_back( DataRecordingDAI::FIELD_SOURCETIME );
        columnNames.push_back( DataRecordingDAI::FIELD_UPDATETIME );
        columnNames.push_back( DataRecordingDAI::FIELD_QUALITY );

        std::map<std::string, std::string>::const_iterator columnIter = m_columnMap.begin();
        for( ; columnIter != m_columnMap.end(); ++columnIter )
        {
            sql << "," << columnIter->second;
            columnNames.push_back( columnIter->second );
        }

		std::string strTableName(DataRecordingDAI::getReadTableName( m_typeName ));
		// make sure the table name is in lowercase
		const char *pszTableName = strlwr(const_cast<char*>(strTableName.c_str()));

       /* sql << " FROM " << DataRecordingDAI::getReadTableName( m_typeName )
            << " WHERE " << DataRecordingDAI::FIELD_ENTITYKEY << " = " << m_entityKey;*/

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), DR_DataNode_STATE_Oracle_SELECT_27051, sql.str(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(DR_DATANODE_STATE_SELECT_27051, sql.str(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_DATANODE_STATE_SELECT_27051, sql.str(),
			pszTableName, m_entityKey);
        std::auto_ptr<TA_Base_Core::IData> data( databaseConnection->executeQuery( strSql, columnNames ) );		
        if ( data.get() != NULL )
        {
            if ( data->getNumRows() == 0 )
            {
                std::stringstream errorMessage;
                errorMessage << "No persisted data found for " << m_entityKey;
                TA_THROW( DataException( errorMessage.str().c_str(), DataException::NO_VALUE, sql.str() ) );
            }
            else if ( data->getNumRows() > 1 )
            {
                std::stringstream errorMessage;
                errorMessage << "Multiple persisted data found for " << m_entityKey;
                TA_THROW( DataException( errorMessage.str().c_str(), DataException::NOT_UNIQUE, sql.str() ) );
            }
            else
            {
                try
                {
                    m_valueMap->clear();

                    //
                    // Retrieve common values
                    //
                    m_sourceTime = data->getStringData( 0, DataRecordingDAI::FIELD_SOURCETIME ).empty() ? 
                                   0 : data->getDateData( 0, DataRecordingDAI::FIELD_SOURCETIME );

                    m_updateTime = data->getStringData( 0, DataRecordingDAI::FIELD_UPDATETIME ).empty() ?
                                   0 : data->getDateData( 0, DataRecordingDAI::FIELD_UPDATETIME );

                    std::string tempVal = data->getStringData( 0,  DataRecordingDAI::FIELD_QUALITY );
                    m_quality = tempVal.empty() ? 0 : boost::lexical_cast<int>( tempVal );

                    //
                    // Retrieve type-specific values
                    //
                    columnIter = m_columnMap.begin();
                    for( ; columnIter != m_columnMap.end(); ++columnIter )
                    {
                        m_valueMap->insert( std::map<std::string, std::string>::value_type( columnIter->first,
                                                                                           data->getStringData( 0, columnIter->second ) ) );
                    }

                    m_valid = true;
                }
                catch( const boost::bad_lexical_cast& )
                {
                    std::stringstream errorMessage;
                    errorMessage << "Error converting SourceTime('" << data->getStringData( 0, DataRecordingDAI::FIELD_SOURCETIME )
                                 << "'), UpdateTime('" << data->getStringData( 0, DataRecordingDAI::FIELD_UPDATETIME )
                                 << "') and/or Quality('" << data->getStringData( 0,  DataRecordingDAI::FIELD_QUALITY ) << "')";
                    TA_THROW( DataException( errorMessage.str().c_str(), DataException::INVALID_VALUE, sql.str() ) ); 
                }
            }
        }

        FUNCTION_EXIT;
    }
                    

    void PersistedState::testIsValid()
    {
        if ( ! m_valid )
        {
            reload();
        }
    }

#ifndef _MSC_VER
    /**
     * Boolean specific implementation of getValue()
     */
    template<>
    void PersistedState::getValue<bool>( const std::string& valueId, bool& value )
    {
        testIsValid();

        std::string& valueString = (*m_valueMap)[ valueId ];
        if ( ! valueString.empty() )
        {
            value = ( valueString == DataRecordingDAI::VALUE_TRUE );
        }
    }

    /**
     * Boolean specific implementation of setValue()
     */
    template<>
    void PersistedState::setValue<bool>( const std::string& valueId, const bool& value )
    {
        testIsValid();

        (*m_valueMap)[ valueId ] = value ? DataRecordingDAI::VALUE_TRUE : DataRecordingDAI::VALUE_FALSE;
    }
#endif
}

