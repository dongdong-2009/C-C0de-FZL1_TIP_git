/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/RecordableItemAccessFactory.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <sstream>

#include "core/data_access_interface/data_recording_dai/src/RecordableItemAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/RecordableItem.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/lexical_cast.hpp"

namespace TA_Base_Core
{
    RecordableItemAccessFactory*         RecordableItemAccessFactory::s_singleton = NULL;
    TA_Base_Core::NonReEntrantThreadLockable  RecordableItemAccessFactory::s_singletonLock;

    const std::string RecordableItemAccessFactory::VIEW_NAME     = "dr_recorded_entities_v";

    const std::string RecordableItemAccessFactory::FIELD_ID      = "ENTITY_PKEY";
    const std::string RecordableItemAccessFactory::FIELD_TYPEKEY = "ENTITY_TYPE";
    const std::string RecordableItemAccessFactory::FIELD_ENABLED = "ENABLE_TRENDING";
    const std::string RecordableItemAccessFactory::VALUE_ENABLED = "1";


    RecordableItemAccessFactory& RecordableItemAccessFactory::getInstance()
    {
        if ( s_singleton == NULL )
        {
            TA_Base_Core::ThreadGuard guard( s_singletonLock );
            if ( s_singleton == NULL )
            {
                s_singleton = new RecordableItemAccessFactory();
            }
        }
        return *s_singleton;
    }


    IRecordableItemPtr RecordableItemAccessFactory::getRecordableItem( unsigned long itemId )
    {
        FUNCTION_ENTRY( "getRecordableItem" );

        /*std::string sql = "SELECT " + FIELD_TYPEKEY + "," + FIELD_ENABLED;
        sql += " FROM " + VIEW_NAME;
        sql += " WHERE " + FIELD_ID + "=" + boost::lexical_cast<std::string>( itemId );*/

        std::vector<std::string> columnNames;
        columnNames.push_back( FIELD_TYPEKEY );
        columnNames.push_back( FIELD_ENABLED );

        // Get database connection
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_recorded_entities_v_STD_SELECT_28001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(DR_RECORDED_ENTITIES_V_SELECT_28001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_RECORDED_ENTITIES_V_SELECT_28001,
			boost::lexical_cast<std::string>( itemId ));

        IData* data = databaseConnection->executeQuery( strSql, columnNames );
        if ( data != NULL )
        {
            if ( data->getNumRows() == 1 )
            {
                try
                {
                    IRecordableItemPtr item( new RecordableItem( itemId, 
                                                         boost::lexical_cast<unsigned long>( data->getStringData( 0, FIELD_TYPEKEY ) ),
                                                         data->getStringData( 0, FIELD_ENABLED ) == VALUE_ENABLED ) );

                    return item;
                }
                catch( boost::bad_lexical_cast& )
                {
                    std::stringstream badValueMessage;
                    badValueMessage << "Invalid type key value " << data->getStringData( 0, FIELD_TYPEKEY )
                                    << " for RecordableItem with id " << itemId;

                    TA_THROW( TA_Base_Core::DataException( badValueMessage.str().c_str(), DataException::INVALID_VALUE, FIELD_TYPEKEY ) );
                }
            }
            else if ( data->getNumRows() == 0 )
            {
                std::stringstream errorMessage;
                errorMessage << "Failed to retrieve RecordableItem with id " << itemId;
        
                TA_THROW( TA_Base_Core::DataException( errorMessage.str().c_str(), DataException::NO_VALUE, FIELD_ID ) );
            }
            else
            {
                std::stringstream errorMessage;
                errorMessage << "Multiple records found for RecordableItem with id " << itemId;
        
                TA_THROW( TA_Base_Core::DataException( errorMessage.str().c_str(), DataException::NOT_UNIQUE, FIELD_ID ) );
            }
        }

        std::stringstream failMessage;
        failMessage << "Failed to retrieve RecordableItem details for id " << itemId;
        TA_THROW( TA_Base_Core::DbConnectionFailed( failMessage.str().c_str() ) );
    }
        


