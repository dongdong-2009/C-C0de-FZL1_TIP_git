/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/SubsystemHelper.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 09:59:59 $
  * Last modified by:  $Author: Ouyang $
  * 
  * SubsystemHelper is used by ConfigSubsystem and Subsystem to implement
  * the actual database code that those classes provide the interface to.
  *
  */


#ifdef __WIN32__
#pragma warning(disable: 4786)
#endif


#include <sstream>

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/Subsystem.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SubsystemHelper.h"
#include "core/data_access_interface/src/SQLCode.h"

using TA_Base_Core::DebugUtil;



namespace TA_Base_Core
{

    SubsystemHelper::SubsystemHelper()
        :
        m_key(0),
        m_name(""),
        m_isExclusive(false),
        m_isPhysical(true),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isValidData(false),
        m_isNew(true),
		m_displayName(""),
		m_isLocationExclusive(false)
    {
    }


    SubsystemHelper::SubsystemHelper(const unsigned long key)
        :
        m_key(key),
        m_name(""),
        m_isExclusive(false),
        m_isPhysical(false),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isValidData(false),
		m_isNew(false),
		m_displayName(""),
		m_isLocationExclusive(false)
    {
    }


    SubsystemHelper::SubsystemHelper(const unsigned long row, TA_Base_Core::IData& data)
		:   
        m_isNew(false),
        m_isValidData(false)
    {
        FUNCTION_ENTRY("SubsystemHelper(const unsigned long, TA_Base_Core::IData&)");        

        reloadUsing(row, data);
        FUNCTION_EXIT;
    }


    SubsystemHelper::SubsystemHelper(const SubsystemHelper& theSubsystemHelperToCopy) :
        m_key(0),
        m_name(theSubsystemHelperToCopy.m_name),
        m_isExclusive(theSubsystemHelperToCopy.m_isExclusive),
        m_isPhysical(theSubsystemHelperToCopy.m_isPhysical),
        m_dateModified(theSubsystemHelperToCopy.m_dateModified),
		m_dateCreated( theSubsystemHelperToCopy.m_dateCreated),
        m_isValidData( theSubsystemHelperToCopy.m_isValidData),
		m_isNew(true),
		m_displayName(theSubsystemHelperToCopy.m_displayName),
		m_isLocationExclusive(theSubsystemHelperToCopy.m_isLocationExclusive)
    {
        TA_ASSERT(theSubsystemHelperToCopy.m_isPhysical, "Only physical subsystems may be copied");
    }
    

    SubsystemHelper::~SubsystemHelper()
    {
    }


    bool SubsystemHelper::isNew() const
    {
        return m_isNew;
    }


    bool SubsystemHelper::canDelete()
    {
        return isPhysical();
    }


    unsigned long SubsystemHelper::getKey()
    {
        TA_ASSERT( ! m_isNew,
                  "Cannot retrieve the key on a subsystem before it "
                  "has been applied to the database");

        return m_key;
    }

	unsigned long SubsystemHelper::getSystemKey()
	{
		TA_ASSERT( ! m_isNew,
			"Cannot retrieve the key on a subsystem before it "
			"has been applied to the database");

		return m_systemKey;
	}

    std::string SubsystemHelper::getName()
    {
        // We don't want to call reload() if this is a new subsystem
        // and it has not been written to the database
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_name;
    }


	std::string SubsystemHelper::getDisplayName()
	{
		// We don't want to call reload() if this is a new subsystem
		// and it has not been written to the database
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		return m_displayName;
	}
    bool SubsystemHelper::isPhysical()
    {
        // We don't want to call reload() if this is a new subsystem
        // and it has not been written to the database
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_isPhysical;
    }


    bool SubsystemHelper::isExclusive()
    {
        // We don't want to call reload() if this is a new subsystem
        // and it has not been written to the database
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_isExclusive;
    }
	bool SubsystemHelper::isLocationExclusive()
    {
        // We don't want to call reload() if this is a new subsystem
        // and it has not been written to the database
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_isLocationExclusive;
    }


