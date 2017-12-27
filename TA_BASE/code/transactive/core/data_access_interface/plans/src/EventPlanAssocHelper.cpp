/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/EventPlanAssocHelper.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 10:18:09 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * EventPlanAssocHelper is an object that is held by all type-specific event plan association objects.
  * It contains all data common across event plan associations, and manipulation methods for the data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include <memory>
#include <vector>

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/plans/src/EventPlanAssocHelper.h"
#include "core/data_access_interface/plans/src/IPlan.h"
#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IMessageTypeData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/MessageTypeAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace
{
    const std::string COLUMN_KEY( "PKEY" );
    const std::string COLUMN_ENABLED( "ENABLED" );
    const std::string COLUMN_EVENTTYPEKEY( "EVENTTYPEKEY" );
    const std::string COLUMN_ENTITYKEY( "ENTITYKEY" );
    const std::string COLUMN_ENTITYTYPEKEY( "ENTITYTYPEKEY" );
    const std::string COLUMN_PLANPATH( "PLANPATH" );
    const std::string COLUMN_DATECREATED( "DATE_CREATED" );
    const std::string COLUMN_DATEMODIFIED( "DATE_MODIFIED" );
};

namespace TA_Base_Core
{

    EventPlanAssocHelper::EventPlanAssocHelper( ) 
        : m_key(0),
          m_enabled(false),
          m_eventTypeKey(0),
          m_entityKey(0),
          m_entityTypeKey(0),
          m_planPath(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(true),
          m_isValidData(false)
    {
    }


    EventPlanAssocHelper::EventPlanAssocHelper( unsigned long key,
                                                bool enabled,
                                                unsigned long eventTypeKey,
                                                unsigned long entityKey,
                                                unsigned long entityTypeKey,
                                                const std::string& planPath,
                                                time_t dateCreated,
                                                time_t dateModified)

        : m_key(key),
          m_enabled(enabled),
          m_eventTypeKey(eventTypeKey),
          m_entityKey(entityKey),
          m_entityTypeKey(entityTypeKey),
          m_planPath(planPath),
          m_dateCreated(dateCreated),
          m_dateModified(dateModified),
          m_isNew(false),
          m_isValidData(true)
    {
    }


    EventPlanAssocHelper::EventPlanAssocHelper( unsigned long key )
        : m_key(key),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(false),
          m_isValidData(false)
    {
    }


    EventPlanAssocHelper::EventPlanAssocHelper( const EventPlanAssocHelper& rhs )
        : m_key(0),
          m_enabled(rhs.m_enabled),
          m_eventTypeKey(rhs.m_eventTypeKey),
          m_entityKey(rhs.m_entityKey),
          m_entityTypeKey(rhs.m_entityTypeKey),
          m_planPath(rhs.m_planPath),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(true),
          m_isValidData(false)
    {
    }


    EventPlanAssocHelper::~EventPlanAssocHelper()
    {
    }


    unsigned long EventPlanAssocHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");

        TA_ASSERT( !m_isNew, "Cannot retrieve the key on a event plan association before it has been written to the database" );

        FUNCTION_EXIT;
        return m_key;
    }


    unsigned long EventPlanAssocHelper::getEventTypeKey()
    {
        FUNCTION_ENTRY("getEventTypeKey");

        // Make sure the data is loaded.
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_eventTypeKey;
    }


    void EventPlanAssocHelper::setEventTypeKey( unsigned long eventTypeKey )
    {
        FUNCTION_ENTRY("setEventType");

        // Make sure the data is loaded.
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        m_eventTypeKey = eventTypeKey;

        FUNCTION_EXIT;
    }


    bool EventPlanAssocHelper::isEnabled()
    {
        FUNCTION_ENTRY("isEnabled");

        // Make sure the data is loaded.
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_enabled;
    }


    void EventPlanAssocHelper::setEnabled( bool enabled )
    {
        FUNCTION_ENTRY("setEnabled");

        // Make sure the data is loaded.
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        m_enabled = enabled;

        FUNCTION_EXIT;
    }

    unsigned long EventPlanAssocHelper::getEntityKey()
    {
        FUNCTION_ENTRY("getEntityKey");

        // Make sure the data is loaded.
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_entityKey;
    }


    void EventPlanAssocHelper::setEntityKey( unsigned long entityKey )
    {
        FUNCTION_ENTRY("setEntityKey");

        // Make sure the data is loaded.
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        m_entityKey = entityKey;

        FUNCTION_EXIT;
    }


