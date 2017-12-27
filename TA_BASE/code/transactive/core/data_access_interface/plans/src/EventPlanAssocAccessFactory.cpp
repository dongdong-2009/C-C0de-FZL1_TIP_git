/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/EventPlanAssocAccessFactory.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * EventPlanAssocAccessFactory is a singleton that is used to retrieve 
  * IEventPlanAssocData objects from the database. It is able to provide 
  * read-only access.
  */

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/plans/src/ConfigEventPlanAssocData.h"
#include "core/data_access_interface/plans/src/EventPlanAssocAccessFactory.h"
#include "core/data_access_interface/plans/src/EventPlanAssocData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace
{
    const std::string COLUMN_PKEY( "PKEY" );
    const std::string COLUMN_ENABLED( "ENABLED" );
    const std::string COLUMN_EVENTTYPEKEY( "EVENTTYPEKEY" );
    const std::string COLUMN_ENTITYKEY( "ENTITYKEY" );
    const std::string COLUMN_ENTITYTYPEKEY( "ENTITYTYPEKEY" );
    const std::string COLUMN_PLANPATH( "PLANPATH" );
    const std::string COLUMN_DATECREATED( "DATECREATED" );
    const std::string COLUMN_DATEMODIFIED( "DATEMODIFIED" );
}

namespace TA_Base_Core
{
    EventPlanAssocAccessFactory* EventPlanAssocAccessFactory::s_instance = NULL;

    EventPlanAssocAccessFactory& EventPlanAssocAccessFactory::getInstance()
    {
        if( NULL == s_instance )
        {
            s_instance = new EventPlanAssocAccessFactory();
        }
        return *s_instance;
    }


    EventPlanAssocAccessFactory::EventPlanAssocAccessFactory()
    {
    }


    EventPlanAssocAccessFactory::~EventPlanAssocAccessFactory()
    {
        // Nothing to do.
    }


    std::vector<IEventPlanAssocData*> EventPlanAssocAccessFactory::getAllEventPlanAssociations( const bool readWrite /*=false*/ )
    {
        return getEventPlanAssociations( false, readWrite );
    }


    std::vector<IEventPlanAssocData*> EventPlanAssocAccessFactory::getEnabledEventPlanAssociations( const bool readWrite /*=false*/ )
    {
        return getEventPlanAssociations( true, readWrite );
    }


    std::vector<IEventPlanAssocData*> EventPlanAssocAccessFactory::getEventPlanAssociations( bool onlyEnabled, bool readWrite )
    {
        FUNCTION_ENTRY("getEventPlanAssociations");

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Create the SQL string to retrieve the data of the EventPlanAssocData
        /*std::stringstream sql;
        sql << "SELECT pkey, enabled, eventtypekey, entitykey, entitytypekey, planpath, ";
        sql << "TO_CHAR(date_created,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(date_modified,'YYYYMMDDHH24MISS') FROM eventplanmaps";*/

        // If we only want associations that are enabled add the required WHERE clause.
		std::stringstream ssSql;
        if ( onlyEnabled )
        {
            ssSql << " WHERE enabled != 0";
        }
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), eventplanmaps_Oracle_SELECT_9551, ssSql.str());
//		std::string sql  = databaseConnection->prepareSQLStatement(EVENTPLANMAPS_SELECT_9551, ssSql.str());
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, EVENTPLANMAPS_SELECT_9551, ssSql.str());
//		TA_ASSERT(sql.size() > 0, "getEventPlanAssociations: Get SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( COLUMN_PKEY );
        columnNames.push_back( COLUMN_ENABLED );
        columnNames.push_back( COLUMN_EVENTTYPEKEY );
        columnNames.push_back( COLUMN_ENTITYKEY );
        columnNames.push_back( COLUMN_ENTITYTYPEKEY );
        columnNames.push_back( COLUMN_PLANPATH );
        columnNames.push_back( COLUMN_DATECREATED );
        columnNames.push_back( COLUMN_DATEMODIFIED );

        // Execute the query.
        std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );
        IData* tempPtr = data.get();

        std::vector<IEventPlanAssocData*> eventPlanAssociations;
        do
        {
            // Assign the object to the auto pointer.
            data = std::auto_ptr<IData>(tempPtr);

            for ( unsigned long i = 0; i < data->getNumRows(); i++ )
            {
                // Create the new event plan association objects.
                IEventPlanAssocData* eventPlan;

                // The date modified field can be null!
                time_t dateModified = 0;
                if ( !data->isNull( i, COLUMN_DATEMODIFIED ) )
                {
                    dateModified = data->getDateData( i, COLUMN_DATEMODIFIED );
                }

                if ( readWrite )
                {
                    eventPlan = new ConfigEventPlanAssocData( data->getUnsignedLongData( i, COLUMN_PKEY ),
                                                              data->getBooleanData( i, COLUMN_ENABLED ),
                                                              data->getUnsignedLongData( i, COLUMN_EVENTTYPEKEY ),
                                                              data->getUnsignedLongData( i, COLUMN_ENTITYKEY ),
                                                              data->getUnsignedLongData( i, COLUMN_ENTITYTYPEKEY ),
                                                              data->getStringData( i, COLUMN_PLANPATH ),
                                                              data->getDateData( i, COLUMN_DATECREATED ),
                                                              dateModified );
                }
                else
                {
                    eventPlan = new EventPlanAssocData( data->getUnsignedLongData( i, COLUMN_PKEY ),
                                                        data->getBooleanData( i, COLUMN_ENABLED ),
                                                        data->getUnsignedLongData( i, COLUMN_EVENTTYPEKEY ),
                                                        data->getUnsignedLongData( i, COLUMN_ENTITYKEY ),
                                                        data->getUnsignedLongData( i, COLUMN_ENTITYTYPEKEY ),
                                                        data->getStringData( i, COLUMN_PLANPATH ),
                                                        data->getDateData( i, COLUMN_DATECREATED ),
                                                        dateModified );
                }

                eventPlanAssociations.push_back( eventPlan );
            }

            tempPtr = NULL;
        }
        while ( databaseConnection->moreData(tempPtr) );

        FUNCTION_EXIT;
        return eventPlanAssociations;
    }


    std::auto_ptr<IConfigEventPlanAssocData> EventPlanAssocAccessFactory::createEventPlanAssociation()
    {
        FUNCTION_ENTRY("createEventPlanAssocation");
        FUNCTION_EXIT;
        return std::auto_ptr<IConfigEventPlanAssocData>(new ConfigEventPlanAssocData());
    }


    std::auto_ptr<IConfigEventPlanAssocData> EventPlanAssocAccessFactory::copyEventPlanAssociation( const IConfigEventPlanAssocData* itemToCopy )
    {
        FUNCTION_ENTRY("copyEventPlanAssociation");

        TA_ASSERT( NULL != itemToCopy, "The event plan association to copy was NULL" );
        
        const ConfigEventPlanAssocData* cast = dynamic_cast<const ConfigEventPlanAssocData*>(itemToCopy);

        TA_ASSERT( NULL != cast, "Event plan association passed could not be converted into a ConfigEventPlanAssoc" );

        FUNCTION_EXIT;
        return std::auto_ptr<IConfigEventPlanAssocData>(new ConfigEventPlanAssocData(*cast));
    }
} // closes TA_Base_Core
