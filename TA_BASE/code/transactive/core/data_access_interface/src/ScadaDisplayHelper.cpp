/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ScadaDisplayHelper.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ScadaDisplayHelper is an object that is held by ScadaDisplay and ConfigScadaDisplay objects. 
  * It contains all data used by display, and manipulation
  * methods for the data. It helps avoid re-writing code for both ScadaDisplay and ConfigScadaDisplay.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <ctime>

#include "core/data_access_interface/src/ScadaDisplayHelper.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	ScadaDisplayHelper::ScadaDisplayHelper( const ScadaDisplayHelper& theScadaDisplayHelper)
		: m_key(0),
		  m_name(theScadaDisplayHelper.m_name),
		  m_path(theScadaDisplayHelper.m_path),
          m_location(theScadaDisplayHelper.m_location),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    ScadaDisplayHelper::ScadaDisplayHelper(const unsigned long key)
        :m_key(key),
         m_name(""),
         m_path(""),
         m_location(0),
         m_dateCreated(0),
         m_dateModified(0),
         m_isValidData(false),
         m_isNew(false)
    {
    }

    
    ScadaDisplayHelper::ScadaDisplayHelper(const unsigned long row, TA_Base_Core::IData& data)
		:   
		m_key(0),
		m_name(""),
		m_path(""),
		m_location(0),
		m_dateCreated(0),
		m_dateModified(0),
		m_isValidData(false),
		m_isNew(false)
    {
        FUNCTION_ENTRY("ProfileHelper(const unsigned long, TA_Base_Core::IData&)");        

        reloadUsing(row, data);
        FUNCTION_EXIT;
    }	


    ScadaDisplayHelper::ScadaDisplayHelper()
        :m_key(0),
         m_name(""),
         m_path(""),
         m_location(0),
         m_dateCreated(0),
         m_dateModified(0),
         m_isValidData(false),
         m_isNew(true)
    {
    }


    ScadaDisplayHelper::~ScadaDisplayHelper()
    {
    }


    unsigned long ScadaDisplayHelper::getKey()
    {
        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"The data must be written to the database before the key can be retrieved");
        
        return m_key;
    }


    std::string ScadaDisplayHelper::getName()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_name;
    }


    void ScadaDisplayHelper::setName(const std::string& name)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ScadaDisplayHelper::setName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "ScadaDisplayHelper::setName" );
    }


    std::string ScadaDisplayHelper::getPath()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_path;
    }


    void ScadaDisplayHelper::setPath(const std::string& path)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_path= path;
    }


	unsigned long ScadaDisplayHelper::getLocation()
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_location;
	}


    std::string ScadaDisplayHelper::getLocationName()
    {
        ILocation* location = NULL;

        try
        {
		    // Get the location object
		    location = LocationAccessFactory::getInstance().getLocationByKey( getLocation(), false );
		    std::string name( location->getName() );

		    // Clean up
		    delete location;

		    // Return the name
		    return name;
        }
        catch ( ... )
        {
            throw;
        }
    }


	void ScadaDisplayHelper::setLocation( const unsigned long location )
	{
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_location = location;
	}

	
    time_t ScadaDisplayHelper::getDateCreated()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_dateCreated;
    }


    time_t ScadaDisplayHelper::getDateModified()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_dateModified;
    }


    void ScadaDisplayHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new operator");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

	void ScadaDisplayHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("SCSSCH_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("PATH");
        columnNames.push_back("LOCATION");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}

    void ScadaDisplayHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Ensure the display has already been written to the database. It doesn't make
        // any sense to reload an operator that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This display does not yet exist in the database. Therefore it cannot be reloaded.");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

        // create the SQL string to retrieve the data of the display based upon the key

        /*std::ostringstream sql;
		sql << " select SCSSCH_ID, NAME, PATH, COLOCA_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SC_SCADA_SCHEMATIC WHERE SCSSCH_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_Oracle_SELECT_20051, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20051, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20051, m_key);
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql,columnNames));

        // Need to bring in the DataException
            using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
			reasonMessage << "No display found for key = " << m_key;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }


		TA_ASSERT(data->getNumRows() == 1, "Invalid number of ScadaDisplay records."); 
        reloadUsing(0, *data);
		
        FUNCTION_EXIT;
    }


	void ScadaDisplayHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key  = data.getUnsignedLongData(row, columnNames[0]);
		m_name = data.getStringData(row, columnNames[1]);
        m_path = data.getStringData(row, columnNames[2]);
		m_location = data.getUnsignedLongData( row, columnNames[3]);

        m_dateModified = data.getDateData(row, columnNames[4], 0);
        m_dateCreated = data.getDateData( row, columnNames[5]);

        m_isValidData = true;
        FUNCTION_EXIT;
    }
		


    void ScadaDisplayHelper::writeData()
    {       
        FUNCTION_ENTRY("writeData");

        // Check all values exist. If not add them to the dataConfiguraitonException
        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }
        if (m_path.empty())
        {
            fieldNames.push_back("Path");
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Display data not fully specified. Scada display cannot be written to database", fieldNames));
        }

        // Write the display to the database

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
        
        // Need to check if this is a new display or an existing display - 
        // as a new display will be inserted, while an existing display will be updated
        if (m_isNew) // This is a new display
        {
            addNew();
        }
        else // This is an existing display
        {
            modifyExisting();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void ScadaDisplayHelper::deleteDisplay()
    {
        FUNCTION_ENTRY("deleteDisplay");

        TA_ASSERT(!m_isNew, "This display does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
        
        /*std::ostringstream sql;
        sql << "delete SC_SCADA_SCHEMATIC where SCSSCH_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_STD_DELETE_20401, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_DELETE_20401, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_DELETE_20401, m_key);

        // Try the delete. if this display is still it use an error will be thrown
        try
        {
            databaseConnection->executeModification(strSql);
        }
        catch (DataException)
        {
            // Check that the display isn't being referenced in the SE_PROFILE_PARAMETERS table
            // this will throw it's own nicely formated error
            checkForReferencesFromProfileParameterTable();

            // otherwise something else bad happened
            throw;
        }

        FUNCTION_EXIT;
    }


    void ScadaDisplayHelper::checkForReferencesFromProfileParameterTable()
    {
        // Get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
        

        // Find out if there are any rows in the entity table that reference
        // this row.
        /*std::ostringstream cascadeSql;
        cascadeSql
            << "select p.name from se_profile_parameters spp, se_profile p "
            << "where spp.display_1 = '" << m_key 
            << "'  or spp.display_2 = '" << m_key 
            << "'  or spp.display_3 = '" << m_key 
            << "' and p.seprof_id = spp.seprof_id" ;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_PARAMETERS_STD_SELECT_22003,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_PARAMETERS_SELECT_22003,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_PARAMETERS_SELECT_22003,
			m_key, m_key, m_key);


        std::string profileColumn = "PROFILE";

        std::vector<std::string> columnNames;
        columnNames.push_back(profileColumn);

        IData* data =
            databaseConnection->executeQuery(strSql, columnNames);


        // If there are rows in the entity table that reference this row,
        // throw an exception.
        if (0 != data->getNumRows())
        {
            try
            {
                std::string message = "Cannot delete Scada Display that is "
                                      "referenced by profile";

                // Get the names of the entities that reference this row.
                std::string profiles = "Profiles: ";
                profiles            += getCommaSeparatedList(data, profileColumn);

                TA_THROW( DataException(message.c_str(), DataException::CANNOT_DELETE, profiles) );
            }
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


    std::string ScadaDisplayHelper::getCommaSeparatedList(
        IData* data, const std::string&  columnName)
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


    void ScadaDisplayHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Display %s already exists. It's data will be updated.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

        // Create a select statement that will ensure the uniqueness of the name and path.
        /*std::string formatSQL = "select SCSSCH_ID from SC_SCADA_SCHEMATIC where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "' or PATH = '"
            + databaseConnection->escapeQueryString(m_path) + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_STD_SELECT_20001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20001,
			databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_path) );

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 rows returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {   // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A display already exists with name = " << m_name;
                reasonMessage << " or path = " << m_path;
                throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and path");
            }
            // else - it matches, so that's okay - it just means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one display exists with name = " << m_name;
            reasonMessage << " or path = " << m_path;
            throw DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and path");
        }

        /*std::ostringstream sql;
        sql << "update SC_SCADA_SCHEMATIC set NAME = '";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "', PATH = '";
        sql << (databaseConnection->escapeInsertString(m_path)).c_str();
        sql << "', COLOCA_ID = " << m_location;
        sql << " where SCSSCH_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_STD_UPDATE_20151,
//		strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_UPDATE_20151,

		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_UPDATE_20151,
			databaseConnection->escapeInsertString(m_name), databaseConnection->escapeInsertString(m_path), m_location, m_key );

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from SC_SCADA_SCHEMATIC where SCSSCH_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_Oracle_SELECT_20055, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20055, m_key);

		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20055, m_key);

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
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and path") );
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

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void ScadaDisplayHelper::addNew()
    {
        FUNCTION_ENTRY("addNew");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Display %s is a new display. It will be added to the database.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

        // First check there is not alreay an operator with this name or 
        // description (it is a uniqueness constraint).

        /*std::ostringstream formatSQL;
        formatSQL << "select SCSSCH_ID from SC_SCADA_SCHEMATIC where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "' or PATH = '";
        formatSQL << databaseConnection->escapeQueryString(m_path) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_STD_SELECT_20001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20001,
			databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_path) );

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name or description already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A display already exists with name " << m_name << " or path " << m_path;
            using TA_Base_Core::DataException;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and path"));
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        /*std::ostringstream sql;
        sql << "insert into SC_SCADA_SCHEMATIC (SCSSCH_ID, NAME, PATH, COLOCA_ID) ";
        sql << "values (SCADA_SEQ.NEXTVAL,'";
        sql << (databaseConnection->escapeInsertString(m_name)).c_str();
        sql << "','" << (databaseConnection->escapeInsertString(m_path)).c_str();
		sql << "'," << m_location;
        sql << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_Oracle_INSERT_20351,
//		strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_INSERT_20351,

		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_INSERT_20351,
		    databaseConnection->getNextPkey("SCADA_SEQ"),
			databaseConnection->escapeInsertString(m_name), databaseConnection->escapeInsertString(m_path), m_location);

        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
        /*sql.str("");
        sql << "select SCSSCH_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from SC_SCADA_SCHEMATIC where NAME = '";
        sql << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_Oracle_SELECT_20056,
//		strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20056,

		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20056,
			databaseConnection->escapeQueryString(m_name) );

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage <<  "No data found for name = " << m_name.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage <<  "More than one entry found for name = " << m_name.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and path"));
        }

        // Retrieve the pkey as an unsigned long - the name of this operator - from the 
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

} // closes TA_Base_Core
