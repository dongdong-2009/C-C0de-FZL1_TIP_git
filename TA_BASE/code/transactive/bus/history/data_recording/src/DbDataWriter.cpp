/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/DbDataWriter.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * IDataWriter implementation for persisting cached data to
 * a database
 *
 */

#ifdef _MSC_VER
  #pragma warning (disable : 4786)
#endif // _MSC_VER

#include <sstream>

#include "bus/history/data_recording/src/DbDataWriter.h"
#include "bus/history/data_recording/src/DataRecordingException.h"
#include "core/data_access_interface/data_recording_dai/src/DataRecordingDAI.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
//#include "core/data_access_interface/src/SQLCode.h"
#include "boost/lexical_cast.hpp"
#include "ace/OS.h"

// TD19620++
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

const std::string CENTRAL_DB_STRING			= "TRANSACT";
const std::string LOCAL_DB_STRING_PREFIX	= "TRA_";
const unsigned long ALL_LOCATIONS			= 0;
// ++TD19620

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    DbDataWriter::DbDataWriter()
		:m_localDatabase("")
    {
        // Nothing to do
    }


    DbDataWriter::~DbDataWriter()
    {
        // Nothing to do
    }


    void DbDataWriter::write( DataCachePtr& cache )
    {
        FUNCTION_ENTRY( "write" );

        try
        {
            TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Dr_Cd, TA_Base_Core::Write);

            const DataCache::DataMap& dataMap = cache->getData();
            const std::map<std::string, std::string>& columnMap = cache->getColumnMap();

            std::string tableName = TA_Base_Core::DataRecordingDAI::getWriteTableName( cache->getType() );
			tableName = TA_Base_Core::strlwr(const_cast<char*>(tableName.c_str()));

            DataCache::DataMap::const_iterator dataIter = dataMap.begin();
            for( ; dataIter != dataMap.end(); ++dataIter )
            {
                const CachedData& data = dataIter->second;

                struct tm timeStruct;

                //convert the timestamp in UTC time to local time before write to database
                ACE_OS::localtime_r( &data.m_timestamp, &timeStruct );
                
                //
                // Construct SQL statement for update
                //

                std::stringstream sql;
				//sql << "BEGIN ";
				// Yanrong++ TD19620
				//sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','UPDATE', '";
				//sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName(data) << "','UPDATE', '";
				// ++Yanrong TD19620
            /*    sql << "UPDATE " << tableName << " SET " << TA_Base_Core::DataRecordingDAI::FIELD_SOURCETIME 
                    << "=" << getSourceTime( timeStruct ) << ", "
                    << TA_Base_Core::DataRecordingDAI::FIELD_QUALITY << "=" << data.m_quality;*/

                std::vector<Datum>::const_iterator datumIter = data.m_data.begin();
                for( ; datumIter != data.m_data.end(); ++datumIter )
                {
                    const Datum& datum = *datumIter;
                    std::map<std::string, std::string>::const_iterator columnIter = columnMap.find( datum.getName() );
                    if ( columnIter != columnMap.end() )
                    {
						switch( datum.getType() )
						{
						case Datum::FLOAT_ITEM :
							sql << "," << columnIter->second << "=" << datum.toString();							 
							break;
							
						case Datum::INTEGER_ITEM :
							sql << "," << columnIter->second << "=" << datum.toString();
							break;
							
						case Datum::STRING_ITEM :
							sql << "," << columnIter->second << "=" << "''" + database->escapeAQSQLString(datum.toString()) + "''";
							break;
							
						case Datum::BOOLEAN_ITEM :
							sql << "," << columnIter->second << "=" << ( datum.getValue().m_bool ? "1" : "0" );
							break;
							
						default :
							TA_ASSERT( false, "Unsupported Datum type" );
							break;
						}                        
                    }
                    else
                    {
                        std::stringstream errorMessage;
                        errorMessage << "Datum Name " << datum.getName() << " does not exist in column map";
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, errorMessage.str().c_str() );
                    }
                }

                // Add WHERE clause
              /*  sql << " WHERE " << TA_Base_Core::DataRecordingDAI::FIELD_ENTITYKEY << "=" << data.m_id;
				sql << "'); ";
				sql << "END;";*/
				//int nDBType = enumBeginDbType;
				TA_Base_Core::SQLStatement strSql;
				TA_Base_Core::SQLTypeAdapter sqlAdapter;
				getSourceTime( sqlAdapter, timeStruct );
				/*while (++nDBType != enumEndDbType)
				{
					//strSql = database->prepareSQLStatement(DR_DATANODE_STATE_UPDATE_27052, nDBType, strSql, getLocalDatabaseName(data),

					//dynsql delete by lin
		            database->prepareSQLStatement(strSql, DR_DATANODE_STATE_UPDATE_27052, nDBType, strSql, getLocalDatabaseName(data),
						tableName, getSourceTime( nDBType, timeStruct ), data.m_quality, sql.str(), data.m_id);
				} */
				database->prepareSQLStatement(strSql, DR_DATANODE_STATE_UPDATE_27052, getLocalDatabaseName(data),
						tableName, sqlAdapter, data.m_quality, sql.str(), data.m_id);					
				
                database->executeModification( strSql );
            }
        }
        catch( TA_Base_Core::DatabaseException& dbex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", dbex.what() );
            TA_THROW( TA_Base_Core::DataRecordingException( "A database error has occurred" ) );
        }
        catch( TA_Base_Core::DataException& dex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", dex.what() );
            TA_THROW( TA_Base_Core::DataRecordingException( "A data error has occurred" ) );
        }
    }
            

    void DbDataWriter::getSourceTime(TA_Base_Core::SQLTypeAdapter& sqlAdpter, struct tm& timeStruct ) const
    {
        std::stringstream sourceTime;
		/*switch( nDbType )
		{
		case enumOracleDb:
        sourceTime << "TO_DATE(''" 
                   << ( timeStruct.tm_mday < 10 ? "0" : "" ) << timeStruct.tm_mday << "/"  
                   << ( timeStruct.tm_mon + 1 < 10 ? "0" : "" ) << timeStruct.tm_mon + 1 << "/"  
                   << timeStruct.tm_year + 1900 << " "  
                   << ( timeStruct.tm_hour < 10 ? "0" : "" ) << timeStruct.tm_hour << ":"  
                   << ( timeStruct.tm_min < 10 ? "0" : "" ) << timeStruct.tm_min << ":" 
                   << ( timeStruct.tm_sec < 10 ? "0" : "" ) << timeStruct.tm_sec
                   << "'',''dd/mm/yyyy hh24:mi:ss'')"; 
                break;
		case enumMysqlDb:
			sourceTime << "STR_TO_DATE(''" 
				<< ( timeStruct.tm_mday < 10 ? "0" : "" ) << timeStruct.tm_mday << "/"  
				<< ( timeStruct.tm_mon + 1 < 10 ? "0" : "" ) << timeStruct.tm_mon + 1 << "/"  
				<< timeStruct.tm_year + 1900 << " "  
				<< ( timeStruct.tm_hour < 10 ? "0" : "" ) << timeStruct.tm_hour << ":"  
				<< ( timeStruct.tm_min < 10 ? "0" : "" ) << timeStruct.tm_min << ":" 
				<< ( timeStruct.tm_sec < 10 ? "0" : "" ) << timeStruct.tm_sec
				<< "'',''%d/%m/%Y %H:%i:%S'')"; 
                break;	
        }*/
		sourceTime << ( timeStruct.tm_mday < 10 ? "0" : "" ) << timeStruct.tm_mday << "/"  
			<< ( timeStruct.tm_mon + 1 < 10 ? "0" : "" ) << timeStruct.tm_mon + 1 << "/"  
			<< timeStruct.tm_year + 1900 << " "  
			<< ( timeStruct.tm_hour < 10 ? "0" : "" ) << timeStruct.tm_hour << ":"  
			<< ( timeStruct.tm_min < 10 ? "0" : "" ) << timeStruct.tm_min << ":" 
			<< ( timeStruct.tm_sec < 10 ? "0" : "" ) << timeStruct.tm_sec;
		
		sqlAdpter.add(TA_Base_Core::enumDateFormat_2, TA_Base_Core::enumFuncFormat_2, sourceTime.str(), true);         

        /*return sourceTime.str()*/;
    }

    std::string DbDataWriter::getValueAsDbString( const Datum& datum ) const
	{
        switch( datum.getType() )
		{
            case Datum::FLOAT_ITEM :
                return datum.toString();
                break;

            case Datum::INTEGER_ITEM :
                return datum.toString();
                break;

            case Datum::STRING_ITEM :
                return "'" + datum.toString() + "'";
                break;
 
            case Datum::BOOLEAN_ITEM :
                return ( datum.getValue().m_bool ? "1" : "0" );
                break;
        
            default :
                TA_ASSERT( false, "Unsupported Datum type" );
                return "";
			}
    }

	std::string DbDataWriter::getLocalDatabaseName()
			{
		if (m_localDatabase.empty())
			{
			// have to find out what is the local db name
			// this should be the first in the connection list so grab it
            if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
			{
				TA_THROW(TA_Base_Core::DatabaseException("db-connection-file not set"));
			}
			try
			{
				/*TA_Base_Core::DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				TA_Base_Core::DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Dr_Cd, TA_Base_Core::Write);
				m_localDatabase = connections[0].first; */
				std::string strDbConFile = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConFile,TA_Base_Core::Dr_Cd, TA_Base_Core::Write);
		}
			catch(...) // most likely a file not found error
			{
				TA_THROW( TA_Base_Core::DatabaseException("Unable to find database connection infomation"));
	}
		}
		
		return m_localDatabase;
	}

	// Yanrong++ TD19620
	std::string DbDataWriter::getLocalDatabaseName( const CachedData& data )
	{
		std::string localDatabaseName = getLocalDatabaseName();
		// TD19620: When use CE set datapoin/datanode value, we should update 
		// the corresponding local database, not central database.

		for (int i=0; i<localDatabaseName.length(); i++) {
			localDatabaseName[i] = toupper(localDatabaseName[i]);
		}

		// if Use central DB string, it must come from CE.
		if( strcmp( localDatabaseName.c_str(), CENTRAL_DB_STRING.c_str() ) == 0 ) 
		{
			TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( data.m_id);
			if( entityData->getLocation() == ALL_LOCATIONS )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD19620, location key should not equal 0 (all locations)!" );
			}

			// constructor local database string.
			localDatabaseName = LOCAL_DB_STRING_PREFIX;
			localDatabaseName += entityData->getLocationName();

			delete entityData;
			entityData = NULL;
		}

		return localDatabaseName;
	}
	// ++Yanrong TD19620
}

