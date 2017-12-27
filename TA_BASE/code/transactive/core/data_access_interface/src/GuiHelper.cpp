/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/GuiHelper.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * GuiHelper is an object that is held by Gui and ConfigGui objects. 
  * It contains all data used by guis, and manipulation
  * methods for the data. It helps avoid re-writing code for both Gui and ConfigGui.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <vector>
#include <ctime>

#include "core/data_access_interface/src/GuiHelper.h"
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
    GuiHelper::GuiHelper(const unsigned long key):
              m_isValidData(false),
              m_isNew(false),
              m_key(key),
              m_dateCreated(0),
              m_dateModified(0)
    {
    }

    GuiHelper::GuiHelper():
            m_isValidData(false),
            m_isNew(true),
            m_name( "" ),
            m_fullName( "" ),
            m_executable( "" ),
            m_helpFilePath( "" ),
            m_isIconDisplayed( true ),
            m_areInstancesLimited( false ),
            m_shouldReposition( false ),
            m_shouldPassParameters( false ),
            m_shouldTerminateOnLogout( true ),
            m_dateCreated(0),
            m_dateModified(0)
    {
    }


    GuiHelper::GuiHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false)
    {
        FUNCTION_ENTRY("GuiHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }


	GuiHelper::GuiHelper( const GuiHelper& theGuiHelper)
        : m_isValidData(false),
          m_isNew(true),
          m_key( theGuiHelper.m_key ),
          m_name( theGuiHelper.m_name ),
          m_fullName( theGuiHelper.m_fullName ),
          m_executable( theGuiHelper.m_executable ),
          m_helpFilePath( theGuiHelper.m_helpFilePath ),
          m_isIconDisplayed( theGuiHelper.m_isIconDisplayed ),
          m_areInstancesLimited( theGuiHelper.m_areInstancesLimited ),
          m_shouldReposition( theGuiHelper.m_shouldReposition ),
          m_shouldPassParameters( theGuiHelper.m_shouldPassParameters ),
          m_shouldTerminateOnLogout( theGuiHelper.m_shouldTerminateOnLogout ),
          m_dateCreated( theGuiHelper.m_dateCreated ),
          m_dateModified( theGuiHelper.m_dateModified )

    {
    }
            
    unsigned long GuiHelper::getKey()
    {
        // This method cannot be called until the key has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_key;
    }


    std::string GuiHelper::getName()
    {
        // This method cannot be called until the name has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_name;
    }


    std::string GuiHelper::getFullName()
    {
        // if full name has not been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_fullName;
    }


    std::string GuiHelper::getExecutable()
    {
        // This method cannot be called until the executable has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_executable;
    }


    std::string GuiHelper::getHelpFilePath()
    {
        // This method cannot be called until the executable has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_helpFilePath;
    }

    
    bool GuiHelper::isIconDisplayed()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_isIconDisplayed;
    }


    bool GuiHelper::areInstancesLimited()
    {
        // This method cannot be called until areInstancesLimited has been set
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_areInstancesLimited;
    }


    bool GuiHelper::shouldReposition()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_shouldReposition;
    }
    

    bool GuiHelper::shouldPassParameters()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_shouldPassParameters;
    }
    

    bool GuiHelper::shouldTerminateOnLogout()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_shouldTerminateOnLogout;
    }


    time_t GuiHelper::getDateCreated()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date created can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t GuiHelper::getDateModified()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateModified;
    }


    void GuiHelper::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;

        FUNCTION_EXIT;
    }


    void GuiHelper::setFullName(const std::string& fullName)
    {
       FUNCTION_ENTRY("setFullName");

       if (!m_isValidData && !m_isNew)
       {
           reload();
       }
       m_fullName = fullName;

       FUNCTION_EXIT;
    }


    void GuiHelper::setExecutable(const std::string& executable)
    {
       FUNCTION_ENTRY("setExecutable");

       if (!m_isValidData && !m_isNew)
       {
           reload();
       }
       m_executable = executable;

       FUNCTION_EXIT;
    }


    void GuiHelper::setHelpFilePath(const std::string& helpFilePath)
    {
       FUNCTION_ENTRY("setHelpFilePath");

       if (!m_isValidData && !m_isNew)
       {
           reload();
       }
       m_helpFilePath = helpFilePath;

       FUNCTION_EXIT;
    }


    void GuiHelper::setIsIconDisplayed(bool isIconDisplayed)
    {
       FUNCTION_ENTRY("setIsIconDisplayed");

       if (!m_isValidData && !m_isNew)
       {
           reload();
       }
       m_isIconDisplayed = isIconDisplayed;

       FUNCTION_EXIT;
    }


    void GuiHelper::setAreInstancesLimited(bool areInstancesLimited)
    {
       FUNCTION_ENTRY("setAreInstancesLimited");

       if (!m_isValidData && !m_isNew)
       {
           reload();
       }
       m_areInstancesLimited = areInstancesLimited;

       FUNCTION_EXIT;
    }


    void GuiHelper::setShouldReposition(bool shouldReposition)
    {
       FUNCTION_ENTRY("setShouldReposition");

       if (!m_isValidData && !m_isNew)
       {
           reload();
       }
       m_shouldReposition = shouldReposition;

       FUNCTION_EXIT;
    }


    void GuiHelper::setShouldPassParameters(bool shouldPassParams)
    {
       FUNCTION_ENTRY("setShouldPassParameters");

       if (!m_isValidData && !m_isNew)
       {
           reload();
       }
       m_shouldPassParameters = shouldPassParams;

       FUNCTION_EXIT;
    }


    void GuiHelper::setShouldTerminateOnLogout(bool shouldTerminateOnLogout)
    {
       FUNCTION_ENTRY("setShouldTerminateOnLogout");

       if (!m_isValidData && !m_isNew)
       {
           reload();
       }
       m_shouldTerminateOnLogout = shouldTerminateOnLogout;

       FUNCTION_EXIT;
    }
    
    
    void GuiHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "GuiHelper::invalidate" );
        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new gui");

        m_isValidData = false;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "GuiHelper::invalidate" );
    }

	void GuiHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("FULLNAME");
        columnNames.push_back("EXECUTABLE");
        columnNames.push_back("HELP");
        columnNames.push_back("MAXINSTANCES");
        columnNames.push_back("LAUNCHICON");
		columnNames.push_back("REPOSITION");
        columnNames.push_back("PASSPARAMS");
        columnNames.push_back("TERMINATE");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}

    void GuiHelper::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "GuiHelper::reload" );
        // Ensure the gui has already been written to the database. It doesn't make
        // any sense to reload an gui that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This gui does not yet exist in the database. Therefore it cannot be reloaded.");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the data of the gui
        // based upon the key
        /*std::ostringstream sql;
		sql << "SELECT pkey, name, fullname, executable, help, maxinstances, show_launch_icon, reposition, passregisterparams, terminate_on_logout, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') ";
        sql << "FROM guiapplication where pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_Oracle_SELECT_16553, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_SELECT_16553, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_SELECT_16553, m_key);

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
			reasonMessage << "No gui found for pkey = " << m_key;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of gui application records."); 
        reloadUsing(0, *data);
		
        LOG ( SourceInfo, DebugUtil::FunctionExit, "GuiHelper::reload" );
    }


	void GuiHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);
		
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key = data.getUnsignedLongData(row, columnNames[0]);
		m_name = data.getStringData(row, columnNames[1]);
		m_fullName = data.getStringData(row, columnNames[2]);
        m_executable = data.getStringData(row, columnNames[3]);
        m_helpFilePath = data.getStringData(row, columnNames[4]);
        m_areInstancesLimited = data.getBooleanData(row, columnNames[5]);
		m_isIconDisplayed = data.getBooleanData(row, columnNames[6]);
        m_shouldReposition = data.getBooleanData(row, columnNames[7]);
        m_shouldPassParameters = data.getBooleanData(row, columnNames[8]);
        m_shouldTerminateOnLogout = data.getBooleanData(row, columnNames[9]);
        m_dateModified = data.getDateData(row, columnNames[10], 0);
        m_dateCreated = data.getDateData( row, columnNames[11]);

        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void GuiHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.

        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Short Name");
        }
        if (m_fullName.empty())
        {
            fieldNames.push_back("Full Name");
        }
        if (m_executable.empty())
        {
            fieldNames.push_back("Executable");
        }

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("Gui data not fully specified. Gui cannot be written to database" ,fieldNames) );
        }

        // Some fields must be unique
        checkUniqueConstraints();

        // Write the Gui to the database

        // Need to check if this is a new gui or an existing gui - 
        // as a new gui will be inserted, while an existing gui will be updated
        if (m_isNew)
        {
             // This is a new gui
            addNew();
        }
        else 
        {
            // This is an existing gui
            modifyExisting();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void GuiHelper::deleteThisGui()
    {
        FUNCTION_ENTRY("deleteThisGui");

        TA_ASSERT(!m_isNew, "This gui does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // We must now attempt to delete the location

        /*std::ostringstream sql;
        sql << "delete GUIAPPLICATION where pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_STD_DELETE_16901, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_DELETE_16901, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_DELETE_16901, m_key);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void GuiHelper::addNew()
    {
        FUNCTION_ENTRY("addNew");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Gui %s is a new gui. It will be added to the database.", m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        //
        // First we must retrieve the next key. Since we have no other unique keys we must retrieve
        // the key first
        //
        //std::string sql("select GUIAPPLICATION_SEQ.NEXTVAL from dual");
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_Oracle_SELECT_16554);
//		std::string strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_SELECT_16554);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_SELECT_16554);
        std::vector<std::string> columnNames;
        columnNames.push_back("key");

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            TA_THROW( DataException("No new pkey could be retrieved",DataException::NO_VALUE,"") );
        }

        unsigned long key = data->getUnsignedLongData(0, "key");
        delete data;
        data = NULL;

        //
        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the gui
        //
        /*std::ostringstream formatSQL;
        formatSQL << "insert into GUIAPPLICATION (PKEY,NAME,FULLNAME,EXECUTABLE,HELP,MAXINSTANCES,REPOSITION,PASSREGISTERPARAMS,SHOW_LAUNCH_ICON,TERMINATE_ON_LOGOUT)";
        formatSQL << " values (" << key << ",'";
		formatSQL << databaseConnection->escapeInsertString(m_name) << "','";
		formatSQL << databaseConnection->escapeInsertString(m_fullName) << "','";
		formatSQL << databaseConnection->escapeInsertString(m_executable) << "','";
		formatSQL << databaseConnection->escapeInsertString(m_helpFilePath) << "',";
        formatSQL << m_areInstancesLimited << ",";
        formatSQL << m_shouldReposition << ",";
        formatSQL << m_shouldPassParameters << ",";
        formatSQL << m_isIconDisplayed << ",";
        formatSQL << m_shouldTerminateOnLogout << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_STD_INSERT_16801, key, databaseConnection->escapeInsertString(m_name),
//		strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_INSERT_16801, key, databaseConnection->escapeInsertString(m_name),

		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_INSERT_16801, key, databaseConnection->escapeInsertString(m_name),
			databaseConnection->escapeInsertString(m_fullName), databaseConnection->escapeInsertString(m_executable), databaseConnection->escapeInsertString(m_helpFilePath),
			m_areInstancesLimited, m_shouldReposition, m_shouldPassParameters, m_isIconDisplayed, m_shouldTerminateOnLogout );

        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
        /*formatSQL.str("");
		formatSQL << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from GUIAPPLICATION where PKEY = " << key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_Oracle_SELECT_16555, key);
//		strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_SELECT_16555, key);

		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_SELECT_16555, key);

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        columnNames.clear();
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
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
        }

        try
        {
            m_key = key;
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


    void GuiHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "Gui %s already exists. It's details will be updated.", m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        /*std::ostringstream sql;
        sql << "update GUIAPPLICATION set NAME = '" << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "', FULLNAME = '" << (databaseConnection->escapeQueryString(m_fullName)).c_str();
        sql << "', EXECUTABLE = '" << (databaseConnection->escapeQueryString(m_executable)).c_str();
        sql << "', HELP = '" << (databaseConnection->escapeQueryString(m_helpFilePath)).c_str();
        sql << "', MAXINSTANCES = " << m_areInstancesLimited;
        sql << ", REPOSITION = " << m_shouldReposition;
        sql << ", PASSREGISTERPARAMS = " << m_shouldPassParameters;
        sql << ", SHOW_LAUNCH_ICON = " << m_isIconDisplayed;
        sql << ", TERMINATE_ON_LOGOUT = " << m_shouldTerminateOnLogout;
        sql << " where pkey = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_STD_UPDATE_16651,
//		std::string strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_UPDATE_16651,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_UPDATE_16651,
			databaseConnection->escapeQueryString(m_name), databaseConnection->escapeQueryString(m_fullName), databaseConnection->escapeQueryString(m_executable),
			databaseConnection->escapeQueryString(m_helpFilePath), m_areInstancesLimited, m_shouldReposition, m_shouldPassParameters, m_isIconDisplayed,
			m_shouldTerminateOnLogout, m_key);

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql << " select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS')"
			<< " from GUIAPPLICATION where PKEY = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_Oracle_SELECT_16556, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_SELECT_16556, m_key);

		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_SELECT_16556, m_key);

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


    void GuiHelper::checkUniqueConstraints()
    {
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        //
        // Check if the (short) name is unique.
        //

        /*std::ostringstream sql;
        sql << " SELECT pkey FROM guiapplication WHERE name like '" << databaseConnection->escapeQueryString(m_name);
        sql << "' AND pkey <> " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_STD_SELECT_16501, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_SELECT_16501, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_SELECT_16501,
			databaseConnection->escapeQueryString(m_name), m_key);

        std::vector<std::string> columnNames;
        std::string nameColumn = "Pkey";
        columnNames.push_back("Pkey");

        // Execute the query. The method can throw a DatabaseException.
        IData* data = databaseConnection->executeQuery(strSql, columnNames);
        bool isShortNameUnique = (data->getNumRows() == 0);
        delete data;
        data = NULL;

        //
        // Check if the full name is unique.
        //

       /* sql.str("");
        sql << "SELECT pkey FROM guiapplication WHERE fullname like '" << databaseConnection->escapeQueryString(m_fullName);
        sql << "' AND pkey <> " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_STD_SELECT_16502,
//		strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_SELECT_16502,

		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_SELECT_16502,
			databaseConnection->escapeQueryString(m_fullName), m_key);

        // Execute the query. The method can throw a DatabaseException.
        data = databaseConnection->executeQuery(strSql, columnNames);
        bool isFullNameUnique = (data->getNumRows() == 0);
        delete data;
        data = NULL;

        // Throw an exception if necessary
        std::ostringstream reasonMessage;

        if (!isShortNameUnique && !isFullNameUnique)
        {
            reasonMessage << "More than one entry found with short name " << m_name << " and full name " << m_fullName;
            TA_THROW( DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "Short Name and Full Name") );
        }
        if (!isShortNameUnique)
        {
            reasonMessage << "More than one entry found with short name " << m_name;
            TA_THROW( DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "Short Name") );
        }
        if (!isFullNameUnique)
        {
            reasonMessage << "More than one entry found with full name " << m_fullName;
            TA_THROW( DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "Full Name") );
        }
    }

} // closes TA_Base_Core
