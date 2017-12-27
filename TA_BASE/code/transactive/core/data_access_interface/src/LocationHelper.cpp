/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/LocationHelper.cpp $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * LocationHelper is an object that is held by all type-specific location objects, as well as by
  * the ConfigLocation. It contains all data common across locations, and manipulation
  * methods for the data. It helps avoid re-writing code for each location object.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include <vector>
#include <algorithm>

#include "core/data_access_interface/src/LocationHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    LocationHelper::LocationHelper( ) 
        : m_key(0),
          m_name(""),
          m_description(""), 
		  m_orderId(0),
		  m_displayName(""),
          m_dateCreated(0),
          m_dateModified(0), 
          m_isValidData(false),
          m_isValidProfileData(false),
          m_isNew(true),
		  m_displayOnly(false),
          m_typeName("")
    {
    }


    LocationHelper::LocationHelper( const unsigned long key )
        : m_key(key),
          m_name(""),
          m_description(""), 
		  m_orderId(0),
		  m_displayName(""),
          m_dateCreated(0),
          m_dateModified(0), 
          m_isValidData(false),
          m_isValidProfileData(false),
          m_isNew(false),
		  m_displayOnly(false),
          m_typeName("")
    {
    }
    

    LocationHelper::LocationHelper( const unsigned long key, const std::string& name, const std::string& description, const unsigned long orderId,
                                    const std::string displayname,const time_t dateCreated, const time_t dateModified, bool displayOnly, const std::string& typeName )
        : m_key(key),
          m_name(name),
          m_description(description),
		  m_orderId(orderId),
		  m_displayName(name),
          m_dateModified(dateModified), 
          m_dateCreated(dateCreated),
          m_isValidData(true),
          m_isValidProfileData(false),
          m_isNew(false),
		  m_displayOnly(displayOnly),
          m_typeName( typeName )
    {
    }


    LocationHelper::LocationHelper(const unsigned long row, TA_Base_Core::IData& data)
          :
          m_isValidData(false),
          m_isValidProfileData(false),
          m_isNew(false)
    {
        FUNCTION_ENTRY("LocationHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }


	LocationHelper::LocationHelper( const LocationHelper& theLocationHelperToCopy)
    {
        m_key = 0;
        m_name = theLocationHelperToCopy.m_name;
        m_description = theLocationHelperToCopy.m_description;
		m_orderId = theLocationHelperToCopy.m_orderId;
		m_displayName = theLocationHelperToCopy.m_displayName;
        m_dateModified = theLocationHelperToCopy.m_dateModified;
        m_dateCreated = theLocationHelperToCopy.m_dateCreated;
        m_isValidData = theLocationHelperToCopy.m_isValidData;
        m_isValidProfileData = theLocationHelperToCopy.m_isValidProfileData;
		m_displayOnly = theLocationHelperToCopy.m_displayOnly;
        m_typeName = theLocationHelperToCopy.m_typeName;
        m_isNew = true;
    }


    LocationHelper::~LocationHelper()
    {
    }


    unsigned long LocationHelper::getKey()
    {
        TA_ASSERT(!m_isNew, "Cannot retrieve the key on a location before it has been applied to the database");
        return m_key;
    }

    
    std::string LocationHelper::getName()
    {
        // We don't want to call reload() if this is a new location and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing location and the data needs to be loaded
            reload();
        }
        return m_name;
    }

	bool LocationHelper::isDisplayOnly()
	{
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing location and the data needs to be loaded
			reload();
		}
		return m_displayOnly;
	}
    
    void LocationHelper::setName(const std::string& name)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;
    }


    std::string LocationHelper::getDescription()
    {
        // We don't want to call reload() if this is a new location and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing location and the data needs to be loaded
            reload();
        }
        return m_description;
    }


    unsigned long LocationHelper::getOrderId()
    {
        // We don't want to call reload() if this is a new location and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing location and the data needs to be loaded
            reload();
        }
        return m_orderId;
    }

	std::string LocationHelper::getDisplayName()
	{
		// We don't want to call reload() if this is a new location and it has not been written to the database
		if (!m_isValidData && !m_isNew)  
		{            
			// This is an existing location and the data needs to be loaded
			reload();
		}
		return m_displayName;
	}

    void LocationHelper::setDescription(const std::string& description)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_description = description;
    }


    void LocationHelper::setOrderId(const unsigned long orderId)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_orderId = orderId;
    }
	void LocationHelper::setDisplayName(std::string dispalyname)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_displayName = dispalyname;
	}




	void LocationHelper::setDisplayOnly(bool displayOnly)
	{
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_displayOnly = displayOnly;
	}

    time_t LocationHelper::getDateCreated()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date created can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t LocationHelper::getDateModified()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateModified;
    }


    std::string LocationHelper::getDefaultDisplay(unsigned long profileKey, int displayNumber)
    {
        TA_ASSERT(!m_isNew, "This location must be applied to the database before the default display can be retrieved");

		// Look up the profile and location
		try
		{
			IProfile* profile = ProfileAccessFactory::getInstance().getProfile( profileKey );

			if ( NULL == profile )
			{
				return "";
			}

            std::string display = profile->getDefaultDisplay( m_key, displayNumber );

            delete profile;
            profile = NULL;

			return display;
		}
		catch ( ... )
		{
			// Any exception returns an empty string
			return "";
		}
    }

    bool LocationHelper::isProfileAssociated(unsigned long profileId)
    {
        FUNCTION_ENTRY("isProfileAssociated");
        if (!m_isValidProfileData && !m_isNew)
        {
            reloadProfileData();
        }

        std::vector<unsigned long>::iterator result = std::find(m_associatedProfileKeys.begin(),
            m_associatedProfileKeys.end(),profileId);

        FUNCTION_EXIT;
        return (result != m_associatedProfileKeys.end());
    }


    void LocationHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new entity");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    void LocationHelper::reloadProfileData()
    {
        FUNCTION_ENTRY("reloadProfileData");

        // Clear the current list of keys
        m_associatedProfileKeys.clear();

        // Can only load the associated profiles if this in not a "new" profile.
        if (!m_isNew)
        {
            // Get the associated profiles
            std::vector<IProfile*> profiles = ProfileAccessFactory::getInstance().getProfilesByLocation(m_key);

            for (unsigned int i = 0;i < profiles.size();i++)
            {
                m_associatedProfileKeys.push_back(profiles[i]->getKey());

                // Delete the IProfile object now it is no longer needed
                delete profiles[i];
                profiles[i] = NULL;
            }

            // Set the valid flag to true
            m_isValidProfileData = true;
        }

        FUNCTION_EXIT;
    }


    void LocationHelper::applyChanges()
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
        if (m_orderId == 0)
        {
            fieldNames.push_back("OrderId");
        }
		if (m_displayName.empty())
		{
			fieldNames.push_back("Display Name");
		}
        if (m_typeName.empty())
        {
            fieldNames.push_back("Type Name");
        }

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("Location data not fully specified. Location cannot be written to database" ,fieldNames) );
        }

        // Write the Location to the database

        // Need to check if this is a new entity or an existing entity - 
        // as a new entity will be inserted, while an existing entity will be updated
        if (m_isNew)
        {
             // This is a new entity
            addNewLocation();
        }
        else 
        {
            // This is an existing entity
            modifyExistingLocation();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


	void LocationHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("ORDER_ID");
		columnNames.push_back("DISPLAY_NAME");
        columnNames.push_back("DATE_MODIFIED");
		columnNames.push_back("DATE_CREATED");
        columnNames.push_back("IS_DISPLAY_ONLY");
        columnNames.push_back("TYPE_NAME");
	}

    void LocationHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        /*std::ostringstream sql;
        sql << "select PKEY, NAME, DESCRIPTION, ORDER_ID, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')),IS_DISPLAY_ONLY, TYPE_NAME FROM LOCATION WHERE PKEY = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_Oracle_SELECT_7057, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7057, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7057, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
            reasonMessage << "No location found for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of location records."); 
        reloadUsing(0, *data);

        FUNCTION_EXIT;
    }


	void LocationHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);
		
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key = data.getUnsignedLongData( row, columnNames[0] );
        m_name = data.getStringData( row, columnNames[1] );
        m_description = data.getStringData( row, columnNames[2] );
		m_orderId = data.getUnsignedLongData( row, columnNames[3] );
		m_displayName = data.getStringData(row, columnNames[4]);
        m_dateModified = data.getDateData(row, columnNames[5], 0);
        m_dateCreated = data.getDateData( row, columnNames[6]);
        m_displayOnly = data.getBooleanData(row , columnNames[7]);
        m_typeName = data.getStringData(row , columnNames[8]);
  
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void LocationHelper::deleteThisLocation()
    {
        FUNCTION_ENTRY("deleteThisLocation");

        TA_ASSERT(!m_isNew, "This location does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // We must now attempt to delete the location

        /*std::ostringstream sql;
        sql << "delete location where pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_STD_DELETE_7401, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_DELETE_7401, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_DELETE_7401, m_key);

        try
        {
            databaseConnection->executeModification(strSql);
        }
        catch(const DatabaseException& ex)
        {
            // TD 11470: Only look for the Integrity constraint error. 
            // All other exceptions should be thrown.
            std::string KEY_WORD("integrity constraint");
            std::string errorMsg(ex.what());
            if(errorMsg.find(KEY_WORD) != std::string::npos)
            {
                std::string message = "Cannot delete Location that is "
                                      "referenced by other items";

                TA_THROW( DataException(message.c_str(),
                                        DataException::CANNOT_DELETE,
                                        "") );
            }
        }
        
        FUNCTION_EXIT;
    }


    void LocationHelper::addNewLocation()
    {
        FUNCTION_ENTRY("addNewLocation");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Location %s is a new location. It will be added to the database.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // First check there is not alreay an entity with this name (it is a uniqueness
        // constraint).
        /*std::ostringstream formatSQL;
        formatSQL << "select PKEY from LOCATION where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";
        formatSQL << " or DESCRIPTION = '";
        formatSQL << databaseConnection->escapeQueryString(m_description) << "'";
        formatSQL << " or ORDER_ID = " << m_orderId;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_STD_SELECT_7002, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7002, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7002,
			databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_description), m_orderId);

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
			reasonMessage << "A location already exists with name = " << m_name.c_str() 
                          << " or description = " << m_description.c_str()
                          << " or order_id = " << m_orderId;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name, description or order") );
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the entity (reuse the string)
        /*formatSQL.str("");
        formatSQL << "insert into LOCATION (PKEY,NAME,DESCRIPTION,ORDER_ID,IS_DISPLAY_ONLY,TYPE_NAME) ";
        formatSQL << "values (LOCATION_SEQ.NEXTVAL,'";
		formatSQL << databaseConnection->escapeInsertString(m_name) << "','";
		formatSQL << databaseConnection->escapeInsertString(m_description) << "',";
        formatSQL << m_orderId<<","<<m_displayOnly;
        formatSQL << ", '" << databaseConnection->escapeInsertString(m_typeName) << "'";
        formatSQL << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_Oracle_INSERT_7351, databaseConnection->escapeInsertString(m_name),
//		strSql  = databaseConnection->prepareSQLStatement(LOCATION_INSERT_7351, databaseConnection->escapeInsertString(m_name),

		databaseConnection->prepareSQLStatement(strSql, LOCATION_INSERT_7351,
            databaseConnection->getNextPkey("LOCATION_SEQ"), databaseConnection->escapeInsertString(m_name),
			databaseConnection->escapeInsertString(m_description), m_orderId, m_displayName, m_displayOnly);
 
        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
        /*std::ostringstream sql;
		sql << "select PKEY, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from LOCATION where NAME = '";
        sql << databaseConnection->escapeQueryString(m_name).c_str() << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_Oracle_SELECT_7058, databaseConnection->escapeQueryString(m_name));
//		strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7058, databaseConnection->escapeQueryString(m_name));

		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7058, databaseConnection->escapeQueryString(m_name));

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

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
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name, description or order") );
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


    void LocationHelper::modifyExistingLocation()
    {
        FUNCTION_ENTRY("modifyExistingLocation");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Location %s already exists. It's details will be updated.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::ostringstream formatSQL;
        formatSQL << "select PKEY from LOCATION where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";
        formatSQL << " or DESCRIPTION = '";
        formatSQL << databaseConnection->escapeQueryString(m_description) << "'";
        formatSQL << " or ORDER_ID = " << m_orderId;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_STD_SELECT_7002, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7002, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7002,
			databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_description), m_orderId);

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
			    reasonMessage << "A location already exists with name = " << m_name.c_str();
                reasonMessage << " or description = " << m_description.c_str();
                reasonMessage << " or order_id = " << m_orderId;
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name, description or order") );
            }
            // else - it matches, so that's okay - it means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one location already exists name = " << m_name.c_str();
            reasonMessage << " or description = " << m_description.c_str();
            reasonMessage << " or order ID = " << m_orderId;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name, description or order") );
        }

        delete data;
        data =  NULL;

        /*std::ostringstream sql;
        sql << "update LOCATION set NAME = '" << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "', DESCRIPTION = '" << (databaseConnection->escapeQueryString(m_description)).c_str();
        sql << "', ORDER_ID = " << m_orderId;?        	sql<<",IS_DISPLAY_ONLY = "<<m_displayOnly;
        sql << ", TYPE_NAME = '" << (databaseConnection->escapeQueryString(m_typeName)).c_str() << "'";
        sql << " where pkey = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_STD_UPDATE_7151, databaseConnection->escapeQueryString(m_name),
//		strSql  = databaseConnection->prepareSQLStatement(LOCATION_UPDATE_7151, databaseConnection->escapeQueryString(m_name),

		databaseConnection->prepareSQLStatement(strSql, LOCATION_UPDATE_7151, databaseConnection->escapeQueryString(m_name),
			databaseConnection->escapeQueryString(m_description), m_orderId, m_displayName, m_displayOnly, m_key);

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from LOCATION where PKEY = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_Oracle_SELECT_7059, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7059, m_key);

		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7059, m_key);

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
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name, description or order") );
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


    std::string LocationHelper::getTypeName()
    {
        // We don't want to call reload() if this is a new location and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing location and the data needs to be loaded
            reload();
        }
        return m_typeName;
    }


    void LocationHelper::setTypeName( const std::string& typeName )
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_typeName = typeName;
    }

} // closes TA_Base_Core


