/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/ReportFile.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2011/11/16 18:23:22 $
  * Last modified by:  $Author: hui.wang $
  * 
  * ReportFile is an implementation of IReportFile. It holds the data specific to an ReportFile entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ReportFile.h"
#include "core/data_access_interface/src/ReportFileAccessFactory.h"
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
    ReportFile::ReportFile(bool isReadOnly)
        : m_reportFileId( ULONG_MAX ),
          m_reportFileName( "" ),
		  m_reportFilePhysicalPath( "" ),
	      m_reportDirectoryId( ULONG_MAX ),
		  m_isReadOnly(isReadOnly),
		  m_isValidData(false)
	{
    }

	ReportFile::ReportFile( unsigned long reportFileID, std::string reportFileName,
		std::string reportFilePhysicalPath, unsigned long reportDirectoryID, bool isReadOnly/*=true*/)
		: m_reportFileId( reportFileID ),
		  m_reportFileName( reportFileName ),
		  m_reportFilePhysicalPath( reportFilePhysicalPath ),
		  m_reportDirectoryId( reportDirectoryID ),
		  m_isReadOnly(isReadOnly),
		  m_isValidData(false)
	{
	}

    ReportFile::~ReportFile()
    {
    }

	unsigned long ReportFile::getReportFileId()
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportFile::getReportFileId...");

		if ( ! m_isValidData  && m_isReadOnly )
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportFile::getReportFileId, call reload()");

			reload();
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportFile::getReportFileId...");

		return m_reportFileId;
	}

	std::string ReportFile::getReportFileName()
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportFile::getReportFileName...");

		if ( ! m_isValidData  && m_isReadOnly )
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportFile::getReportFileName, call reload()");

			reload();
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportFile::getReportFileName...");

		return m_reportFileName;
	}

	std::string ReportFile::getReportFilePhysicalPath()
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportFile::getReportFilePhysicalPath...");

		if ( ! m_isValidData  && m_isReadOnly )
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportFile::getReportFilePhysicalPath, call reload()");

			reload();
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportFile::getReportFilePhysicalPath...");

		return m_reportFilePhysicalPath;
	}

	unsigned long ReportFile::getReportFileDirectoryId()
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportFile::getReportFileDirectoryId...");

		if ( ! m_isValidData  && m_isReadOnly )
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportFile::getReportFileDirectoryId, call reload()");

			reload();
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportFile::getReportFileDirectoryId...");

		return m_reportDirectoryId;
	}

    void ReportFile::invalidate()
    {
		m_reportFileId = ULONG_MAX ;
		m_reportFileName = "" ;
		m_reportFilePhysicalPath = "";
		m_reportDirectoryId = ULONG_MAX;
		m_isReadOnly = false;
    }

	
    std::string ReportFile::getLocalDatabaseName()
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

    void ReportFile::applyChanges()
	{
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ReportFile::applyChanges" );     
		TA_ASSERT( m_reportFileId > 0, "Report File ID not set, can not apply" );

		// NOTE: Report File can only be created or destroyed - nothing else. 
		// There is no need to handle updating records, only creating new records... UNLESS
		// were are updating the HAS_ENDED column ...

        // Write the report file infomation to the database

		// get a connection to the database
		IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

		LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			"Writing new report file (id: %ld) to the database.",
			m_reportFileId);

        /*std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		formatSQL << "insert into RM_REPORT_FILE (RP_FILE_ID, RP_FILE_NAME, RP_PHYSICAL_PATH, RP_DIR_ID) ";
		formatSQL << "values (''" << m_reportFileId << "'',"<< m_reportFileName <<","<< m_reportFilePhysicalPath <<","<< m_reportDirectoryId;
		formatSQL << "');";
		formatSQL << "END;";*/
				
		// This will throw an exception upon failure - ready to catch??
		//databaseConnection->executeModification(formatSQL.str());


		//AQ:('%s','PUBLIC', 'insert into RM_REPORT_FILE (RP_FILE_ID, RP_FILE_NAME, RP_PHYSICAL_PATH, RP_DIR_ID) values(%s, ''%s'',''%s'', %s)')
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_FILE_INSERT_150005, 
			getLocalDatabaseName(), m_reportDirectoryId, m_reportFileId, m_reportFileName, m_reportFilePhysicalPath, m_reportDirectoryId);
		databaseConnection->executeModification(sql);

		// Set the data to be valid - no need to force a reload after a succesful write
		m_isValidData = true;
	}


    void ReportFile::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );
 
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportFile::reload...");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        /*char sql[256];
		sprintf(sql,"select RP_FILE_ID,RP_FILE_NAME,RP_PHYSICAL_PATH,RP_DIR_ID from RM_REPORT_FILE where RP_FILE_ID = '%d'",
			m_reportFileId);*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_FILE_SELECT_150006, m_reportFileId);

        // Set up the columnNames vector to be passed to executeQuery()
		std::string fileIdColumn = "RP_FILE_ID";
		std::string fileNameColumn = "RP_FILE_NAME";
		std::string filePhysicalPathColumn = "RP_PHYSICAL_PATH";
		std::string directoryIdColumn = "RP_DIR_ID";

        std::vector<std::string> columnNames;
        columnNames.push_back(fileIdColumn);
        columnNames.push_back(fileNameColumn );
		columnNames.push_back(filePhysicalPathColumn );
		columnNames.push_back(directoryIdColumn);

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
			sprintf(reasonMessage, "No Report File found for ID = %ld", m_reportFileId );			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            char reasonMessage[256];
			sprintf(reasonMessage, "More than one Report File found with ID = %ld", m_reportFileId );
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_reportFileId = data->getUnsignedLongData(0,columnNames[0]);
		m_reportFileName = data->getStringData(0,columnNames[1]);
		m_reportFilePhysicalPath = data->getStringData(0,columnNames[2]);
		m_reportDirectoryId = data->getUnsignedLongData(0,columnNames[3]);

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportFile::reload, m_reportFileId=%ld, m_reportFileName = %s, m_reportFilePhysicalPath = %s, m_reportDirectoryId = %ld",
			m_reportFileId,m_reportFileName.c_str(),m_reportFilePhysicalPath.c_str(),m_reportDirectoryId);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportFile::reload...");
        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }


} // closes TA_Base_Core
