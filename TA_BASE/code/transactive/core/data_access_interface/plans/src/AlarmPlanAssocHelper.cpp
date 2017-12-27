/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/AlarmPlanAssocHelper.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 10:18:09 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * AlarmPlanAssocHelper is an object that is held by all type-specific alarm plan association objects.
  * It contains all data common across alarm plan associations, and manipulation
  * methods for the data. It helps avoid re-writing code for each object.
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include <vector>

#include "core/data_access_interface/plans/src/AlarmPlanAssocHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/plans/src/IPlan.h"
#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    AlarmPlanAssocHelper::AlarmPlanAssocHelper( ) 
        : m_key(0),
		  m_entityKey(0),
		  m_entityTypeKey(0),
		  m_alarmType(0),
		  m_planPath(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(true),
          m_isValidData(false)
    {
    }


    AlarmPlanAssocHelper::AlarmPlanAssocHelper( unsigned long key, 
			                                    unsigned long entityKey,
		                                        unsigned long entityTypeKey,
			                                    unsigned long alarmType,
			                                    const std::string& planPath,
                                                time_t dateCreated,
                                                time_t dateModified)

        : m_key(key),
		  m_entityKey(entityKey),
		  m_entityTypeKey(entityTypeKey),
		  m_alarmType(alarmType),
		  m_planPath(planPath),
          m_dateCreated(dateCreated),
          m_dateModified(dateModified),
          m_isNew(false),
          m_isValidData(true)
    {
    }


    AlarmPlanAssocHelper::AlarmPlanAssocHelper( unsigned long key )
        : m_key(key),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(false),
          m_isValidData(false)
    {
    }


	AlarmPlanAssocHelper::AlarmPlanAssocHelper( const AlarmPlanAssocHelper& rhs)
        : m_key(0),
		  m_entityKey(rhs.m_entityKey),
		  m_entityTypeKey(rhs.m_entityTypeKey),
		  m_alarmType(rhs.m_alarmType),
		  m_planPath(rhs.m_planPath),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(true),
          m_isValidData(false)
    {
    }


    AlarmPlanAssocHelper::~AlarmPlanAssocHelper()
    {
    }


    unsigned long AlarmPlanAssocHelper::getKey()
    {
        TA_ASSERT(!m_isNew, "Cannot retrieve the key on a alarm plan association before it has been applied to the database");
        return m_key;
    }

    
    std::string AlarmPlanAssocHelper::getName()
    {
        // We don't want to call reload() if this is a new alarm plan association and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing alarm plan association and the data needs to be loaded
            reload();
        }
        
        std::string name = "Alarm Type: ";
        name += retrieveAlarmTypeName(m_alarmType);

        if (m_entityKey != 0)
        {
            name += ", Entity: ";
            name += retrieveEntityName(m_entityKey);
        }
        else if (m_entityTypeKey != 0)
        {
            name += ", Entity Type: ";
            name += retrieveEntityTypeName(m_entityTypeKey);
        }

        return name;
    }


    unsigned long AlarmPlanAssocHelper::getAlarmType()
    {
        // We don't want to call reload() if this is a new alarm plan association and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing alarm plan association and the data needs to be loaded
            reload();
        }

        return m_alarmType;
    }


    void AlarmPlanAssocHelper::setAlarmType(unsigned long alarmType)
    {
        FUNCTION_ENTRY("setAlarmType");

        if (!m_isValidData && !m_isNew)  
        {            
            reload();
        }

        m_alarmType = alarmType;

        FUNCTION_EXIT;
    }


	unsigned long AlarmPlanAssocHelper::getEntityKey()
    {
        // We don't want to call reload() if this is a new alarm plan association and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing alarm plan association and the data needs to be loaded
            reload();
        }

        return m_entityKey;
    }


    void AlarmPlanAssocHelper::setEntityKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setEntityKey");

        if (!m_isValidData && !m_isNew)  
        {            
            reload();
        }

        m_entityKey = entityKey;

        FUNCTION_EXIT;
    }


    unsigned long AlarmPlanAssocHelper::getEntityTypeKey()
    {
        // We don't want to call reload() if this is a new alarm plan association and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing alarm plan association and the data needs to be loaded
            reload();
        }

        return m_entityTypeKey;
    }


	void AlarmPlanAssocHelper::setEntityTypeKey(unsigned long entityTypeKey)
    {
        FUNCTION_ENTRY("setEntityTypeKey");

        if (!m_isValidData && !m_isNew)  
        {            
            reload();
        }

        m_entityTypeKey = entityTypeKey;

        FUNCTION_EXIT;
    }

    
    std::string AlarmPlanAssocHelper::getPlanPath()
    {
        if (!m_isValidData && !m_isNew)  
        {            
            reload();
        }

        return m_planPath;
    }

        
	void AlarmPlanAssocHelper::setPlanPath(const std::string& planPath)
    {
        FUNCTION_ENTRY("setPlanPath");

        if (!m_isValidData && !m_isNew)  
        {            
            reload();
        }

        // Create a Plan object based on the specified path. This will ensure the path is valid.
        std::auto_ptr<IPlan> plan(PlanAccessFactory::getInstance().getPlanByPath(planPath));
        m_planPath = plan->getPath();

        FUNCTION_EXIT;
    }


    time_t AlarmPlanAssocHelper::getDateCreated()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date created can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t AlarmPlanAssocHelper::getDateModified()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateModified;
    }


    void AlarmPlanAssocHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new alarm plan association");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.

        std::vector<std::string> fieldNames;
        if (m_alarmType == 0)
        {
            fieldNames.push_back("Alarm Type");
        }
        if (m_entityKey == 0 && m_entityTypeKey == 0)
        {
            fieldNames.push_back("Entity or Entity Type");
        }
        if (m_planPath.empty())
        {
            fieldNames.push_back("Plan");
        }

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("Alarm Plan Association data not fully specified. Alarm Plan Association cannot be written to database" ,fieldNames) );
        }

        // Write the alarm plan association to the database

        // Need to check if this is new or existing - 
        // as a new item will be inserted, while an existing item will be updated
        if (m_isNew)
        {
            addNew();
        }
        else 
        {
            modifyExisting();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        /*std::ostringstream sql;
        sql << "select PKEY, ENTITYTYPEKEY, ENTITYKEY, PLANPATH, ALARMTYPEKEY, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from ALARMPLANMAPS WHERE pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_Oracle_SELECT_2052, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_SELECT_2052, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMPLANMAPS_SELECT_2052, m_key);


        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "PKEY";
        std::string entityTypeColumn = "ENTITY_TYPE";
        std::string entityColumn = "ENTITY";
        std::string planColumn = "PLAN";
        std::string alarmColumn = "ALARM";
        std::string dateModifiedColumn = "DATE_MODIFIED";
        std::string dateCreatedColumn = "DATE_CREATED";

        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(entityTypeColumn);
        columnNames.push_back(entityColumn);
        columnNames.push_back(planColumn);
        columnNames.push_back(alarmColumn);
        columnNames.push_back(dateModifiedColumn);
        columnNames.push_back(dateCreatedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( strSql, columnNames );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "No alarm plan association found for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "More than one alarm plan association found for pkey = " << m_key;
			TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key = data->getUnsignedLongData( 0, keyColumn );
        m_entityKey = data->getUnsignedLongData( 0, entityColumn );
        m_entityTypeKey = data->getUnsignedLongData( 0, entityTypeColumn );
        m_alarmType = data->getUnsignedLongData( 0, alarmColumn );
        m_planPath = data->getStringData( 0, planColumn );
        m_dateCreated = data->getDateData( 0, dateCreatedColumn);
        m_dateModified = data->getDateData(0,dateModifiedColumn,0);
 
        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void AlarmPlanAssocHelper::deleteThisAssociation()
    {
        FUNCTION_ENTRY("deleteThisAssociation");

        TA_ASSERT(!m_isNew, "This location does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // We must now attempt to delete the location

        /*std::ostringstream sql;
        sql << "delete ALARMPLANMAPS where pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_STD_DELETE_2401, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_DELETE_2401, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMPLANMAPS_DELETE_2401, m_key);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocHelper::addNew()
    {
        FUNCTION_ENTRY("addNew");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Alarm Plan Association %lu is a new association. It will be added to the database.",
                m_key);

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // We must retrieve the key first so we know where we are inserting the data. No other fields
        // are unique so this is our only identifier.
        /*std::string sql ("select ALARMPLANMAPS_SEQ.NEXTVAL from DUAL");*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_Oracle_SELECT_2053);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_SELECT_2053);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMPLANMAPS_SELECT_2053);

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // Name already exists
        {
            delete data;
            data = NULL;

            TA_THROW( DataException("Could not retrieve a new key for the alarm plan association",DataException::NO_VALUE,"pkey") );
        }


        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the entity (reuse the string)
        /*std::ostringstream formatSQL;
        formatSQL << "insert into ALARMPLANMAPS (PKEY,ENTITYTYPEKEY,ENTITYKEY,ALARMTYPEKEY,PLANPATH) ";
        formatSQL << "values (" << data->getUnsignedLongData(0, keyColumn) << ",";
        formatSQL << m_entityTypeKey << "," << m_entityKey << "," << m_alarmType << ",";
        formatSQL << "'" << databaseConnection->escapeInsertString(m_planPath) << "')";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_STD_INSERT_2301, data->getUnsignedLongData(0, keyColumn),
//		strSql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_INSERT_2301, data->getUnsignedLongData(0, keyColumn),

		databaseConnection->prepareSQLStatement(strSql, ALARMPLANMAPS_INSERT_2301, data->getUnsignedLongData(0, keyColumn),
			m_entityTypeKey, m_entityKey, m_alarmType, databaseConnection->escapeInsertString(m_planPath) );

        // Now the insert has been successful we can store the key
        m_key = data->getUnsignedLongData(0, keyColumn);

        // delete the IData pointer now that it's finished with
        delete data;
        data = NULL;
 
        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
       /* formatSQL.str("");;
		formatSQL << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from ALARMPLANMAPS where PKEY = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_Oracle_SELECT_2054, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_SELECT_2054, m_key);

		databaseConnection->prepareSQLStatement(strSql, ALARMPLANMAPS_SELECT_2054, m_key);

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for key = " << m_key;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for key = " << m_key;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
        }

        // Retrieve the date created from the
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Alarm Plan Association %lu already exists. It's details will be updated.", m_key );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        /*std::ostringstream sql;
        sql << "update ALARMPLANMAPS set ENTITYKEY = " << m_entityKey;
        sql << ", ENTITYTYPEKEY = " << m_entityTypeKey;
        sql << ", ALARMTYPEKEY = " << m_alarmType;
        sql << ", PLANPATH = '" << databaseConnection->escapeInsertString(m_planPath) << "'";
        sql << " where pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_STD_UPDATE_2151, m_entityKey, m_entityTypeKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_UPDATE_2151, m_entityKey, m_entityTypeKey,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMPLANMAPS_UPDATE_2151, m_entityKey, m_entityTypeKey,
			m_alarmType, databaseConnection->escapeInsertString(m_planPath), m_key);

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from ALARMPLANMAPS where PKEY = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_Oracle_SELECT_2055, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_SELECT_2055, m_key);

		databaseConnection->prepareSQLStatement(strSql, ALARMPLANMAPS_SELECT_2055, m_key);

        // Can use the same columnNames vector from above.
        std::vector<std::string> columnNames;
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for key = " << m_key;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for key = " << m_key;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
            m_dateModified = data->getDateData(0,modifiedColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    std::string AlarmPlanAssocHelper::retrieveAlarmTypeName(unsigned long alarmTypeKey)
    {
        FUNCTION_ENTRY("retrieveAlarmTypeName");


        IAlarmTypeData* data = NULL;
        std::string name = "";
        try
        {
            data = AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey( alarmTypeKey,false );
            name = data->getName();
        }
        catch( const DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the entity name");
            name = "";
        }
        catch( const DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the entity name");
            name = "";
        }

        if (data != NULL)
        {
            delete data;
            data = NULL;
        }

        FUNCTION_EXIT;
        return name;
    }
     
	 
	std::string AlarmPlanAssocHelper::retrieveAlarmTypeDispalyName(unsigned long alarmTypeKey)
	{
		FUNCTION_ENTRY("retrieveAlarmTypeName");


		IAlarmTypeData* data = NULL;
		std::string name = "";
		try
		{
			data = AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey( alarmTypeKey,false );
			name = data->getDisplayName();
		}
		catch( const DataException& )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the entity name");
			name = "";
		}
		catch( const DatabaseException& )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the entity name");
			name = "";
		}

		if (data != NULL)
		{
			delete data;
			data = NULL;
		}

		FUNCTION_EXIT;
		return name;
	}
    
    std::string AlarmPlanAssocHelper::retrieveEntityName(unsigned long entityKey)
    {
        FUNCTION_ENTRY("retrieveEntityName");

        if (entityKey == 0)
        {
            FUNCTION_EXIT;
            return "";
        }

        IEntityData* data = NULL;
        std::string name = "";
        try
        {
            data = EntityAccessFactory::getInstance().getEntity(entityKey);
            name = data->getName();
        }
        catch( const DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the entity name");
            name = "";
        }
        catch( const DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the entity name");
            name = "";
        }

        if (data != NULL)
        {
            delete data;
            data = NULL;
        }

        FUNCTION_EXIT;
        return name;
    }
     
        
    std::string AlarmPlanAssocHelper::retrieveEntityTypeName(unsigned long entityTypeKey)
    {
        FUNCTION_ENTRY("retrieveEntityTypeName");

        if (entityTypeKey == 0)
        {
            FUNCTION_EXIT;
            return "";
        }

        std::string name = "";
        IData* data = NULL;

        try
        {
			IDatabase* pDb = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
            /*std::ostringstream sql; 
            sql << "select name from entitytype where pkey = " << entityTypeKey;*/
//			std::string strSql = defPrepareSQLStatement(pDb->GetDbServerType(), Entitytype_STD_SELECT_4020, entityTypeKey);
//			std::string strSql  = pDb->prepareSQLStatement(ENTITYTYPE_SELECT_4020, entityTypeKey);
		SQLStatement strSql;
		pDb->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4020, entityTypeKey);

            std::vector<std::string> columnNames;
            columnNames.push_back("Name");

            data = pDb->executeQuery(strSql, columnNames);

            if (data->getNumRows() != 1)
            {
                name = "";
            }
            else
            {
                name = data->getStringData( 0, "Name" );
            }
        }
        catch( const DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the entity name");
            name = "";
        }
        catch( const DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the entity name");
            name = "";
        }

        if (data != NULL)
        {
            delete data;
            data = NULL;
        }

        FUNCTION_EXIT;
        return name;
    }
     
} // closes TA_Base_Core