    std::vector<IRecordableItemPtr> RecordableItemAccessFactory::getRecordableItems()
    {
        FUNCTION_ENTRY( "getRecordableItems" );

        std::vector<IRecordableItemPtr> items;

        /*std::string sql = "SELECT " + FIELD_ID + "," + FIELD_TYPEKEY + "," + FIELD_ENABLED;
        sql += " FROM " + VIEW_NAME;*/

        std::vector<std::string> columnNames;
        columnNames.push_back( FIELD_ID );
        columnNames.push_back( FIELD_TYPEKEY );
        columnNames.push_back( FIELD_ENABLED );

        // Get database connection
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_recorded_entities_v_STD_SELECT_28002);
//		std::string strSql  = databaseConnection->prepareSQLStatement(DR_RECORDED_ENTITIES_V_SELECT_28002);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_RECORDED_ENTITIES_V_SELECT_28002);

        IData* data = databaseConnection->executeQuery( strSql, columnNames );
        if ( data != NULL )
        {
            do
            {
                unsigned long i = 0;
                try
                {
                    for( ; i < data->getNumRows(); ++i )
                    {
                        unsigned long id = boost::lexical_cast<unsigned long>( data->getStringData( i, FIELD_ID ) );
                        unsigned long typeKey = boost::lexical_cast<unsigned long>( data->getStringData( i, FIELD_TYPEKEY ) );
                        bool enabled = data->getStringData( i, FIELD_ENABLED ) == VALUE_ENABLED;

                        IRecordableItemPtr item( new RecordableItem( id, typeKey, enabled ) );

                        items.push_back( item );
                    }

                }
                catch( boost::bad_lexical_cast& )
                {
                    std::stringstream errorMessage;
                    errorMessage << "Invalid field value (id=" << data->getStringData( i, FIELD_ID ) 
                                 << ", typekey=" << data->getStringData( i, FIELD_TYPEKEY ) << ")";

                    TA_THROW( TA_Base_Core::DataException( errorMessage.str().c_str(), DataException::INVALID_VALUE, "" ) );
                }
                delete data;
                data = NULL;

            } while( databaseConnection->moreData( data ) );
        }
        
        std::stringstream logMessage;
        logMessage << items.size() << " RecordableItems retrieved";
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, logMessage.str().c_str() );

        return items;
    }

    std::vector<IRecordableItemPtr> RecordableItemAccessFactory::getRecordableItemsByType( unsigned long typeKey )
    {
        FUNCTION_ENTRY( "getRecordableItems" );

        std::vector<IRecordableItemPtr> items;

        /*std::string sql = "SELECT " + FIELD_ID + "," + FIELD_ENABLED; 
        sql += " FROM " + VIEW_NAME + " WHERE " + FIELD_TYPEKEY + "=" + boost::lexical_cast<std::string>( typeKey );*/		

        std::vector<std::string> columnNames;
        columnNames.push_back( FIELD_ID );
        columnNames.push_back( FIELD_ENABLED );

        // Get database connection
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_recorded_entities_v_STD_SELECT_28003,
//		std::string strSql  = databaseConnection->prepareSQLStatement(DR_RECORDED_ENTITIES_V_SELECT_28003,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_RECORDED_ENTITIES_V_SELECT_28003,
			boost::lexical_cast<std::string>( typeKey ));

        IData* data = databaseConnection->executeQuery( strSql, columnNames );
        if ( data != NULL )
        {
            do
            {
                unsigned long i = 0;
                try
                {
                    for( ; i < data->getNumRows(); ++i )
                    {
                        unsigned long id = boost::lexical_cast<unsigned long>( data->getStringData( i, FIELD_ID ) );
                        bool enabled = data->getStringData( i, FIELD_ENABLED ) == VALUE_ENABLED;

                        IRecordableItemPtr item( new RecordableItem( id, typeKey, enabled ) );

                        items.push_back( item );
                    }

                }
                catch( boost::bad_lexical_cast& )
                {
                    std::stringstream errorMessage;
                    errorMessage << "Invalid field value (id=" << data->getStringData( i, FIELD_ID ) << ")";

                    TA_THROW( TA_Base_Core::DataException( errorMessage.str().c_str(), DataException::INVALID_VALUE, "" ) );
                }
                delete data;
                data = NULL;

            } while( databaseConnection->moreData( data ) );
        }

        std::stringstream logMessage;
        logMessage << items.size() << " RecordableItems retrieved for type " << typeKey;
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, logMessage.str().c_str() );

        return items;
    }


    RecordableItemAccessFactory::RecordableItemAccessFactory()
    {
        // Nothing to do
    }


    RecordableItemAccessFactory::~RecordableItemAccessFactory()
    {
        // Nothing to do
    }

}

