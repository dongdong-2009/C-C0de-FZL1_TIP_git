/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportDataHelper.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#include "EquipReportDataHelper.h"

#include "core/utilities/src/RunParams.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/uuid/src/TAuuid.h"

namespace TA_Base_Core
{

    EquipReportDataHelper::EquipReportDataHelper() 
        : m_Key(0),
          m_stringId(""),
		  m_ReportKey(""), //TD16289 Mintao++
		  m_DataNodeKey(0),
		  m_DataPointKey(0),
		  m_isValidData(false),
          m_isNew(true) 
    {
        m_stringId = TA_Base_Core::TA_Uuid::generateUUID();
    }

    EquipReportDataHelper::EquipReportDataHelper(std::string& key) 
        : m_Key(0),
          m_stringId(key),
		  m_ReportKey(""), //TD16289 Mintao++
		  m_DataNodeKey(0),
		  m_DataPointKey(0),
		  m_isValidData(false),
          m_isNew(false) {}

	EquipReportDataHelper::EquipReportDataHelper(std::string& key, std::string& reportKey)
        : m_Key(0),            
          m_stringId(key),
		  m_ReportKey(reportKey),
          m_isValidData(false),
		  m_isNew(false) {}

    EquipReportDataHelper::EquipReportDataHelper(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_Key(0),
        m_isNew(false),
        m_isValidData(false)
    {
        FUNCTION_ENTRY("EquipReportDataHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }

	EquipReportDataHelper::EquipReportDataHelper(const EquipReportDataHelper& theEquipReportDataToCopy)
	{
	 	m_Key = 0;
        m_stringId = TA_Base_Core::TA_Uuid::generateUUID();
		m_ReportKey = theEquipReportDataToCopy.m_ReportKey;
		m_DataNodeKey = theEquipReportDataToCopy.m_DataNodeKey;
		m_DataPointKey = theEquipReportDataToCopy.m_DataPointKey;
		m_isValidData = theEquipReportDataToCopy.m_isValidData;
		m_isNew = true;
    }

	void EquipReportDataHelper::addNewEquipReportData()
	{
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
        // Now we can formulate our SQL string
		/*std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
        formatSQL << "insert into SC_EQUIP_REPORT_ENTITIES (SCEREN_ID, SCEREP_ID, DATAPOINT_ID, DATANODE_ID) ";
        formatSQL << "values (''" << m_stringId << "'',''" << m_ReportKey << "''," << m_DataPointKey << "," << m_DataNodeKey << ")";
        formatSQL << "'); ";
        formatSQL << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_Oracle_INSERT_15351,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_INSERT_15351,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_INSERT_15351,
			getLocalDatabaseName(),m_stringId, m_ReportKey, m_DataPointKey, m_DataNodeKey);

        databaseConnection->executeModification(strSql);  
		
        // now need to get the PKEY back out for this equipment report data
		// we base it on the date when the record was created as a unique identifier
        /*std::ostringstream sql;
		sql << "select SCEREN_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from SC_EQUIP_REPORT_ENTITIES ";
		sql << "where SCEREP_ID = '" << m_ReportKey << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_Oracle_SELECT_15051, m_ReportKey);
//		strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_SELECT_15051, m_ReportKey);

		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_SELECT_15051, m_ReportKey);

        // set up the column
		std::vector<std::string> columnNames;
		std::string keyColumn		= "SCEREN_ID";
		std::string createdColumn	= "DATE_CREATED";
		columnNames.push_back(keyColumn);
        columnNames.push_back(createdColumn);
        
		// execute the query
		IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No equipment report data found for Report ID  = " << m_ReportKey;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_stringId = data->getStringData(0,keyColumn);
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
	}

	void EquipReportDataHelper::applyChanges()
	{
        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.
        std::vector<std::string> fieldNames;
        if (m_ReportKey.empty())
        {
            fieldNames.push_back("SCEREP_ID");
        }
        
		// Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("Equip Report data not fully specified. Report data cannot be written to database" ,fieldNames) );
        }

        // Write the report to the database

        // Need to check if this is a new entity or an existing entity - 
        // as a new entity will be inserted, while an existing entity will be updated
        if (m_isNew)
        {
             // This is a new entity
            addNewEquipReportData();
        }
        else 
        {
            // This is an existing entity
            modifyExistingEquipReportData();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;
	}

	void EquipReportDataHelper::deleteThisEquipReportData()
	{
		TA_ASSERT(!m_isNew, "This equipment report data does not yet exist in the database, and therefore cannot be deleted");
        
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
        
		// We must now attempt to delete the equipment report
		/*std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
        formatSQL << "delete SC_EQUIP_REPORT_ENTITIES where SCEREN_ID = ''" << m_stringId << "''";
        formatSQL << "'); ";
        formatSQL << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_Oracle_DELETE_15451, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_DELETE_15451, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_DELETE_15451,
			getLocalDatabaseName(), m_stringId);

		databaseConnection->executeModification(strSql);
	}

	void EquipReportDataHelper::invalidate()
	{
		// This method cannot be called until the key has been set
		TA_ASSERT(!m_isNew,"Attempted to call Invalidate() on a new entity");

		m_isValidData = false;
	}

	void EquipReportDataHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

		columnNames.push_back("SCEREN_ID");
		columnNames.push_back("SCEREP_ID");
		columnNames.push_back("DATANODE_ID");
		columnNames.push_back("DATAPOINT_ID");
	}

	void EquipReportDataHelper::reload()
	{
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

		// set up the sql string
        /*std::ostringstream sql;
        sql << "select SCEREN_ID, SCEREP_ID, DATANODE_ID, DATAPOINT_ID ";
		sql << "from SC_EQUIP_REPORT_ENTITIES where SCEREN_ID = '" << m_stringId << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_STD_SELECT_15004, m_stringId);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_SELECT_15001, m_stringId);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_SELECT_15001, m_stringId);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        getColumnNames(columnNames);
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( strSql, columnNames );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "No equipment report data found for pkey = " << m_stringId;
			TA_THROW(DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }


        TA_ASSERT(data->getNumRows() == 1, "Invalid number of equipment report data records."); 
        reloadUsing(0, *data);
		
		LOG ( SourceInfo, DebugUtil::FunctionExit, "EquipReportDataHelper::reload" );
	}


    void EquipReportDataHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_stringId	= data.getStringData( row, columnNames[0] );
		m_ReportKey	= data.getStringData( row, columnNames[1] );
		
		m_DataNodeKey = data.getUnsignedLongData( row, columnNames[2],0);
    	m_DataPointKey = data.getUnsignedLongData( row, columnNames[3], 0);
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }
	

    void EquipReportDataHelper::setReportKey(std::string& aReportKey)
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_ReportKey = aReportKey;
	}

	void EquipReportDataHelper::setDataNodeKey(const unsigned long aDataNodeKey)
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_DataNodeKey= aDataNodeKey;
	}

	void EquipReportDataHelper::setDataPointKey(const unsigned long aDataPointKey)
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_DataPointKey = aDataPointKey;
	}

    std::string EquipReportDataHelper::getReportKey()
	{
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_ReportKey;
	}

    std::string EquipReportDataHelper::getId()
	{
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_stringId;
	}

	unsigned long EquipReportDataHelper::getDataNodeKey()
	{
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_DataNodeKey;
	}

	unsigned long EquipReportDataHelper::getDataPointKey()
	{
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_DataPointKey;
	}

	unsigned long EquipReportDataHelper::getKey()
	{
		if (!m_isValidData && !m_isNew)  
		{            
			reload();
		}

		return m_Key;
	}

	void EquipReportDataHelper::modifyExistingEquipReportData()
	{
		// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

		/*std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
        formatSQL << "update SC_EQUIP_REPORT_ENTITIES set SCEREP_ID = ''" << m_ReportKey;
		formatSQL << "'', DATANODE_ID = " << m_DataNodeKey;
		formatSQL << ", DATAPOINT_ID = " << m_DataPointKey;
        formatSQL << " where SCEREN_ID = ''" << m_stringId;
        formatSQL << "'''); ";
        formatSQL << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_EQUIP_REPORT_ENTITIES_Oracle_UPDATE_15201,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_EQUIP_REPORT_ENTITIES_UPDATE_15201,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_EQUIP_REPORT_ENTITIES_UPDATE_15201,
			getLocalDatabaseName(), m_ReportKey, m_DataNodeKey, m_DataPointKey, m_stringId);

        databaseConnection->executeModification(strSql);
	}

	std::string EquipReportDataHelper::getLocalDatabaseName()
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
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Scada_Cd, Write);

               /* DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Scada_Cd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }


} // TA_Base_Core