    time_t SubsystemHelper::getDateCreated()
    {
        TA_ASSERT( ! m_isNew,
                  "The data must be written to the database before the "
                  "date created can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t SubsystemHelper::getDateModified()
    {
        TA_ASSERT( ! m_isNew,
                  "The data must be written to the database before the "
                  "date modified can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateModified;
    }

    
    void SubsystemHelper::setExclusiveControlStatus(bool isExclusive)
    {
        TA_ASSERT(m_isPhysical, "Only physical subsystems may be modified.");

        // We don't want to call reload() if this is a new subsystem
        // and it has not been written to the database
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isExclusive = isExclusive;
    }

	void SubsystemHelper::setLocationExclusiveControlStatus(bool isExclusive)
    {
        TA_ASSERT(m_isPhysical, "Only physical subsystems may be modified.");

        // We don't want to call reload() if this is a new subsystem
        // and it has not been written to the database
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isLocationExclusive = isExclusive;
    }
        
    void SubsystemHelper::setName(const std::string& name)
    {
        TA_ASSERT(m_isPhysical, "Only physical subsystems may be modified.");

        // We don't want to call reload() if this is a new subsystem
        // and it has not been written to the database
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;
    }


	void SubsystemHelper::setDisplayName(const std::string& displayName)
	{
		TA_ASSERT(m_isPhysical, "Only physical subsystems may be modified.");

		// We don't want to call reload() if this is a new subsystem
		// and it has not been written to the database
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_displayName = displayName;
	}


    
    void SubsystemHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        m_isValidData = false;
        FUNCTION_EXIT;
    }
    

    void SubsystemHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty,
        // and add each failure to a vector of failed field names.

        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }

        // Now, if we have at least one entry in the vector,
        // something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("Subsystem data not fully specified. Subsystem cannot be written to database", fieldNames) );
        }

        // Write the Subsystem to the database

        // Need to check if this is a new entity or an existing entity - 
        // as a new entity will be inserted, while an existing entity will
        // be updated
        if (m_isNew)
        {
             // This is a new entity
            addNewSubsystem();
        }
        else 
        {
            // This is an existing entity
            modifyExistingSubsystem();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew       = false;

        FUNCTION_EXIT;
    }


