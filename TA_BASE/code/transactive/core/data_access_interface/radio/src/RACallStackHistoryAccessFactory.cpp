 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RACallStackHistoryAccessFactory.cpp $
  * @author Glen Kidd
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * RACallStackHistoryAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IRACallStackHistory interface.
  */
#if 0
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif
#endif

#include "ace/OS.h"

#include <sstream>
#include <time.h>

#include "core/data_access_interface/radio/src/RACallStackHistoryAccessFactory.h"
#include "core/data_access_interface/radio/src/RACallStackHistory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    TA_Base_Core::NonReEntrantThreadLockable RACallStackHistoryAccessFactory::m_singletonLock;
	RACallStackHistoryAccessFactory* RACallStackHistoryAccessFactory::s_instance = NULL;


    RACallStackHistoryAccessFactory& RACallStackHistoryAccessFactory::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if (s_instance == NULL)
        {
            s_instance = new RACallStackHistoryAccessFactory;
        }
        return *s_instance;
    }


    RACallStackHistoryAccessFactory::~RACallStackHistoryAccessFactory()
    {

    }


    RACallStackHistoryAccessFactory::CallStackHistoryList RACallStackHistoryAccessFactory::getList(const std::string & profile, const unsigned long & locationKey, const bool readWrite ) const
    {
		// Perform SQL here
		std::vector< IRACallStackHistory* > records;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

		// create the SQL string 
        //std::string sql("select PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID from TA_SESSION");
   //     std::stringstream s;
   //     s   << "select "
   //         << "  RACSHI_ID, "
   //         << "  PROFILE_NAME, "
   //         << "  LOCATION_KEY, "
   //         << "  to_char(TIMESTAMP,'YYYYMMDDHH24MISS') TIMESTAMP, "
   //         << "  ORIGIN, "
			//<< "  CALL_TYPE, "
			//<< "  IDENTIFIER, "
			//<< "  LOCATION, "
			//<< "  ISEMERGENCY" //td17294
   //         << " from "
   //         << "     RA_CALL_STACK_HISTORY "
   //         << " where "
			//<< "   PROFILE_NAME = '" << databaseConnection->escapeQueryString(profile) << "' "
			//<< " and LOCATION_KEY = " << locationKey 
			//<< " and rownum < " << RACallStackHistory::rowLimit
			//<< " order by TIMESTAMP asc"
   //         << std::ends;
   //     std::string sql(s.str()); 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_CALL_STACK_HISTORY_Oracle_SELECT_34051,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_CALL_STACK_HISTORY_SELECT_34051,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_CALL_STACK_HISTORY_SELECT_34051,
			databaseConnection->escapeQueryString(profile), locationKey, RACallStackHistory::rowLimit);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string colRACSHI_ID("RACSHI_ID");
        std::string colPROFILE("PROFILE");
        std::string colLOCATION_KEY("LOCATION_KEY");
        std::string colTIMESTAMP("TIMESTAMP");
        std::string colORIGIN("ORIGIN");
        std::string colCALL_TYPE("CALL_TYPE");
        std::string colIDENTIFIER("IDENTIFIER");
        std::string colLOCATION("LOCATION");
		// td17294
		std::string colISEMERGENCY("ISEMERGENCY");

        std::vector<std::string> columnNames;
		columnNames.push_back(colRACSHI_ID);
		columnNames.push_back(colPROFILE);
		columnNames.push_back(colLOCATION_KEY);
        columnNames.push_back(colTIMESTAMP);
        columnNames.push_back(colORIGIN);
        columnNames.push_back(colCALL_TYPE);
        columnNames.push_back(colIDENTIFIER);
        columnNames.push_back(colLOCATION);
		columnNames.push_back(colISEMERGENCY); // td17294

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
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
                    IRACallStackHistory* record = new RACallStackHistory( 
						data->getUnsignedLongData(i,colRACSHI_ID),
						data->getStringData(i,colPROFILE),
						data->getUnsignedLongData(i,colLOCATION_KEY),
						data->getDateData(i,colTIMESTAMP),
						static_cast<IRACallStackHistory::OriginType>(data->getStringData(i,colORIGIN)[0]),
						static_cast<IRACallStackHistory::CallType>(data->getStringData(i,colCALL_TYPE)[0]),
						data->getStringData(i,colIDENTIFIER),
						data->getStringData(i,colLOCATION),
                        false,                                     // false indicates not a new session
						data->getBooleanData(i,colISEMERGENCY));   // td17294
                    
                    records.push_back( record );
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

		return records;

    }

    
	std::string RACallStackHistoryAccessFactory::getLocalDatabaseName()
    {
		std::string localDatabaseName;    
		if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw DatabaseException("db-connection-file not set");
		}
		try
		{
			std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			localDatabaseName = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Radio_Cd, Write);

			/*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			DataConnections connections = connectionStrings.getConnectionList(Radio_Cd, Write);
			localDatabaseName = connections[0].first;  */
		}
		catch(...) // most likely a file not found error
		{
			throw DatabaseException("Unable to find database connection infomation");
		}
		return localDatabaseName;
	}
	
    void RACallStackHistoryAccessFactory::addCallStackEntry( 
		//const unsigned long& key,
		const std::string& profile,
		const unsigned long& locationKey,
		const time_t& timestamp,
		IRACallStackHistory::OriginType originType,
		IRACallStackHistory::CallType callType,
		const std::string& identifier,
		const std::string& location,
		bool isEmergency // td17294
		)
    {
		/// @note icky date conversion...
		struct tm dateStruct;
        char dateBuf[20] = {0};
        if ( ACE_OS::localtime_r( &timestamp, &dateStruct ) != NULL )
        {
			// YYYY/MM/DD HH:MM:SS
			sprintf( dateBuf, "%04d/%02d/%02d %02d:%02d:%02d", 
                dateStruct.tm_year + 1900, 
                dateStruct.tm_mon + 1, 
                dateStruct.tm_mday, 
                dateStruct.tm_hour, 
                dateStruct.tm_min, 
                dateStruct.tm_sec);
        }

        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Writing RACallStackEntry (Profile, LocationKey) = (%s, %ld) to the database.",
            profile.c_str(), locationKey);

		{ 
			//std::stringstream s;		//TD18473, zhanghongzhi
			//s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			//    << "INSERT INTO RA_CALL_STACK_HISTORY ( "
			//	<< " PROFILE_NAME, LOCATION_KEY, "
			//	<< " TIMESTAMP, "
			//	<< " ORIGIN, CALL_TYPE, IDENTIFIER, LOCATION ,ISEMERGENCY) VALUES ( "
			//	<< " ''" << databaseConnection->escapeInsertString(profile) << "'' "
			//	<< " , " << locationKey 
			//	<< " , to_date(''" << dateBuf << "'', ''YYYY/MM/DD HH24:MI:SS'') " 
			//	<< " , ''" << static_cast<char>(originType) << "'' "
			//	<< " , ''" << static_cast<char>(callType) << "'' "
			//	<< " , ''" << databaseConnection->escapeInsertString(identifier) << "'' "
			//	<< " , ''" << databaseConnection->escapeInsertString(location) << "'' " 
			//	<< " , " << (isEmergency ? 1:0) // td17294
			//	<< " )'); END;" 
			//	<< std::ends;
			//std::string sql(s.str());
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_CALL_STACK_HISTORY_Oracle_INSERT_34351, getLocalDatabaseName(),
//			std::string strSql  = databaseConnection->prepareSQLStatement(RA_CALL_STACK_HISTORY_INSERT_34351, getLocalDatabaseName(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_CALL_STACK_HISTORY_INSERT_34351, getLocalDatabaseName(),
				databaseConnection->escapeInsertString(profile), locationKey, dateBuf, static_cast<char>(originType), static_cast<char>(callType),
				databaseConnection->escapeInsertString(identifier), databaseConnection->escapeInsertString(location), (isEmergency ? 1:0));
			
			// This will throw an exception upon failure - ready to catch??
			databaseConnection->executeModification(strSql);
		}
		
		// now delete if there are more than rowLimit records for this profile/location

		{
			//std::stringstream s;		//TD18473, zhanghongzhi
			//s   << "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			//    << "delete from ra_call_stack_history where racshi_id in ( "
			//	<< "   select * from ( "
			//	<< "      select racshi_id from ra_call_stack_history "
			//	<< "      where profile_name = ''" << databaseConnection->escapeQueryString(profile) << "'' "
			//	<< "            and location_key = " << locationKey 
			//	<< "      order by racshi_id asc ) "
			//	<< "   where rownum <= ( select count(*) from ra_call_stack_history "
			//	<< "                     where profile_name = ''" << databaseConnection->escapeQueryString(profile) << "'' "
			//	<< "                           and location_key = " << locationKey 
			//	<< "                   ) - " << RACallStackHistory::rowLimit
			//	<< "   ) "
			//	<< "'); END;"
			//	<< std::ends;
			//std::string sql(s.str());
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_CALL_STACK_HISTORY_Oracle_DELETE_34451,getLocalDatabaseName(),
//			std::string strSql  = databaseConnection->prepareSQLStatement(RA_CALL_STACK_HISTORY_DELETE_34451,getLocalDatabaseName(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_CALL_STACK_HISTORY_DELETE_34451,getLocalDatabaseName(),
				databaseConnection->escapeQueryString(profile), locationKey, databaseConnection->escapeQueryString(profile), locationKey, RACallStackHistory::rowLimit);
			
			// This will throw an exception upon failure - ready to catch??
			databaseConnection->executeModification(strSql);

		}
    }



} //end namespace TA_Base_Core


