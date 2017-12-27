/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/LivePropertyAccessFactory.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <sstream>

#include "core/data_access_interface/data_recording_dai/src/LivePropertyAccessFactory.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyData.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyDataHelper.h"
#include "core/data_access_interface/data_recording_dai/src/ConfigLivePropertyData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/lexical_cast.hpp"

namespace TA_Base_Core
{
    LivePropertyAccessFactory*   LivePropertyAccessFactory::s_singleton = NULL;
    NonReEntrantThreadLockable   LivePropertyAccessFactory::s_singletonLock;

    LivePropertyAccessFactory& LivePropertyAccessFactory::getInstance()
    {
        if ( s_singleton == NULL )
        {
            TA_Base_Core::ThreadGuard guard( s_singletonLock );
            
            if ( s_singleton == NULL )
            {
                s_singleton = new LivePropertyAccessFactory();
            }
        }
        return *s_singleton;
    }


    ILivePropertyDataPtr LivePropertyAccessFactory::getLivePropertyData( unsigned long typeKey, const std::string& name, bool writable )
    {
        FUNCTION_ENTRY( "getLivePropertyData" );

        if ( writable )
        {
            ILivePropertyDataPtr propertyData( new ConfigLivePropertyData( typeKey, name ) );
            return propertyData;
        }
        else
        {
            ILivePropertyDataPtr propertyData( new LivePropertyData( typeKey, name ) );
            return propertyData;
        }
    }


    std::vector<ILivePropertyDataPtr> LivePropertyAccessFactory::getLivePropertiesByType( unsigned long typeKey, bool writable )
    {
        FUNCTION_ENTRY( "getLivePropertiesByType" );

        TA_Base_Core::ThreadGuard guard( m_lock );
        LivePropertyMapPtr livePropertyMapPtr= m_LivePropertyMap.find(typeKey);
        if (livePropertyMapPtr != m_LivePropertyMap.end())
        {
            return livePropertyMapPtr->second;
        }
        std::vector<ILivePropertyDataPtr> properties;

        /*std::string sql = "SELECT " + LivePropertyDataHelper::FIELD_NAME;
        sql += " FROM " + LivePropertyDataHelper::TABLE_NAME;
        sql += " WHERE " + LivePropertyDataHelper::FIELD_TYPEKEY + " = " + boost::lexical_cast<std::string>( typeKey );*/

        std::vector<std::string> columnNames;
        columnNames.push_back( LivePropertyDataHelper::FIELD_NAME );

        // Get a connection to the database.
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Dr_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), dr_live_property_STD_SELECT_27502,
//		std::string strSql  = databaseConnection->prepareSQLStatement(DR_LIVE_PROPERTY_SELECT_27502,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, DR_LIVE_PROPERTY_SELECT_27502,
			boost::lexical_cast<std::string>( typeKey ));

        IData* data = databaseConnection->executeQuery( strSql, columnNames );
        if ( data != NULL )
        {
            do
            {
                for( unsigned long i = 0; i < data->getNumRows(); ++i )
                {
                    std::string propertyName = data->getStringData( i, LivePropertyDataHelper::FIELD_NAME );

                    if ( writable )
                    {
                        ILivePropertyDataPtr propertyData( new ConfigLivePropertyData( typeKey, propertyName ) );
                        properties.push_back( propertyData );
                    }
                    else
                    {
                        ILivePropertyDataPtr propertyData( new LivePropertyData( typeKey, propertyName ) );
                        properties.push_back( propertyData );
                    }
                }

                delete data;
                data = NULL;
            }
            while( databaseConnection->moreData( data ) );
        }

        std::stringstream logMessage;
        logMessage << properties.size() << " LiveProperties retrieved for type key " << typeKey;
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, logMessage.str().c_str() );

        m_LivePropertyMap.insert(LivePropertyMap::value_type(typeKey, properties));
        return properties;
    }
            

    IConfigLivePropertyDataPtr LivePropertyAccessFactory::createLiveProperty()
    {
        FUNCTION_ENTRY( "createLiveProperty" );

        IConfigLivePropertyDataPtr propertyData( new ConfigLivePropertyData() );
        return propertyData;
    }
    

    LivePropertyAccessFactory::LivePropertyAccessFactory()
    {
        // Nothing
    }


    LivePropertyAccessFactory::~LivePropertyAccessFactory()
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        for (LivePropertyMapPtr livePropertyMapPtr = m_LivePropertyMap.begin();
                livePropertyMapPtr != m_LivePropertyMap.end(); ++livePropertyMapPtr)
        {
            (livePropertyMapPtr->second).clear();
        }
        m_LivePropertyMap.clear();
    }

}