    unsigned long EventPlanAssocHelper::getEntityTypeKey()
    {
        FUNCTION_ENTRY("getEntityTypeKey");

        // Make sure the data is loaded.
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_entityTypeKey;
    }


    void EventPlanAssocHelper::setEntityTypeKey(unsigned long entityTypeKey)
    {
        FUNCTION_ENTRY("setEntityTypeKey");

        // Make sure the data is loaded.
        if ( !m_isValidData && !m_isNew )
        {
            reload();
        }

        m_entityTypeKey = entityTypeKey;

        FUNCTION_EXIT;
    }

    
    std::string EventPlanAssocHelper::getPlanPath()
    {
        FUNCTION_ENTRY("getPlanPath");

        // Make sure the data is loaded.
        if (!m_isValidData && !m_isNew)  
        {            
            reload();
        }

        FUNCTION_EXIT;
        return m_planPath;
    }

        
	void EventPlanAssocHelper::setPlanPath(const std::string& planPath)
    {
        FUNCTION_ENTRY("setPlanPath");

        // Make sure the data is loaded.
        if (!m_isValidData && !m_isNew)  
        {            
            reload();
        }

        // Create a Plan object based on the specified path. This will ensure the path is valid.
        std::auto_ptr<IPlan> plan(PlanAccessFactory::getInstance().getPlanByPath(planPath));
        m_planPath = plan->getPath();

        FUNCTION_EXIT;
    }

    time_t EventPlanAssocHelper::getDateCreated()
    {
        TA_ASSERT( !m_isNew, "The data must be written to the database before the date created can be retrieved" );

        if ( !m_isValidData )
        {
            reload();
        }

        return m_dateCreated;
    }


    time_t EventPlanAssocHelper::getDateModified()
    {
        TA_ASSERT( !m_isNew, "The data must be written to the database before the date modified can be retrieved" );

        if (!m_isValidData)
        {
            reload();
        }

        return m_dateModified;
    }


