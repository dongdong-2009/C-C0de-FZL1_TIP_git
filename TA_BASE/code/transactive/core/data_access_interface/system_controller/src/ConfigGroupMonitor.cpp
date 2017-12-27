/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ConfigGroupMonitor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigGroupMonitor is an implementation of IConfigGroupMonitor. It holds the data specific to a group entry
  * in the database, and allows read-write access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/system_controller/src/ConfigGroupMonitor.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    long ConfigGroupMonitor::s_nextAvailableIdentifier = 0;

    const std::string ConfigGroupMonitor::NAME = "Name";
    const std::string ConfigGroupMonitor::DESCRIPTION = "Description";


    ConfigGroupMonitor::ConfigGroupMonitor()
        : m_uniqueIdentifier(s_nextAvailableIdentifier++),
          m_key(0),
          m_name(""),
          m_description(""), 
          m_dateCreated(0),
          m_dateModified(0), 
          m_isNew(true),
          m_isValidData(false)
    {
    }


	ConfigGroupMonitor::ConfigGroupMonitor( const ConfigGroupMonitor& theGroup)
        : m_uniqueIdentifier(s_nextAvailableIdentifier++),
          m_key(theGroup.m_key),
          m_name(theGroup.m_name),
          m_description(theGroup.m_description), 
          m_dateCreated(0),
          m_dateModified(0), 
          m_isNew(true),
          m_isValidData(false)
	{
	}


	ConfigGroupMonitor::ConfigGroupMonitor( const unsigned long key, const std::string& name, const std::string& description,
                                    const time_t dateCreated, const time_t dateModified)
		: m_uniqueIdentifier(s_nextAvailableIdentifier++),
          m_key(key),
          m_name(name),
          m_description(description), 
          m_dateCreated(dateCreated),
          m_dateModified(dateModified), 
          m_isNew(false),
          m_isValidData(true)
	{
	}


    ConfigGroupMonitor::~ConfigGroupMonitor()
    {
    }


    unsigned long ConfigGroupMonitor::getKey()
    {
        return m_key;
    }


    std::string ConfigGroupMonitor::getName()
    {
        // We don't want to call reload() if this is a new group and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing group and the data needs to be loaded
            reload();
        }
        return m_name;
    }


	std::string ConfigGroupMonitor::getDescription()
	{
        // We don't want to call reload() if this is a new group and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing group and the data needs to be loaded
            reload();
        }
        return m_description;
	}


    time_t ConfigGroupMonitor::getDateCreated()
    {
        // We don't want to call reload() if this is a new group and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing group and the data needs to be loaded
            reload();
        }
        return m_dateCreated;
    }


    time_t ConfigGroupMonitor::getDateModified()
    {
        // We don't want to call reload() if this is a new group and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing group and the data needs to be loaded
            reload();
        }
        return m_dateModified;
    }


    void ConfigGroupMonitor::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        updateGroupChanges(NAME,m_name, name);

        m_name = name;
        FUNCTION_EXIT;
    }


    void ConfigGroupMonitor::setDescription(const std::string& description)
    {
        FUNCTION_ENTRY("setDescription");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        updateGroupChanges(DESCRIPTION,m_description, description);

        m_description = description;
        FUNCTION_EXIT;
    }

    
    void ConfigGroupMonitor::invalidate()
    {
        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new group");

        m_isValidData = false;
        m_groupChanges.clear();
    }


    void ConfigGroupMonitor::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.

        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }
        if (m_description.empty())
        {
            fieldNames.push_back("Description");
        }
        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("Monitor Group data not fully specified. Monitor Group cannot be written to database" ,fieldNames) );
        }

        // Write the SC Group to the database

        // Need to check if this is a new group or an existing group - 
        // as a new group will be inserted, while an existing group will be updated
        if (m_isNew)
        {
             // This is a new group
            addNewGroup();
        }
        else 
        {
            // This is an existing group
            modifyExistingGroup();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;
        m_groupChanges.clear();

        FUNCTION_EXIT;
      }

    
    void ConfigGroupMonitor::deleteThisGroup()
    {
        FUNCTION_ENTRY("deleteThisGroup");

        TA_ASSERT(!m_isNew, "This monitor group does not yet exist in the database, and therefore cannot be deleted");
        
        //Check not linked to any system controllers. This will throw an exception if it finds any
        checkForLinkedSystemControllers();

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_GROUP_MONITOR_STD_SELECT_22501, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_GROUP_MONITOR_DELETE_22501, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_GROUP_MONITOR_DELETE_22501, m_key);
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::deleteThisGroup()");

        // First delete any entries where this group is monitored
        /*std::ostringstream sql;
        sql << "delete SN_GROUP_MONITOR where SNSCGR_ID = " << m_key;*/

        databaseConnection->executeModification(strSql);

        // We must now attempt to delete the SC group
        /*sql.str("");
        sql << "delete SN_SYSTEM_CONTROLLER_GROUP where SNSCGR_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_STD_SELECT_23001, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_DELETE_23001, m_key);

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_DELETE_23001, m_key);
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::deleteThisGroup()");

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void ConfigGroupMonitor::updateGroupChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateGroupChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_groupChanges.find(name);

        if (matching != m_groupChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_groupChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_groupChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


    void ConfigGroupMonitor::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_Oracle_SELECT_23051, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_SELECT_23051, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_SELECT_23051, m_key);
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::reload(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "select SNSCGR_ID, NAME, DESCRIPTION, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SN_SYSTEM_CONTROLLER_GROUP WHERE SNSCGR_ID = " << m_key;*/


        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "PKEY";
        std::string nameColumn = "NAME";
        std::string descriptionColumn = "DESCRIPTION";
        std::string dateModifiedColumn = "DATE_MODIFIED";
        std::string dateCreatedColumn = "DATE_CREATED";

        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(descriptionColumn);
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
            reasonMessage << "No group found for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "More than one group found for pkey = " << m_key;
			TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key = data->getUnsignedLongData( 0, keyColumn );
        m_name = data->getStringData( 0, nameColumn );
        m_description = data->getStringData( 0, descriptionColumn );
        m_dateCreated = data->getDateData( 0, dateCreatedColumn);
        m_dateModified = data->getDateData(0,dateModifiedColumn, 0);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void ConfigGroupMonitor::addNewGroup()
    {
        FUNCTION_ENTRY("addNewGroup");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Monitor Group %s is a new group. It will be added to the database.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_STD_SELECT_23002,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_SELECT_23002,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_SELECT_23002,
			databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::addNewGroup(): Prepare SQL Statement error");
        
        // First check there is not alreay an entity with this name (it is a uniqueness
        // constraint).
        /*std::ostringstream formatSQL;
        formatSQL << "select SNSCGR_ID from SN_SYSTEM_CONTROLLER_GROUP where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";*/

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A group monitor already exists with name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the entity (reuse the string)
        /*formatSQL.str("");
        formatSQL << "insert into SN_SYSTEM_CONTROLLER_GROUP (SNSCGR_ID,NAME,DESCRIPTION) ";
        formatSQL << "values (SYSTEM_CONTROL_SEQ.NEXTVAL,'";
		formatSQL << databaseConnection->escapeInsertString(m_name) << "','";
		formatSQL << databaseConnection->escapeInsertString(m_description) << "'";
        formatSQL << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_Oracle_INSERT_23351,
//		strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_INSERT_23351,

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_INSERT_23351,
            databaseConnection->getNextPkey("SYSTEM_CONTROL_SEQ"),
			databaseConnection->escapeInsertString(m_name), databaseConnection->escapeInsertString(m_description));
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::addNewGroup(): Prepare SQL Statement error");
 
        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this SC group and update the date fields
        /*std::ostringstream sql;
		sql << "select SNSCGR_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from SN_SYSTEM_CONTROLLER_GROUP where NAME = '";
        sql << databaseConnection->escapeQueryString(m_name).c_str() << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_Oracle_SELECT_23052,
//		strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_SELECT_23052,

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_SELECT_23052,
			databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::addNewGroup(): Prepare SQL Statement error");

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
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
			reasonMessage << "No data found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,keyColumn);
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


    void ConfigGroupMonitor::modifyExistingGroup()
    {
        FUNCTION_ENTRY("modifyExistingGroup");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Monitor Group %s already exists. It's details will be updated.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_STD_SELECT_23002,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_SELECT_23002,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_SELECT_23002,
			databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::modifyExistingGroup(): Prepare SQL Statement error");

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::string formatSQL = "select SNSCGR_ID from SN_SYSTEM_CONTROLLER_GROUP where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "'";*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A monitor group already exists with name = " << m_name.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
            }
            // else - it matches, so that's okay - it means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one SC group already exists name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        delete data;
        data =  NULL;

        /*std::ostringstream sql;
        sql << "update SN_SYSTEM_CONTROLLER_GROUP set NAME = '" << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "', DESCRIPTION = '" << (databaseConnection->escapeQueryString(m_description)).c_str();
        sql << "' where SNSCGR_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_STD_UPDATE_23151,
//		strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_UPDATE_23151,

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_UPDATE_23151,
			databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_description),m_key);
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::modifyExistingGroup(): Prepare SQL Statement error");

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from SN_SYSTEM_CONTROLLER_GROUP where SNSCGR_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_Oracle_SELECT_23053, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_SELECT_23053, m_key);

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_SELECT_23053, m_key);
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::modifyExistingGroup(): Prepare SQL Statement error");

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

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
		    reasonMessage << "No data found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
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


    void ConfigGroupMonitor::checkForLinkedSystemControllers()
    {
        FUNCTION_ENTRY("checkForLinkedSystemControllers");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_SELECT_23501, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23501, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23501, m_key);
//		TA_ASSERT(strSql.size() > 0, "ConfigGroupMonitor::checkForLinkedSystemControllers(): Prepare SQL Statement error");

        // If there are any entities with this entity as the parent key or agent then we cannot delete
        // until they have been moved.l

        /*std::ostringstream sql;
        sql << "select NAME from SN_SYSTEM_CONTROLLER where SNSCGR_ID = " << m_key;*/

        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // We only want to select about 10 rows. Any more child entities than that and we don't 
        // want to list them.
        IData* data = databaseConnection->executeQuery(strSql,columnNames,11);

        TA_ASSERT( data != NULL, "The data returned was NULL");

        if (data->getNumRows() > 0)
        {
            std::string linkedSystemControllers("");
            // Only display the first ten items that we receive. If there are more then show
            // this using "...<more>".
            for( unsigned int i = 0; (i < data->getNumRows() && i < 10); ++i)
            {
                linkedSystemControllers += "System Controller ";
                linkedSystemControllers += data->getStringData(i,nameColumn);
                linkedSystemControllers += "\n";
            }

            if (data->getNumRows() > 10)
            {
                linkedSystemControllers += "...<more>\n";
            }

            delete data;

            TA_THROW( DataException("This group cannot be deleted as it has system controllers using it.",
                                    DataException::CANNOT_DELETE,
                                    linkedSystemControllers ) );
        }

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core
