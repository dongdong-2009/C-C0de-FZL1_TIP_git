/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/GlobalParameterHelper.cpp $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * GlobalParameterHelper is an object that is held by all type-specific GlobalParameter objects, as well as by
  * the ConfigGlobalParameter. It contains all data common across GlobalParameters, and manipulation
  * methods for the data. It helps avoid re-writing code for each GlobalParameter object.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include <vector>
#include <algorithm>

#include "core/data_access_interface/src/GlobalParameterHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"

#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }

    static const unsigned long INVALID_KEY = -1;

    GlobalParameterHelper::GlobalParameterHelper( ) 
        : m_key(0),
          m_name(""),
          m_value(""), 
          m_subsystemKey(INVALID_KEY),
          m_dateCreated(0),
          m_dateModified(0), 
          m_isValidData(false),
          m_isNew(true)
    {
    }


    GlobalParameterHelper::GlobalParameterHelper( const unsigned long key )
        : m_key(key),
          m_name(""),
          m_value(""), 
          m_subsystemKey(INVALID_KEY),
          m_dateCreated(0),
          m_dateModified(0), 
          m_isValidData(false),
          m_isNew(false)
    {
    }
    

    GlobalParameterHelper::GlobalParameterHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false)
    {
        FUNCTION_ENTRY("GlobalParameterHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }

	
	GlobalParameterHelper::GlobalParameterHelper( const GlobalParameterHelper& theGlobalParameterHelperToCopy)
    {
        m_key           = 0;
        m_name          = theGlobalParameterHelperToCopy.m_name;
        m_value         = theGlobalParameterHelperToCopy.m_value;
        m_subsystemKey  = theGlobalParameterHelperToCopy.m_subsystemKey;
        m_dateModified  = theGlobalParameterHelperToCopy.m_dateModified;
        m_dateCreated   = theGlobalParameterHelperToCopy.m_dateCreated;
        m_isValidData   = theGlobalParameterHelperToCopy.m_isValidData;
        m_isNew         = true;
    }


    GlobalParameterHelper::~GlobalParameterHelper()
    {
    }


    unsigned long GlobalParameterHelper::getKey()
    {
        TA_ASSERT(!m_isNew, "Cannot retrieve the key on a GlobalParameter before it has been applied to the database");
        return m_key;
    }

    
    std::string GlobalParameterHelper::getName()
    {
        // We don't want to call reload() if this is a new GlobalParameter and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing GlobalParameter and the data needs to be loaded
            reload();
        }
        return m_name;
    }


    
    void GlobalParameterHelper::setName(const std::string& name)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;
    }


    std::string GlobalParameterHelper::getValue()
    {
        // We don't want to call reload() if this is a new GlobalParameter and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing GlobalParameter and the data needs to be loaded
            reload();
        }
        return m_value;
    }

    void GlobalParameterHelper::setValue(const std::string& value)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_value = value;
    }

    unsigned long GlobalParameterHelper::getSubsystemKey()
    {
        // We don't want to call reload() if this is a new GlobalParameter and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing GlobalParameter and the data needs to be loaded
            reload();
        }
        return m_subsystemKey;
    }

    void GlobalParameterHelper::setSubsystemKey(unsigned long subsystemKey)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_subsystemKey = subsystemKey;
    }


    time_t GlobalParameterHelper::getDateCreated()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date created can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t GlobalParameterHelper::getDateModified()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData)
        {
            reload();
        }
        return m_dateModified;
    }



    void GlobalParameterHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new entity");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void GlobalParameterHelper::applyChanges()
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
        if (m_subsystemKey == INVALID_KEY)
        {
            fieldNames.push_back("Subsystem Key");
        }
        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("GlobalParameter data not fully specified. GlobalParameter cannot be written to database" ,fieldNames) );
        }

        // Write the GlobalParameter to the database

        // Need to check if this is a new entity or an existing entity - 
        // as a new entity will be inserted, while an existing entity will be updated
        if (m_isNew)
        {
             // This is a new entity
            addNewGlobalParameter();
        }
        else 
        {
            // This is an existing entity
            modifyExistingGlobalParameter();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

	void GlobalParameterHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("GPARAM_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("VALUE");
        columnNames.push_back("SUBSYSTEMKEY");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}

	void GlobalParameterHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        /*std::ostringstream sql;
        sql << "select GPARAM_ID, NAME, VALUE, SUBSYSTEMKEY, "
			<< "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
			<< "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM GLOBAL_PARAMETER WHERE GPARAM_ID = " << m_key;*/		

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), GLOBAL_PARAMETER_Oracle_SELECT_16053, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(GLOBAL_PARAMETER_SELECT_16053, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GLOBAL_PARAMETER_SELECT_16053, m_key);
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery( strSql, columnNames ));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reasonMessage;
            reasonMessage << "No GlobalParameter found for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of global parameter records."); 
        reloadUsing(0, *data);

        FUNCTION_EXIT;
    }


	void GlobalParameterHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);
		
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key           = data.getUnsignedLongData( row, columnNames[0] );
        m_name          = data.getStringData( row, columnNames[1] );
        m_value         = data.getStringData( row, columnNames[2] );
        m_subsystemKey  = data.getUnsignedLongData( row, columnNames[3] );
        m_dateModified = data.getDateData( row, columnNames[4], 0);
        m_dateCreated   = data.getDateData( row, columnNames[5] );
 
        m_isValidData = true;        
        FUNCTION_EXIT;
    }

    void GlobalParameterHelper::deleteThisGlobalParameter()
    {
        FUNCTION_ENTRY("deleteThisGlobalParameter");

        TA_ASSERT(!m_isNew, "This GlobalParameter does not yet exist in the database, and therefore cannot be deleted");
        
        IDatabase *pDatabaseConn = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        // We must now attempt to delete the GlobalParameter

        /*std::ostringstream sql;
		sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','DELETE', '";
        sql << "delete GLOBAL_PARAMETER where GPARAM_ID = " << m_key;
        sql << "');";
        sql << "END;";*/
//		std::string strSql = defPrepareSQLStatement(pDatabaseConn->GetDbServerType(), GLOBAL_PARAMETER_Oracle_DELETE_16451, 
//		std::string strSql  = pDatabaseConn->prepareSQLStatement(GLOBAL_PARAMETER_DELETE_16451, 
		SQLStatement strSql;
		pDatabaseConn->prepareSQLStatement(strSql, GLOBAL_PARAMETER_DELETE_16451,
			getLocalDatabaseName(), m_key );

        pDatabaseConn->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void GlobalParameterHelper::addNewGlobalParameter()
    {
        FUNCTION_ENTRY("addNewGlobalParameter");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "GlobalParameter %s is a new GlobalParameter. It will be added to the database.",
                m_name.c_str());

        validateNameSubsystemUniqueness();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);   
        /*std::ostringstream sql;
		sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','INSERT', '";
        sql << "insert into GLOBAL_PARAMETER (PKEY,NAME,VALUE,SUBSYSTEMKEY) ";
        sql << "values (GPARAM_SEQ.NEXTVAL,''";
		sql << databaseConnection->escapeAQSQLString(m_name) << "'',''";
		sql << databaseConnection->escapeAQSQLString(m_value) << "'',";
        sql << m_subsystemKey << ")";
        sql << "');";
        sql << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), GLOBAL_PARAMETER_Oracle_INSERT_16351,
//		std::string strSql  = databaseConnection->prepareSQLStatement(GLOBAL_PARAMETER_INSERT_16351,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GLOBAL_PARAMETER_INSERT_16351,
			getLocalDatabaseName(), databaseConnection->escapeAQSQLString(m_name), databaseConnection->escapeAQSQLString(m_value),
			m_subsystemKey );
        databaseConnection->executeModification(strSql);

        //now need to get the update of the date fields
        /*sql.str("");
        sql << " select GPARAM_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from GLOBAL_PARAMETER where SUBSYSTEMKEY = " << m_subsystemKey
			<< " and NAME = '" << m_name << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), GLOBAL_PARAMETER_Oracle_SELECT_16055, m_subsystemKey, m_name);
//		strSql  = databaseConnection->prepareSQLStatement(GLOBAL_PARAMETER_SELECT_16055, m_subsystemKey, m_name);

		databaseConnection->prepareSQLStatement(strSql, GLOBAL_PARAMETER_SELECT_16055, m_subsystemKey, m_name);

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("GPARAM_ID");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read)->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for name-subsystem = " << m_name << "-" << m_subsystemKey;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,"GPARAM_ID");
            m_dateCreated = data->getDateData(0,"DATE_CREATED");
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


    void GlobalParameterHelper::modifyExistingGlobalParameter()
    {
        FUNCTION_ENTRY("modifyExistingGlobalParameter");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "GlobalParameter %s already exists. It's details will be updated.",
                m_name.c_str());

        //validateKeyUniqueness();
        //validateNameSubsystemUniqueness();
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        /*std::ostringstream sql;

		sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','UPDATE', '";
		sql << "update GLOBAL_PARAMETER set ";
        sql << "NAME=''" << (databaseConnection->escapeAQSQLString(m_name)).c_str() << "'', ";
        sql << "VALUE=''"<< (databaseConnection->escapeAQSQLString(m_value)).c_str() << "'', ";
        sql << "SUBSYSTEMKEY=" << m_subsystemKey << " ";
        sql << "where GPARAM_ID=" << m_key;
        sql << "');";
        sql << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), GLOBAL_PARAMETER_Oracle_UPDATE_16201,
//		std::string strSql  = databaseConnection->prepareSQLStatement(GLOBAL_PARAMETER_UPDATE_16201,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GLOBAL_PARAMETER_UPDATE_16201,
			getLocalDatabaseName(), databaseConnection->escapeAQSQLString(m_name), databaseConnection->escapeAQSQLString(m_value),
			m_subsystemKey, m_key);
        databaseConnection->executeModification(strSql);

        //now need to update the date fields
        /*sql.str("");
        sql << " select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS')"
            << " from GLOBAL_PARAMETER where GPARAM_ID = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), GLOBAL_PARAMETER_Oracle_SELECT_16056, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(GLOBAL_PARAMETER_SELECT_16056, m_key);

		databaseConnection->prepareSQLStatement(strSql, GLOBAL_PARAMETER_SELECT_16056, m_key);

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DATE_MODIFIED");

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
		    reasonMessage << "No data found for pKey = " << m_key;			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for pKey = " << m_key;			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"primary key") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,"DATE_CREATED");
            m_dateModified = data->getDateData(0,"DATE_MODIFIED");
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


    void GlobalParameterHelper::validateKeyUniqueness()
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First ensure the uniqueness of the record.
        //
        ////////////////////////////////////////////////////////////////////////////

        if (m_isNew)
        {
			IDatabase *pDatabaseConn = DatabaseFactory::getInstance().getDatabase(GlobalParameter_Cd, Read);
            /*std::ostringstream sql;
            sql << "select GPARAM_ID from GLOBAL_PARAMETER where GPARAM_ID = " << m_key;*/
