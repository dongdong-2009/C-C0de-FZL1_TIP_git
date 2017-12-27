/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/ReportFileAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/10/27 16:19:08 $
  * Last modified by:  $Author: hui.wang $
  * 
  * ReportFileAccessFactory is a singleton that is used to retrieve ReportFile objects either from the
  * database or newly created.
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/src/ReportFileAccessFactory.h"
#include "core/data_access_interface/src/ReportFile.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
//#include <sstream>
#include <iomanip>

namespace TA_Base_Core
{
    ReportFileAccessFactory* ReportFileAccessFactory::m_instance = 0;

    ReportFileAccessFactory& ReportFileAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ReportFileAccessFactory();
        }
        return *m_instance;
    }

    IReportFile* ReportFileAccessFactory::getReportFile(bool isReadOnly)
    {
        return new ReportFile(isReadOnly);
    }

	std::vector<IReportFile*> ReportFileAccessFactory::getAllReportFiles()
	{
		// Perform SQL here
		std::vector< IReportFile* > reportFiles;
		/*std::ostringstream sql;;*/

		// create the SQL string to retrieve all the Subsystems of the alarm
        /*sql << "select RP_FILE_ID,RP_FILE_NAME,RP_PHYSICAL_PATH,RP_DIR_ID from RM_REPORT_FILE";
		getReportFiles( sql.str(), reportFiles );*/

		TA_Base_Core::SQLStatement sql;
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_FILE_SELECT_150001);
		getReportFiles( sql, reportFiles );


        
		return reportFiles;
	}

	std::vector<IReportFile*> ReportFileAccessFactory::getAllReportFilesInSpecifiedDirId( unsigned long dirId )
	{
		// Perform SQL here
		std::vector< IReportFile* > reportFiles;
		//std::ostringstream sql;

		// create the SQL string to retrieve all the Subsystems of the alarm
		/*sql << "select RP_FILE_ID,RP_FILE_NAME,RP_PHYSICAL_PATH,RP_DIR_ID from RM_REPORT_FILE where RP_DIR_ID=";
		sql << dirId;
		getReportFiles( sql.str(), reportFiles );*/

		//RM_REPORT_FILE_SELECT_150002
		TA_Base_Core::SQLStatement sql;
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_FILE_SELECT_150002, dirId);
		getReportFiles( sql, reportFiles );

        
		return reportFiles;
	}

    std::string ReportFileAccessFactory::getLocalDatabaseName()
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

    void ReportFileAccessFactory::deleteReportFile(IReportFile* reportFile)
    {
        if (reportFile == NULL)
        {
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        /*std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
	    formatSQL << "delete from RM_REPORT_FILE where RP_FILE_ID = ''"<< reportFile->getReportFileId() <<"''";	
        formatSQL << "');";
        formatSQL << "END;";

        databaseConnection->executeModification(formatSQL.str());*/
		//AQ:('%s','PUBLIC', 'delete from RM_REPORT_FILE where RP_FILE_ID =''%s''')
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_FILE_DELETE_150003, getLocalDatabaseName(), reportFile->getReportFileId() );
		databaseConnection->executeModification(sql);

    }

	void ReportFileAccessFactory::getReportFiles( TA_Base_Core::SQLStatement& sql, std::vector<IReportFile*>& reportFiles )
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportFileAccessFactory::getReportFiles...");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

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

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            return;
        }
	
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					// Create the subsystem
                    IReportFile* reportFile = NULL;
             //       reportFile = new ReportFile( data->getUnsignedLongData( i, fileIdColumn ),
							      //         data->getStringData(i, fileNameColumn),
             //                              data->getStringData(i, filePhysicalPathColumn ),
										   //data->getUnsignedLongData(i, directoryIdColumn ),
             //                              true); // false indicates not a new ReportFile
					unsigned long fileId = data->getUnsignedLongData( i, fileIdColumn );
					std::string fileName = data->getStringData(i, fileNameColumn);
					std::string filePhysicalPath = data->getStringData(i, filePhysicalPathColumn );
					unsigned long directoryId = data->getUnsignedLongData(i, directoryIdColumn );

					reportFile = new ReportFile(fileId,fileName,filePhysicalPath,directoryId,true);

					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportFileAccessFactory::getReportFiles, fileId = %ld, fileName = %s, filePhysicalPath = %s, directoryId = %ld",
						fileId,fileName.c_str(),filePhysicalPath.c_str(),directoryId);
					
                    reportFiles.push_back( reportFile );
                }
                catch (const TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportFileAccessFactory::getReportFiles...");
	}

	bool ReportFileAccessFactory::isValidReportFileId( unsigned long reportFileId )
	{
		if ( reportFileId == ULONG_MAX || reportFileId < 0)
		{
			return false;
		}
		return true;
	}

    unsigned long ReportFileAccessFactory::getDirIdFromReportFileId( unsigned long reportFileId)
    {
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
        std::string keyColumn     ("RP_DIR_ID");
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        
        /*std::ostringstream sql;        
        sql << "select RP_DIR_ID from RM_REPORT_FILE where RP_FILE_ID = '" << reportFileId <<"'";
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);*/

		//RM_REPORT_FILE_SELECT_150004=select RP_DIR_ID from RM_REPORT_FILE where RP_FILE_ID='%s'
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_FILE_SELECT_150004, reportFileId);
		IData* data = databaseConnection->executeQuery(sql, columnNames);

        
        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            
            return false;
        }
        
        if (data->getNumRows()==1)
        {
            return data->getUnsignedLongData(0, keyColumn);
        }
        
        return false;
    }
    
} // closes TA_Base_Core