    unsigned long EventPlanAssocHelper::retrieveEventTypeKey( const std::string& eventTypeName )
    {
        FUNCTION_ENTRY("retrieveEventTypeKey");

        unsigned long key = 0;;

        // Make sure its a non-zero event type key.
        if ( !eventTypeName.empty() )
        {
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
            // Construct the query..
            /*std::stringstream sql; 
            sql << "SELECT memtyp_id FROM me_message_type WHERE type_name='" << databaseConnection->escapeInsertString(eventTypeName) << "'";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_message_type_STD_SELECT_10001,
//			std::string strSql  = databaseConnection->prepareSQLStatement(ME_MESSAGE_TYPE_SELECT_10001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ME_MESSAGE_TYPE_SELECT_10001,
				databaseConnection->escapeInsertString(eventTypeName));

            std::string pkeyColumn("PKEY");
            std::vector<std::string> columnNames;
            columnNames.push_back( pkeyColumn );

            // Execute it and ensure that only one row was returned.
            std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );
            if ( 1 == data->getNumRows() )
            {
                key = data->getUnsignedLongData( 0, pkeyColumn );
            }
            else
            {
                TA_THROW( DataException( "Could not find unique event type name.", DataException::NO_VALUE, "" ) );
            }
        }

        FUNCTION_EXIT;
        return key;
    }


    std::string EventPlanAssocHelper::retrieveEventTypeName( unsigned long eventTypeKey )
    {
        FUNCTION_ENTRY("retrieveEventTypeName");

        std::string name("");

        // Make sure its a non-zero event type key.
        if ( 0 != eventTypeKey )
        {
            // getMessageTypeByKey takes a string of all things, so convert our key to a string.
            std::stringstream numberToStr;
            numberToStr << eventTypeKey;

            std::auto_ptr<IMessageTypeData> data( MessageTypeAccessFactory::getInstance().getMessageTypeByKey( numberToStr.str(), false ) );
            name = data->getName();
        }

        FUNCTION_EXIT;
        return name;
    }
	std::string EventPlanAssocHelper::retrieveEventTypeDisplayName( unsigned long eventTypeKey )
	{
		FUNCTION_ENTRY("retrieveEventTypeDisplayName");

		std::string name("");

		// Make sure its a non-zero event type key.
		if ( 0 != eventTypeKey )
		{
			// getMessageTypeByKey takes a string of all things, so convert our key to a string.
			std::stringstream numberToStr;
			numberToStr << eventTypeKey;

			std::auto_ptr<IMessageTypeData> data( MessageTypeAccessFactory::getInstance().getMessageTypeByKey( numberToStr.str(), false ) );
			name = data->getDisplayName();
		}

		FUNCTION_EXIT;
		return name;
	}


    unsigned long EventPlanAssocHelper::retrieveEntityTypeKey( const std::string& entityTypeName )
    {
        FUNCTION_ENTRY("retrieveEntityTypeKey");

        unsigned long key = 0;

        // Make sure its a non-zero entity type key.
        if ( !entityTypeName.empty() )
        {
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

            /*std::stringstream sql; 
            sql << "SELECT pkey FROM entitytype WHERE name='" << databaseConnection->escapeInsertString(entityTypeName) << "'";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4001,
//			std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4001,
				databaseConnection->escapeInsertString(entityTypeName));

            std::string pkeyColumn("PKEY");
            std::vector<std::string> columnNames;
            columnNames.push_back( pkeyColumn );

            // Execute it and ensure that only one row was returned.
            std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );
            if ( 1 == data->getNumRows() )
            {
                key = data->getUnsignedLongData( 0, pkeyColumn );
            }
            else
            {
                TA_THROW( DataException( "Could not find unique entity type name.", DataException::NO_VALUE, "" ) );
            }
        }

        FUNCTION_EXIT;
        return key;
    }


    std::string EventPlanAssocHelper::retrieveEntityTypeName( unsigned long entityTypeKey )
    {
        FUNCTION_ENTRY("retrieveEntityTypeName");

        std::string name("");

        // Make sure its a non-zero entity type key.
        if ( 0 != entityTypeKey )
        {
			IDatabase *pDb = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
            /*std::ostringstream sql; 
            sql << "SELECT name FROM entitytype WHERE pkey=" << entityTypeKey;*/
//			std::string strSql = defPrepareSQLStatement(pDb->GetDbServerType(), Entitytype_STD_SELECT_4011, entityTypeKey);
//			std::string strSql  = pDb->prepareSQLStatement(ENTITYTYPE_SELECT_4011, entityTypeKey);
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4011, entityTypeKey);

            std::string nameColumn("NAME");
            std::vector<std::string> columnNames;
            columnNames.push_back( nameColumn );

            // Execute it and ensure that only one row was returned.
            std::auto_ptr<IData> data( pDb->executeQuery( strSql, columnNames ) );
            if ( 1 == data->getNumRows() )
            {
                name = data->getStringData( 0, nameColumn );
            }
        }

