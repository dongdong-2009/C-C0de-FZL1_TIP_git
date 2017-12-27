/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaDvaMessageVersionHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaDvaMessageVersionHelper is an object that is held by PaDvaMessageVersion and ConfigPaDvaMessageVersion objects. 
  * It contains all data used by PaDvaMessageVersionGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessageVersion and ConfigPaDvaMessageVersion.
  *
  */

#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaDvaMessageVersionHelper.h"
#include "core/data_access_interface/pa/src/PaDvaMessageVersionAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"


namespace TA_Base_Core
{
    const char* const PA_DVA_MESSAGE_VERSION_TABLE_NAME = "PA_DVA_MESSAGE_VERSION";
    const char* const PA_DVA_MESSAGE_VERSION_TABLE_SHORT_NAME = "PADMVE";
    const char* const PA_DVA_MESSAGE_VERSION_SEQUENCE = "PADMVE_SEQ";
    const char* const KEY_COL = "PADMVE_ID";
    const char* const VERSION_NUMBER_COL = "VERSION_NUMBER";
    const char* const LOCATIONKEY_COL = "LOCATIONKEY";
    const char* const DATEMODIFIED_COL = "DATE_MODIFIED";
    const char* const DATECREATED_COL = "DATE_CREATED";

	PaDvaMessageVersionHelper::PaDvaMessageVersionHelper( const PaDvaMessageVersionHelper& thePaDvaMessageVersionHelper)
		: // m_key(),   // Don't set the key upon copy..
          m_locationKey(thePaDvaMessageVersionHelper.m_locationKey),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
	}


    PaDvaMessageVersionHelper::PaDvaMessageVersionHelper(const unsigned long key)
        : m_key(key),	
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    PaDvaMessageVersionHelper::PaDvaMessageVersionHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),	
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        reloadUsing(row, data);
    }


    PaDvaMessageVersionHelper::PaDvaMessageVersionHelper()
        : m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    PaDvaMessageVersionHelper::~PaDvaMessageVersionHelper()
    {
    }
  

    unsigned long PaDvaMessageVersionHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        if (!m_key.hasBeenSet())
        {
            TA_THROW(TA_Base_Core::DataException("Key not set",  
                            TA_Base_Core::DataException::NO_VALUE, "Primary key"));
        }        

        FUNCTION_EXIT;
        return m_key.getValue();
    }


    std::string PaDvaMessageVersionHelper::getVersionData()
    {
        FUNCTION_ENTRY("getVersionData" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        return m_strVersion.c_str();
    }


    unsigned long PaDvaMessageVersionHelper::getLocationKey()
    {
        FUNCTION_ENTRY("getLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        // Following the reload of data, all values should have been set
        try
        {
            return m_locationKey.getValue();
        }
        catch (::TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_locationKey value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_locationKey value not set after reload.");
            throw;
        }
    }


    void PaDvaMessageVersionHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }


    void PaDvaMessageVersionHelper::setVersionData(const std::string& newVersion)
    {
        FUNCTION_ENTRY("setVersionData" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_strVersion = newVersion.c_str();
        
        FUNCTION_EXIT;
    }


    time_t PaDvaMessageVersionHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t PaDvaMessageVersionHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }



    void PaDvaMessageVersionHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaDvaMessageVersion");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    DatabaseQueryHelper::QueryData 
        PaDvaMessageVersionHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_DVA_MESSAGE_VERSION_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);
        result.columnNames.push_back(VERSION_NUMBER_COL);
        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

       /* result.statement = 
            DatabaseQueryHelper::createBasicSelectStatement(result.columnNames, 
                                                            result.tableName);*/   
		DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement, result.columnNames, 
			result.tableName); 
        return result;
    }


    void PaDvaMessageVersionHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing" );
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
        m_strVersion = data.getStringData( row, VERSION_NUMBER_COL);
        m_dateCreated = data.getDateData( row, DATECREATED_COL );

        try
        {
            m_dateModified = data.getDateData( row, DATEMODIFIED_COL );
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void PaDvaMessageVersionHelper::writePaDvaMessageVersionData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaDvaMessageVersionData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;        
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        
        /*
        // Can have empty version data values ""
        int i;
        for (i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            if (m_versionData[i].length() == 0)
            {
                fieldNames.push_back(VERSION_COLUMNS[i]);
            }
        }*/

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("PaDvaMessageVersion data not fully specified. PaDvaMessageVersion cannot be written to database",
                                                         fieldNames));
        }
        
        // Write the PaDvaMessageVersion to the database

        try 
        {
            // Need to check if this is a new PaDvaMessageVersion or an existing PaDvaMessageVersion - 
            // as a new PaDvaMessageVersion will be inserted, while an existing PaDvaMessageVersion will be updated
            if (m_isNew) // This is a new PaDvaMessageVersion
            {
                addNewPaDvaMessageVersion();
            }
            else // This is an existing PaDvaMessageVersion
            {
                modifyExistingPaDvaMessageVersion();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaDvaMessageVersion data not fully specified. PaDvaMessageVersion cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PaDvaMessageVersionHelper::deletePaDvaMessageVersion()
    {
        FUNCTION_ENTRY("deletePaDvaMessageVersion");

        TA_ASSERT(!m_isNew, "This PaDvaMessageVersion does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        //std::ostringstream sql;

        // Good to go...
       /* sql.str("");
        sql << "delete " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " where " << KEY_COL << " = " << m_key.getValue();*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_DVA_MESSAGE_VERSION_STD_DELETE_43401, m_key.getValue());
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_VERSION_DELETE_43401, getLocalDatabaseName(), m_key.getValue());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_VERSION_DELETE_43401, getLocalDatabaseName(), m_key.getValue());

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void PaDvaMessageVersionHelper::modifyExistingPaDvaMessageVersion() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaDvaMessageVersion");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA DVA Message Version %u already exists. It's data will be updated.", m_key.getValue());


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        std::ostringstream sql;

//         sql << "update " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " set " 
//             << LOCATIONKEY_COL << " = "
//             << m_locationKey.getValue()
//             << ", "
//             << VERSION_NUMBER_COL << " = '" 
//             << (databaseConnection->escapeInsertString(m_strVersion)).c_str() << "'"
//             << " where " << KEY_COL << " = " << m_key.getValue();        
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_VERSION_UPDATE_43151, getLocalDatabaseName(), 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_VERSION_UPDATE_43151, getLocalDatabaseName(), 
			m_locationKey.getValue(), databaseConnection->escapeInsertString(m_strVersion), m_key.getValue());

        databaseConnection->executeModification(strSql);

        // now need to update the date fields
      /*  sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << 
            " where " << KEY_COL << " = " << m_key.getValue();*/
		/*strSql = defPrepareSQLStatement(nDbServerType, PA_DVA_MESSAGE_VERSION_Oracle_SELECT_43051, m_key.getValue());*/
//		strSql = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_VERSION_SELECT_43051, m_key.getValue());

		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_VERSION_SELECT_43051, m_key.getValue());

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA DVA Message Version record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA DVA Message Version key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA DVA Message Version key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0, DATECREATED_COL);
            m_dateModified = data->getDateData(0, DATEMODIFIED_COL);
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


    void PaDvaMessageVersionHelper::addNewPaDvaMessageVersion() 
                    //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPaDvaMessageVersion");

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(databaseConnection,
            PA_DVA_MESSAGE_VERSION_SEQUENCE);

//         std::ostringstream sql;
//         sql << "insert into " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " (" 
//             << KEY_COL << ", "
//             << LOCATIONKEY_COL << ", "
//             << VERSION_NUMBER_COL << ") "
//             << "values (" << sequenceNumber << ", "
//             << m_locationKey.getValue() << ", '"
//             << (databaseConnection->escapeInsertString(m_strVersion)).c_str()
//             << "')";
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_VERSION_INSERT_43301, getLocalDatabaseName(), 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_VERSION_INSERT_43301, getLocalDatabaseName(), 
			sequenceNumber,  m_locationKey.getValue(), databaseConnection->escapeInsertString(m_strVersion));

        databaseConnection->executeModification(strSql);

       /* 
	   sql.str("");
       sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " where "
            << KEY_COL << " = " << sequenceNumber;
		*/
//		strSql = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_VERSION_SELECT_43052,  sequenceNumber);

		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_VERSION_SELECT_43052,  sequenceNumber);

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA DVA Message Version record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA DVA Message Version key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA DVA Message Version key" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key.setValue(data->getUnsignedLongData(0,KEY_COL));
            m_dateCreated = data->getDateData(0,DATECREATED_COL);
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


    std::string PaDvaMessageVersionHelper::getLocalDatabaseName()
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
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Pa_Cd, Write);

               /* DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Pa_Cd, Write);
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