    void SubsystemHelper::modifyExistingSubsystem()
    {
        FUNCTION_ENTRY("modifyExistingSubsystem");
    
        LOG (SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Subsystem %s already exists. Its details will be updated.",
             m_name.c_str());

        // Get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Create a select statement that will ensure the uniqueness of
        // the entity name.
       /* std::string formatSQL = "select PKEY from subsystem where NAME = '"
							  + databaseConnection->escapeQueryString(m_name) + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_STD_SELECT_18003,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18003,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18003,
			databaseConnection->escapeQueryString(m_name) );

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // Execute the query
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        // Check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A subsystem already exists with name = " << m_name.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
            }
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one subsystem already exists name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        delete data;
        data =  NULL;

        /*std::ostringstream sql;
        sql << " update SUBSYSTEM set NAME = '" << (databaseConnection->escapeQueryString(m_name))
			<< "', EXCLUSIVE_CONTROL = " << m_isExclusive
			<< " where pkey = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_STD_UPDATE_18151, 
//		strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_UPDATE_18151, 

		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_UPDATE_18151,
			databaseConnection->escapeQueryString(m_name), (databaseConnection->escapeQueryString(m_displayName)), m_isExclusive, m_key);

        databaseConnection->executeModification(strSql);

        // Now need to update the date fields
       /* sql.str("");
        sql << " select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'),TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS')"
            << " from SUBSYSTEM where PKEY = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_Oracle_SELECT_18055, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18055, m_key);

		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18055, m_key);

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object
        // when we're done with it.
        data = databaseConnection->executeQuery(strSql, columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }

        try
        {
            m_dateCreated  = data->getDateData(0, createdColumn);
            m_dateModified = data->getDateData(0, modifiedColumn);
		}
		//exception
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

    
    void SubsystemHelper::addNewSubsystem()
    {
        FUNCTION_ENTRY("addNewSubsystem");

        LOG (SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Subsystem %s is a new subsystem. "
             "It will be added to the database.",
             m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // First check there is not already a subsytem with this name
        // (it is a uniqueness constraint).
        /*std::ostringstream formatSQL;
        formatSQL << "select PKEY from SUBSYSTEM where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_STD_SELECT_18003,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18003,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18003,
			databaseConnection->escapeQueryString(m_name) );


        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Check the number of rows returned (the actual data is not important)
        // - it should be 0
        if (0 != data->getNumRows()) // Name already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage <<
                "A subsystem already exists with name = " << m_name.c_str();

            TA_THROW( DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "name") );
        }

        // delete the IData pointer now that it's finished with
        delete data;
        data = NULL;

        // Now we can get on with writing the subsystem to the database.
        // generate the SQL string to write the subsystem (reuse the string)
        // Note that IS_PHYSICAL must be "1" for new Subsystems
        /*formatSQL.str("");
        formatSQL <<
            "insert into SUBSYSTEM " <<
            "(PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL) " <<
            "values " <<
            "(" <<
                "SUBSYSTEM_SEQ.NEXTVAL, "                                      <<
		        "'" << databaseConnection->escapeInsertString(m_name) << "', " <<
                "1, "                                                          <<
                m_isExclusive                                                  <<
            ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_Oracle_INSERT_18351, 
//		strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_INSERT_18351, 

		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_INSERT_18351,
            databaseConnection->getNextPkey("SUBSYSTEM_SEQ"),
			databaseConnection->escapeInsertString(m_name), databaseConnection->escapeInsertString(m_displayName), m_isExclusive);

        databaseConnection->executeModification(strSql);


        // Now need to get the PKEY back out for this subsystem
        // and update the date created field
        /*std::ostringstream sql;
		sql << " select PKEY, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from SUBSYSTEM "
			<< " where NAME = '" << databaseConnection->escapeQueryString(m_name) << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_Oracle_SELECT_18056,
//		strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18056,

		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18056,
			databaseConnection->escapeQueryString(m_name) );

        // Can use the same columnNames vector from above.
        // Key is already in it so we can just add the date created field
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object
        // when we're done with it
        data = databaseConnection->executeQuery(strSql, columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
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
        // returned data. There should only be a single returned item
        // (as any other circumstance is specifically prevented by the above
        // checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0, keyColumn);
            m_dateCreated = data->getDateData(0, createdColumn);
		}
		//exception
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


    void SubsystemHelper::deleteThisSubsystem()
    {
        FUNCTION_ENTRY("deleteThisSubsystem");

        TA_ASSERT( ! m_isNew,
                  "This subsystem does not yet exist in the database, "
                  "and therefore cannot be deleted");

        // Subsystems which are not physical cannot be deleted
        if( ! m_isPhysical )
        {
            std::string message = "Non-physical subsystems cannot be deleted.";
            DataException ex(message.c_str(), DataException::CANNOT_DELETE, "");
            TA_THROW(ex);
        }


        // Check that there are no foreign key references from the
        // - entity table
        // - entitytype table
        // - se_profile_access table
        // - se_profile_delegate table
        // to this row.
        // These methods all throw a DataException if there is a foreign key
        // reference to this row.
        checkForReferencesFromEntityTable();
        checkForReferencesFromEntityTypeTable();
        checkForReferencesFromProfileTables();

        // Get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        

        // We must now attempt to delete the subsystem
        /*std::ostringstream sql;
        sql << "delete subsystem where pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_STD_DELETE_18401, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_DELETE_18401, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_DELETE_18401, m_key);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void SubsystemHelper::checkForReferencesFromEntityTable()
    {
        // Get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        

        // Find out if there are any rows in the entity table that reference
        // this row.
        /*std::ostringstream cascadeSql;
        cascadeSql
            << "select name from entity where physical_subsystem_key = "
            << m_key << " or subsystemkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4508, m_key, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4508, m_key, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4508, m_key, m_key);


        std::string nameColumn = "NAME";

        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        IData* data =
            databaseConnection->executeQuery(strSql, columnNames);


        // If there are rows in the entity table that reference this row,
        // throw an exception.
        if (0 != data->getNumRows())
        {
            try
            {
                std::string message = "Cannot delete Subsystem that is "
                                      "referenced by entity";

                // Get the names of the entities that reference this row.
                std::string entities = "Entities: ";
                entities            += getCommaSeparatedList(data, nameColumn);

                TA_THROW( DataException(message.c_str(),
                                        DataException::CANNOT_DELETE,
                                        entities) );
			}
			//exception
            catch(...)
            {
                delete data;
                data = NULL;

                throw;
            }
        }

        delete data;
        data = NULL;
    }


    void SubsystemHelper::checkForReferencesFromEntityTypeTable()
    {
        // Get a connection to the database
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        

        // Find out if there are any rows in the entity type table that
        // reference this row.
        /*std::ostringstream cascadeSql;
        cascadeSql
            << "select name from entitytype where ta_subsystemkey = "
            << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4018, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4018, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4018, m_key);


        std::string nameColumn = "NAME";

        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        IData* data =
            databaseConnection->executeQuery(strSql, columnNames);


        // If there are rows in the entity type table that reference this row,
        // throw an exception.
        if (0 != data->getNumRows())
        {
            try
            {
                std::string message = "Cannot delete Subsystem that is "
                                      "referenced by entity type";

                // Get the names of the entity type that reference this row.
                std::string types = "Entity types: ";
                types            += getCommaSeparatedList(data, nameColumn);
            
                TA_THROW( DataException(message.c_str(),
                                        DataException::CANNOT_DELETE,
                                        types) );
			}
			//exception
            catch(...)
            {
                delete data;
                data = NULL;

                throw;
            }
        }

        delete data;
        data = NULL;
    }


    void SubsystemHelper::checkForReferencesFromProfileTables()
    {
        // Get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        

        // Find out if there are any rows in the profile access or
        // profile delegate table that reference this row.
        /*std::ostringstream cascadeSql;
        cascadeSql
            << "select unique P.name from se_profile P where "
            << "P.seprof_id in "
            <<     "(select A.seprof_id from se_profile_access A "
            <<     "where A.subsystemkey = " << m_key << ") ";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_STD_SELECT_7506, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_SELECT_7506, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_SELECT_7506, m_key);

            // TD 11419: SE_PROFILE_DELEGATE table no longer exists
            //<< "or "
            //<< "P.seprof_id in "
            //<<     "(select D.seprof_id from se_profile_delegate D "
            //<<     "where D.subsystemkey = " << m_key << ")";


        std::string nameColumn = "NAME";

        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        IData* data =
            databaseConnection->executeQuery(strSql, columnNames);


        // If there are rows in the profile tables that reference this
        // row, throw an exception.
        if (0 != data->getNumRows())
        {
            try
            {
                std::string message = "Cannot delete Subsystem that is "
                                      "referenced by profile";

                // Get the names of the profiles that reference this row.
                std::string profiles = "Profiles: ";
                profiles            += getCommaSeparatedList(data, nameColumn);

                TA_THROW( DataException(message.c_str(),
                                        DataException::CANNOT_DELETE,
                                        profiles) );
			}
			//exception
            catch(...)
            {
                delete data;
                data = NULL;

                throw;
            }
        }

        delete data;
        data = NULL;
    }



    std::string SubsystemHelper::getCommaSeparatedList(IData* data, const std::string&  columnName)
    {
        TA_ASSERT(data != NULL, "Data must not be NULL");

        // Get the values from the data object
        std::string list = "";
        for(unsigned int i = 0; i < data->getNumRows(); i ++)
        {
            if(i != 0)
            {
                list += ", ";
            }

            list += data->getStringData(i, columnName);
        }

        return list;
    }

	void SubsystemHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
		columnNames.push_back("DISPLAY_NAME");
        columnNames.push_back("IS_PHYSICAL");
        columnNames.push_back("EXCLUSIVE_CONTROL");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		columnNames.push_back("SYSTEM_KEY");
	}	

    void SubsystemHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        /*std::ostringstream sql;
        sql << " select PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'),SYSTEM_KEY from SUBSYSTEM where PKEY = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_Oracle_SELECT_18057, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18057, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18057, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when
        // we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery( strSql, columnNames ));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
            reasonMessage << "No subsystem found with pkey = " << m_key;
            TA_THROW( DataException( reasonMessage.str().c_str(), DataException::NO_VALUE,"" ) );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of subsystem records."); 
        reloadUsing(0, *data);		

        FUNCTION_EXIT;
    }

	void SubsystemHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in
        // the comment of this method.
		try
		{
			m_key			= data.getUnsignedLongData( row, columnNames[0] );
			m_name			= data.getStringData(  row, columnNames[1] );
		m_displayName	= data.getStringData(  row, columnNames[2] );
        m_isPhysical	= data.getBooleanData( row, columnNames[3]);
        m_isExclusive	= data.getBooleanData( row, columnNames[4]);

        m_dateModified = data.getDateData(row, columnNames[5], 0);
        m_dateCreated	= data.getDateData( row, columnNames[6]);
			m_systemKey = data.getUnsignedLongData( row, columnNames[7]);

			m_isValidData = true;
		}
		//data exception
		catch ( const TA_Base_Core::DataException& exception)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not apply the item changes");

			std::string errorStr;
			switch(exception.getFailType())
			{
			case (TA_Base_Core::DataException::WRONG_TYPE):
				{
					errorStr = "Wrong Type of Data. ";
					break;
				} 

			case (TA_Base_Core::DataException::NO_VALUE):
				{
					errorStr = "No Value of Data. ";
					break;
				}

			case (TA_Base_Core::DataException::NOT_UNIQUE):
				{
					errorStr = "Not Unique of Data. ";
					break;
				}

			case (TA_Base_Core::DataException::MISSING_MANDATORY):
				{
					errorStr = "Missing Mandatory of Data. ";
					break;
				}

			case (TA_Base_Core::DataException::INVALID_VALUE):
				{
					errorStr = "Invalid Value of Data. ";
					break;
				}

			default:
				{
					errorStr = "Unknown error of Data. ";
					break;
				}
			}

			//CString actionName, reason;
			//actionName.LoadString(IDS_APPLY);
			//reason.LoadString(errorId);
			//reason += exception.getWhichData().c_str();
			//TA_Base_Bus::TransActiveMessage userMsg;
			//userMsg << actionName << reason;
			//userMsg.showMsgBox(IDS_UE_030060);
			std::string reason = errorStr;
			reason += exception.getWhichData();
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Error happen when get data for subsystem, error detail: %s", 
				reason.c_str());
		}
		//exception
		catch( ... )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Error happen when get data for subsystem, unknown error.");
		}

        FUNCTION_EXIT;
    }		    
} // closes TA_Base_Core