//			std::string strSql = defPrepareSQLStatement(pDatabaseConn->GetDbServerType(), GLOBAL_PARAMETER_STD_SELECT_16001, m_key);
//			std::string strSql  = pDatabaseConn->prepareSQLStatement(GLOBAL_PARAMETER_SELECT_16001, m_key);
		SQLStatement strSql;
		pDatabaseConn->prepareSQLStatement(strSql, GLOBAL_PARAMETER_SELECT_16001, m_key);

            // Set up the vector to pass to exectueQuery
            std::vector<std::string> columnNames;
            columnNames.push_back("GPARAM_ID");

            // execute the query
            IData* data = pDatabaseConn->executeQuery(strSql,columnNames);
        
            if (0 != data->getNumRows())
            {
                // Key already exists
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A GLOBAL_PARAMETER record exists with GPARAM_ID=" << m_key;
                TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"Primary key"));
            }

            delete data;
            data = NULL;

        }
        else
        { 
            // Do nothing
        }

    }


    void GlobalParameterHelper::validateNameSubsystemUniqueness()
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // Ensure uniqueness via SubsystemKey and Name (case insensitive).
        //
        ////////////////////////////////////////////////////////////////////////////
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(GlobalParameter_Cd, Read);
        /*std::ostringstream sql;
        sql << "select GPARAM_ID from GLOBAL_PARAMETER where UPPER(NAME) = '"
            << databaseConnection->escapeInsertString(toUpper(m_name))
            << "' and SUBSYSTEMKEY=" << m_subsystemKey
            << " and GPARAM_ID<>" << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), GLOBAL_PARAMETER_STD_SELECT_16002,
//		std::string strSql  = databaseConnection->prepareSQLStatement(GLOBAL_PARAMETER_SELECT_16002,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GLOBAL_PARAMETER_SELECT_16002,
			databaseConnection->escapeInsertString(toUpper(m_name)), m_subsystemKey, m_key);

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("GPARAM_ID");

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 != data->getNumRows())
        {
            // SubsystemKey and Name combination already exists
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "An GLOBAL_PARAMETER record exists with " 
                          << "Name-Subsystem = " 
                          << m_name << "-" << m_subsystemKey;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE, "Name-Subsystem (case-insensitive)"));
        }
        
        delete data;
        data = NULL;
    }

	std::string GlobalParameterHelper::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
				std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, GlobalParameter_Cd, Write);

                /*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(GlobalParameter_Cd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

} // closes TA_Base_Core


