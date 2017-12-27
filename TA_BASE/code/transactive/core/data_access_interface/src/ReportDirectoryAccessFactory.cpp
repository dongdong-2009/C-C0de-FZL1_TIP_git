/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/ReportDirectoryAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/10/27 16:19:08 $
  * Last modified by:  $Author: hui.wang $
  * 
  * ReportDirectoryAccessFactory is a singleton that is used to retrieve ReportDirectory objects either from the
  * database or newly created.
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/src/ReportDirectoryAccessFactory.h"
#include "core/data_access_interface/src/ReportDirectory.h"
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
    ReportDirectoryAccessFactory* ReportDirectoryAccessFactory::m_instance = 0;

    ReportDirectoryAccessFactory& ReportDirectoryAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ReportDirectoryAccessFactory();
        }
        return *m_instance;
    }

    IReportDirectory* ReportDirectoryAccessFactory::getReportDirectory(bool isReadOnly)
    {
        return new ReportDirectory(isReadOnly);
    }

	std::vector<IReportDirectory*> ReportDirectoryAccessFactory::getAllReportDirectories()
	{
		// Perform SQL here
		std::vector< IReportDirectory* > reportDirectorys;
		//std::ostringstream sql;
		
		// create the SQL string to retrieve all the Subsystems of the alarm
        //sql << "select RP_DIR_ID,RP_DIR_NAME,RP_PARENT_DIR_ID from RM_REPORT_DIRECTORY";
		//getReportDirectorys( sql.str(), reportDirectorys );

		TA_Base_Core::SQLStatement sql;
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_DIRECTORY_SELECT_140001);
		getReportDirectorys( sql, reportDirectorys );
        
		return reportDirectorys;
	}

	std::vector<IReportDirectory*> ReportDirectoryAccessFactory::getAllReportDirsInSpecifiedParentDirId( unsigned long parentDirId )
	{
		// Perform SQL here
		std::vector< IReportDirectory* > reportDirectorys;
		//std::ostringstream sql;

		// create the SQL string to retrieve all the Subsystems of the alarm
		//sql << "select RP_DIR_ID,RP_DIR_NAME,RP_PARENT_DIR_ID from RM_REPORT_DIRECTORY where RP_PARENT_DIR_ID=";
		//sql << parentDirId;
		//getReportDirectorys( sql.str(), reportDirectorys );

		TA_Base_Core::SQLStatement sql;
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_DIRECTORY_SELECT_140002, parentDirId);
		getReportDirectorys( sql, reportDirectorys );

        
		return reportDirectorys;
	}

    std::string ReportDirectoryAccessFactory::getLocalDatabaseName()
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

    void ReportDirectoryAccessFactory::deleteReportDirectory(IReportDirectory* reportDirectory)
    {
        if (reportDirectory == NULL)
        {
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Write);

        /*std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
	    formatSQL << "delete from RM_REPORT_DIRECTORY where RP_DIR_ID = ''"<< reportDirectory->getReportDirectoryId() <<"''";	
        formatSQL << "');";
        formatSQL << "END;";

        databaseConnection->executeModification(formatSQL.str());*/

		//AQ:('%s','PUBLIC', 'delete from RM_REPORT_DIRECTORY where RP_DIR_ID =''%s''')
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_DIRECTORY_DELETE_140003, getLocalDatabaseName(), reportDirectory->getReportDirectoryId());
		databaseConnection->executeModification(sql);

    }
	
	//void ReportDirectoryAccessFactory::getReportDirectorys( const std::string& sql, std::vector<IReportDirectory*>& reportDirectorys )
	void ReportDirectoryAccessFactory::getReportDirectorys( TA_Base_Core::SQLStatement& sql, std::vector<IReportDirectory*>& reportDirectorys )
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Start ReportDirectoryAccessFactory::getReportDirectorys...");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        // Set up the columnNames vector to be passed to executeQuery()
		std::string directoryIdColumn = "RP_DIR_ID";
		std::string directoryNameColumn = "RP_DIR_NAME";
		std::string directoryParentDirIdColumn = "RP_PARENT_DIR_ID";

        std::vector<std::string> columnNames;
		columnNames.push_back(directoryIdColumn);
		columnNames.push_back(directoryNameColumn );
		columnNames.push_back(directoryParentDirIdColumn );

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
                    IReportDirectory* reportDirectory = NULL;
                    //reportDirectory = new ReportDirectory( data->getUnsignedLongData( i, directoryIdColumn ),
							             //  data->getStringData(i, directoryNameColumn),
                    //                       data->getUnsignedLongData(i, directoryParentDirIdColumn ),
                    //                       true); // false indicates not a new ReportDirectory
					
					unsigned long directoryId = data->getUnsignedLongData(i,directoryIdColumn);
					std::string directoryName = data->getStringData(i,directoryNameColumn);
					unsigned long directoryParentDirId = data->getUnsignedLongData(i,directoryParentDirIdColumn);

					reportDirectory = new ReportDirectory(directoryId,directoryName,directoryParentDirId,true);

					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"In ReportDirectoryAccessFactory::getReportDirectorys, directoryId = %ld, directoryName = %s, directoryParentDirId = %ld",
						directoryId,directoryName.c_str(),directoryParentDirId);

                    reportDirectorys.push_back( reportDirectory );
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

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"End ReportDirectoryAccessFactory::getReportDirectorys...");
	}

	bool ReportDirectoryAccessFactory::isValidReportDirectoryId( unsigned long reportDirectoryId )
	{
		if ( reportDirectoryId == ULONG_MAX || reportDirectoryId < 0)
		{
			return false;
		}
		return true;
	}

    unsigned long ReportDirectoryAccessFactory::getParentDirIdFromReportDirectoryId( unsigned long reportDirectoryId)
    {
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
        std::string keyColumn     ("RP_PARENT_DIR_ID");
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        
        /*std::ostringstream sql;        
        sql << "select RP_PARENT_DIR_ID from RM_REPORT_DIRECTORY where RP_DIR_ID = '" << reportDirectoryId <<"'";
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);*/
		//select RP_PARENT_DIR_ID from RM_REPORT_DIRECTORY where RP_DIR_ID='%s'
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, RM_REPORT_DIRECTORY_SELECT_140004, reportDirectoryId);
		IData* data = databaseConnection->executeQuery(sql,columnNames);
        
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