        FUNCTION_EXIT;
        return name;
    }


    unsigned long EventPlanAssocHelper::retrieveEntityKey( const std::string& entityName )
    {
        FUNCTION_ENTRY("retrieveEntityKey");

        unsigned long key = 0;

        // Make sure the entity name exists..
        if ( !entityName.empty() )
        {
            std::auto_ptr<IEntityData> data( EntityAccessFactory::getInstance().getEntity( entityName ) );
            key = data->getKey();
        }

        FUNCTION_EXIT;
        return key;
    }


    std::string EventPlanAssocHelper::retrieveEntityName( unsigned long entityKey )
    {
        FUNCTION_ENTRY("retrieveEntityName");

        std::string name("");

        // Make sure its a non-zero entity key.
        if ( 0 != entityKey )
        {
            std::auto_ptr<IEntityData> data( EntityAccessFactory::getInstance().getEntity( entityKey ) );
            name = data->getName();
        }

        FUNCTION_EXIT;
        return name;
    }


    void EventPlanAssocHelper::deleteThisAssociation()
    {
        FUNCTION_ENTRY("deleteThisAssociation");

        TA_ASSERT( !m_isNew, "This location does not yet exist in the database, and therefore cannot be deleted" );

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Create the modification string.
        /*std::ostringstream sql;
        sql << "DELETE FROM eventplanmaps WHERE pkey=" << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), eventplanmaps_STD_DELETE_9901, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENTPLANMAPS_DELETE_9901, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENTPLANMAPS_DELETE_9901, m_key);

        // Execute the modification.
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void EventPlanAssocHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.

        std::vector<std::string> fieldNames;
        if ( 0 == m_eventTypeKey )
        {
            fieldNames.push_back( "Event Type" );
        }
        if ( !(( 0 == m_entityKey ) ^ ( 0 == m_entityTypeKey )) )
        {
            fieldNames.push_back( "Entity or Entity Type" );
        }
        if ( m_planPath.empty() )
        {
            fieldNames.push_back( "Plan" );
        }

        // Now, if we have at least one entry in the vector required data is missing.
        if ( 0 < fieldNames.size() )
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("Event Plan Association data not fully specified. Event Plan Association cannot be written to database" ,fieldNames) );
        }

        if (m_isNew)
        {
            // If its a new item create it.
            addNew();
        }
        else 
        {
            // Otherwise modify its values.
            modifyExisting();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void EventPlanAssocHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new alarm plan association" );

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void EventPlanAssocHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        /*std::ostringstream sql;
        sql << "SELECT pkey, enabled, eventtypekey, entitykey, entitytypekey, planpath, ";
        sql << "TO_CHAR(date_created,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(date_modified,'YYYYMMDDHH24MISS') FROM eventplanmaps WHERE pkey=" << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), eventplanmaps_Oracle_SELECT_9552, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENTPLANMAPS_SELECT_9552, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENTPLANMAPS_SELECT_9552, m_key);


        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( COLUMN_KEY );
        columnNames.push_back( COLUMN_ENABLED );
        columnNames.push_back( COLUMN_EVENTTYPEKEY );
        columnNames.push_back( COLUMN_ENTITYKEY );
        columnNames.push_back( COLUMN_ENTITYTYPEKEY );
        columnNames.push_back( COLUMN_PLANPATH );
        columnNames.push_back( COLUMN_DATECREATED );
        columnNames.push_back( COLUMN_DATEMODIFIED );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );

        // Make sure there is one, and only one, row returned.
        if ( 0 == data->getNumRows() )
        {
            // No entry found with the specified pkey
            std::ostringstream reasonMessage;
            reasonMessage << "No event plan association found for pkey=" << m_key;
            TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NO_VALUE, "" ) );
        }
        else if ( 1 < data->getNumRows() )
        {
            // More than one entry found for the pkey
            std::ostringstream reasonMessage;
            reasonMessage << "More than one event plan association found for pkey=" << m_key;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key = data->getUnsignedLongData( 0, COLUMN_KEY );
        m_enabled = data->getBooleanData( 0, COLUMN_ENABLED );
        m_eventTypeKey = data->getUnsignedLongData( 0, COLUMN_EVENTTYPEKEY );
        m_entityKey = data->getUnsignedLongData( 0, COLUMN_ENTITYKEY );
        m_entityTypeKey = data->getUnsignedLongData( 0, COLUMN_ENTITYTYPEKEY );
        m_planPath = data->getStringData( 0, COLUMN_PLANPATH );
        m_dateCreated = data->getDateData( 0, COLUMN_DATECREATED );

        m_dateModified = 0;
        if ( !data->isNull( 0, COLUMN_DATEMODIFIED ) )
        {
            m_dateModified = data->getDateData( 0, COLUMN_DATEMODIFIED );
        }

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void EventPlanAssocHelper::addNew()
    {
        FUNCTION_ENTRY("addNew");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "This Event Plan Association is a new association. It will be added to the database." );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // We must retrieve the key first so we know where we are inserting the data. No other fields
        // are unique so this is our only identifier.
        /*std::string sql ("SELECT eventplanmaps_seq.NEXTVAL FROM dual");*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), eventplanmaps_Oracle_SELECT_9553);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENTPLANMAPS_SELECT_9553);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENTPLANMAPS_SELECT_9553);

        std::vector<std::string> columnNames;
        columnNames.push_back( COLUMN_KEY );

        // Execute the query.
        std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data->getNumRows() )
        {
            TA_THROW( DataException("Could not retrieve a new key for the alarm plan association",DataException::NO_VALUE,"pkey") );
        }

        m_key = data->getUnsignedLongData( 0, COLUMN_KEY );

        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the entity (reuse the string)
        /*std::ostringstream formatSQL;
        formatSQL << "INSERT INTO eventplanmaps ( pkey, enabled, eventtypekey, entitykey, entitytypekey, ";
        formatSQL << "planpath ) VALUES ( " << m_key << ", " << (m_enabled?1:0) << ", "<< m_eventTypeKey;
        formatSQL << ", " << m_entityKey << ", " << m_entityTypeKey;
        formatSQL << ", '" << databaseConnection->escapeInsertString(m_planPath) << "' )";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), eventplanmaps_STD_INSERT_9801, m_key, (m_enabled?1:0), m_eventTypeKey,
//		strSql  = databaseConnection->prepareSQLStatement(EVENTPLANMAPS_INSERT_9801, m_key, (m_enabled?1:0), m_eventTypeKey,

		databaseConnection->prepareSQLStatement(strSql, EVENTPLANMAPS_INSERT_9801, m_key, (m_enabled?1:0), m_eventTypeKey,
			m_entityKey, m_entityTypeKey, databaseConnection->escapeInsertString(m_planPath));

        databaseConnection->executeModification(strSql);

        // Now we need to get the creation date.
        /*formatSQL.str("");
        formatSQL << "SELECT TO_CHAR(date_created,'YYYYMMDDHH24MISS') FROM eventplanmaps WHERE ";
        formatSQL << " pkey=" << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), eventplanmaps_Oracle_SELECT_9554, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(EVENTPLANMAPS_SELECT_9554, m_key);

		databaseConnection->prepareSQLStatement(strSql, EVENTPLANMAPS_SELECT_9554, m_key);

        // Create the list of columns to be retrieved.
        columnNames.clear();
        columnNames.push_back( COLUMN_DATECREATED );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = std::auto_ptr<IData>( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data->getNumRows() )
        {
            // No entry found with the specified key.
            std::ostringstream reasonMessage;
            reasonMessage << "No data found for key=" << m_key;
            TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NO_VALUE, "" ) );
        }
        else if ( 1 < data->getNumRows() )
        {
            // More than one entry found for the name
            std::ostringstream reasonMessage;
            reasonMessage << "More than one entry found for key=" << m_key;
            TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "" ) );
        }

        // Retrieve the date created from the returned data.
        m_dateCreated = data->getDateData(0, COLUMN_DATECREATED );

        FUNCTION_EXIT;
    }


    void EventPlanAssocHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Event Plan Association %lu already exists. It's details will be updated.", m_key );

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // Construct the update and execute it.
        /*std::ostringstream sql;
        sql << "UPDATE eventplanmaps SET enabled=" << (m_enabled?1:0) << ", eventtypekey=";
        sql << m_eventTypeKey << ", entitykey=" << m_entityKey << ", entitytypekey=";
        sql << m_entityTypeKey << ", planpath='" << databaseConnection->escapeInsertString(m_planPath) << "'";
        sql << " WHERE pkey=" << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), eventplanmaps_STD_UPDATE_9651, (m_enabled?1:0),
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENTPLANMAPS_UPDATE_9651, (m_enabled?1:0),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENTPLANMAPS_UPDATE_9651, (m_enabled?1:0),
			m_eventTypeKey, m_entityKey, m_entityTypeKey, databaseConnection->escapeInsertString(m_planPath), m_key);

        databaseConnection->executeModification(strSql);

        // Now we need to update the date fields
        /*sql.str("");
        sql << "SELECT TO_CHAR(date_created,'YYYYMMDDHH24MISS'), TO_CHAR(date_modified,'YYYYMMDDHH24MISS') ";
        sql << "FROM eventplanmaps WHERE pkey=" << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), eventplanmaps_Oracle_SELECT_9555, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(EVENTPLANMAPS_SELECT_9555, m_key);

		databaseConnection->prepareSQLStatement(strSql, EVENTPLANMAPS_SELECT_9555, m_key);

        // Create the vector of columns we want.
        std::vector<std::string> columnNames;
        columnNames.push_back( COLUMN_DATECREATED );
        columnNames.push_back( COLUMN_DATEMODIFIED );

        // Execute the query. The method can throw a DatabaseException.
        std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );
        if ( 0 == data->getNumRows() )
        {
            // No entry found with the specified name
            std::ostringstream reasonMessage;
            reasonMessage << "No data found for key=" << m_key;
            TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NO_VALUE, "" ) );
        }
        else if ( 1 < data->getNumRows() )
        {
            // More than one entry found for the name
            std::ostringstream reasonMessage;
            reasonMessage << "More than one entry found for key = " << m_key;
            TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "" ) );
        }

        // Save the date information.
        m_dateCreated = data->getDateData( 0, COLUMN_DATECREATED );
        m_dateModified = data->getDateData( 0, COLUMN_DATEMODIFIED );

        FUNCTION_EXIT;
    }
} // closes TA_Base_Core


