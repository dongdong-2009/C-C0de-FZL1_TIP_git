/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/ReportDirectory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2011/11/16 18:23:22 $
  * Last modified by:  $Author: hui.wang $
  * 
  * ReportDirectory is an implementation of IReportDirectory. It holds the data specific to an ReportDirectory entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ReportDirectory.h"
#include "core/data_access_interface/src/ReportDirectoryAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{
    ReportDirectory::ReportDirectory(bool isReadOnly)
        : m_reportDirectoryId( ULONG_MAX ),
		  m_reportDirectoryName( "" ),
	      m_reportParentDirectoryId( ULONG_MAX ),
		  m_isReadOnly(isReadOnly),
		  m_isValidData(false)
	{
    }

	ReportDirectory::ReportDirectory( unsigned long reportDirectoryId, std::string reportDirectoryName,
		unsigned long reportParentDirectoryId, bool isReadOnly/*=true*/)
		: m_reportDirectoryId( reportDirectoryId ),
		  m_reportDirectoryName( reportDirectoryName ),
		  m_reportParentDirectoryId( reportParentDirectoryId ),
		  m_isReadOnly(isReadOnly),
		  m_isValidData(false)
	{
	}

    ReportDirectory::~ReportDirectory()
    {
    }

	unsigned long ReportDirectory::getReportDirectoryId()
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportDirectory::getReportDirectoryId...");
		if ( ! m_isValidData  && m_isReadOnly )
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportDirectory::getReportDirectoryId, call reload()");

			reload();
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportDirectory::getReportDirectoryId...");
		return m_reportDirectoryId;
	}

	std::string ReportDirectory::getReportDirectoryName()
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportDirectory::getReportDirectoryName...");
		if ( ! m_isValidData  && m_isReadOnly )
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportDirectory::getReportDirectoryName, call reload()");

			reload();
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportDirectory::getReportDirectoryName...");
		return m_reportDirectoryName;
	}

	unsigned long ReportDirectory::getReportParentDirectoryId()
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportDirectory::getReportParentDirectoryId...");
		if ( ! m_isValidData  && m_isReadOnly )
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportDirectory::getReportParentDirectoryId, call reload()");

			reload();
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportDirectory::getReportParentDirectoryId...");
		return m_reportParentDirectoryId;
	}

    void ReportDirectory::invalidate()
    {
		m_reportDirectoryId = ULONG_MAX ;
		m_reportDirectoryName = "" ;
		m_reportParentDirectoryId = ULONG_MAX;
		m_isReadOnly = false;
    }

	
    std::string ReportDirectory::getLocalDatabaseName()
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
                DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(OnlineSecurity_Cd, Write);
                m_localDatabase = connections[0].first;  
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

    void ReportDirectory::applyChanges()
	{
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ReportDirectory::applyChanges" );     
		TA_ASSERT( m_reportDirectoryId > 0, "Report Directory ID not set, can not apply" );

		// NOTE: Report Directory can only be created or destroyed - nothing else. 
		// There is no need to handle updating records, only creating new records... UNLESS
		// were are updating the HAS_ENDED column ...

        // Write the report directory infomation to the database

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

		LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			"Writing new report directory (id: %ld) to the database.",
			m_reportParentDirectoryId);

        /*std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		formatSQL << "insert into RM_REPORT_DIRECTORY (RP_DIR_ID, RP_DIR_NAME, RP_PARENT_DIR_ID) ";
		formatSQL << "values (''" << m_reportDirectoryId << "'',"<< m_reportDirectoryName <<","<< m_reportParentDirectoryId ;
		formatSQL << "');";
		formatSQL << "END;";*/			
		// This will throw an exception upon failure - ready to catch??
		//databaseConnection->executeModification(formatSQL.str());

		//AQ:('%s','PUBLIC', 'insert into RM_REPORT_DIRECTORY (RP_DIR_ID, RP_DIR_NAME, RP_PARENT_DIR_ID) values(%s, ''%s'', %s)')
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_DIRECTORY_INSERT_140006, 
			getLocalDatabaseName(), m_reportDirectoryId, m_reportDirectoryName, m_reportParentDirectoryId);
		databaseConnection->executeModification(sql);

		// Set the data to be valid - no need to force a reload after a succesful write
		m_isValidData = true;
	}


    void ReportDirectory::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );
 
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportDirectory::reload...");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        /*char sql[256];
		sprintf(sql,"select RP_DIR_ID,RP_DIR_NAME,RP_PARENT_DIR_ID from RM_REPORT_DIRECTORY where RP_DIR_ID = '%d'",
			m_reportDirectoryId);*/

		//select RP_DIR_ID,RP_DIR_NAME,RP_PARENT_DIR_ID from RM_REPORT_DIRECTORY where RP_DIR_ID ='%s'
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_DIRECTORY_SELECT_140005, m_reportDirectoryId);

        // Set up the columnNames vector to be passed to executeQuery()
		std::string dirIdColumn = "RP_DIR_ID";
		std::string dirNameColumn = "RP_DIR_NAME";
		std::string dirParentIdColumn = "RP_PARENT_DIR_ID";

        std::vector<std::string> columnNames;
        columnNames.push_back(dirIdColumn);
        columnNames.push_back(dirNameColumn );
		columnNames.push_back(dirParentIdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            char reasonMessage[256];
			sprintf(reasonMessage, "No Report Directory found for ID = %ld", m_reportDirectoryId );			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            char reasonMessage[256];
			sprintf(reasonMessage, "More than one Report Directory found with ID = %ld", m_reportDirectoryId );
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_reportDirectoryId = data->getUnsignedLongData(0,columnNames[0]);
		m_reportDirectoryName = data->getStringData(0,columnNames[1]);
		m_reportParentDirectoryId = data->getUnsignedLongData(0,columnNames[2]);

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportDirectory::reload, m_reportDirectoryId = %ld, m_reportDirectoryName = %s, m_reportParentDirectoryId = %ld",
			m_reportDirectoryId,(m_reportDirectoryName).c_str(),m_reportParentDirectoryId);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportDirectory::reload...");

        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }


} // closes TA_Base_Core
