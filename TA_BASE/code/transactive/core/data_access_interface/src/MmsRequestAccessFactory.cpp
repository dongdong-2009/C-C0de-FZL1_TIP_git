/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestAccessFactory.cpp $
* @author:  Luo HuiRong
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/12/29 14:17:48 $
* Last modified by:  $Author: grace.koh $
* 
* MmsRequestAccessFactory is a singleton that is used to retrieve MmsRequestData objects from the database.
* It provides data retrieving, data saving functions. 
*  
*/


#pragma warning(disable:4786)

#include <iomanip>
#include "ace/OS.h"

#include "core/data_access_interface/src/MmsRequestAccessFactory.h"
 
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
 

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DataConversion.h"

#include <time.h>
#include <sstream>
#include <string>
using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{

    MmsRequestAccessFactory* MmsRequestAccessFactory::m_instance = 0;

    MmsRequestAccessFactory& MmsRequestAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new MmsRequestAccessFactory();
        }
        return *m_instance;
    }

	std::string MmsRequestAccessFactory::getLocalDatabaseName()
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
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Alarm_Sd, Write);

				/*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				DataConnections connections = connectionStrings.getConnectionList(Alarm_Sd, Write);
				m_localDatabase = connections[0].first; */ 
			}
			catch(...) // most likely a file not found error
			{
				throw DatabaseException("Unable to find database connection information");
			}

		}
		return m_localDatabase;
	}

// 	std::string  MmsRequestAccessFactory::getInsertSQL(IDatabase* databaseConnection, MmsRequestAlarmData& MmsRequest)
// 	{
// /*		char pKeyString[50];*/
// 		unsigned  long  severityKey=MmsRequest.getAlarmSeverity();
// 		std::string severityKeyStr = DataConversion::toString(severityKey);
// 
// 		std::string isAutoTriggerString;
// 		MmsRequest.getIsAutoTrigger()? isAutoTriggerString="1": isAutoTriggerString="0";
// 
// 		// Convert time data into formatted strings
// 		struct tm sourceTimeStruct;
// 
// 		// extract the time from the struct and make it into a string
// 		char  sourceTimeString[24];
// 		timeb sourceTime=MmsRequest.getAlarmDate();
// 
// 		if (ACE_OS::localtime_r(&sourceTime.time, &sourceTimeStruct) != NULL)
// 		{
// 			sprintf(sourceTimeString, "%02d/%02d/%04d %02d:%02d:%02d", sourceTimeStruct.tm_mday, sourceTimeStruct.tm_mon + 1,
// 				sourceTimeStruct.tm_year + 1900, sourceTimeStruct.tm_hour, sourceTimeStruct.tm_min, sourceTimeStruct.tm_sec,
// 				sourceTime.millitm);
// 		}
// 
// 		// do it again with the transactiveTime
// 		struct tm targetTimeStruct;
// 		timeb targetTime=MmsRequest.getTargetDate();
// 		char targetTimeString[24];
// 		if (ACE_OS::localtime_r(&targetTime.time, &targetTimeStruct) != NULL)
// 		{
// 
// 			sprintf(targetTimeString, "%02d/%02d/%04d %02d:%02d:%02d", targetTimeStruct.tm_mday, targetTimeStruct.tm_mon + 1,
// 				targetTimeStruct.tm_year + 1900, targetTimeStruct.tm_hour, targetTimeStruct.tm_min, targetTimeStruct.tm_sec, 
// 				targetTime.millitm);
// 		}
// 		std::string formatSQL="";
// 		formatSQL += "insert into mms_request_alarm (alarmID,alarmDate,TargetDate,AlarmSeverity,";
// 		formatSQL += "EquipmentID,AlarmFaultCode,AlarmDescription,ProblemReport,Requestor,IsAutoTrigger) ";
// 		formatSQL += "values (''" + MmsRequest.getAlarmID() + "'',TO_TIMESTAMP(''";
// 		formatSQL += std::string(sourceTimeString) + "'',''dd/mm/yyyy hh24:mi:ss''),";
// 		formatSQL += "TO_TIMESTAMP(''" + std::string(targetTimeString) + "'',''dd/mm/yyyy hh24:mi:ss''),";	 
// 		formatSQL += severityKeyStr + ", ''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getAlarmEquipmentID()) + "'',''";		 
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getAlarmFaultCode() ) +  "'',''";	 
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getAlarmDescription()) + "'',''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getProblemReport()) + "'', ''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getRequestor()) + "'',''";	
// 		formatSQL += isAutoTriggerString + "''";
// 		formatSQL += " )";
// 		return formatSQL;
// 
// 	}

// 	std::string  MmsRequestAccessFactory::getInsertSQL(IDatabase* databaseConnection, MmsRequestSchematicData& MmsRequest)
// 	{
// 		unsigned long entityKey=MmsRequest.getEntityKey();
// 
// 		std::string entityKeyStr = DataConversion::toString(entityKey);
// 	 
// 
// 		// extract the time from the struct and make it into a string
// 		struct tm targetTimeStruct;
// 		timeb targetTime=MmsRequest.getTargetDate();
// 		char targetTimeString[24];
// 		if (ACE_OS::localtime_r(&targetTime.time, &targetTimeStruct) != NULL)
// 		{
// 
// 			sprintf(targetTimeString, "%02d/%02d/%04d %02d:%02d:%02d", targetTimeStruct.tm_mday, targetTimeStruct.tm_mon + 1,
// 				targetTimeStruct.tm_year + 1900, targetTimeStruct.tm_hour, targetTimeStruct.tm_min, targetTimeStruct.tm_sec, 
// 				targetTime.millitm);
// 		}
// 
// 		std::string formatSQL="";
// 		formatSQL += "insert into mms_request_schematic (ENTITYKEY,REQUESTOR,EQUIPMENTID,PROBLEMREPORT,";
// 		formatSQL += "TARGETDATE) ";
// 		formatSQL += "values (" + entityKeyStr + ", ''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getRequestor()) + "'', ''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getEquipmentID()) + "'', ''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getProblemReport()) + "'',TO_TIMESTAMP(''";
// 		formatSQL += std::string(targetTimeString)+ "'',''dd/mm/yyyy hh24:mi:ss'') )";
// 		return formatSQL;
// 
// 	}


// 	std::string  MmsRequestAccessFactory::getInsertSQL(IDatabase* databaseConnection, MmsRequestScheduledData& MmsRequest)
// 	{
// 		unsigned long entityKey=MmsRequest.getEntityKey();
// 
// 		std::string entityKeyStr = DataConversion::toString(entityKey);
// 
// 		unsigned long scheduledType = static_cast<unsigned long>(MmsRequest.getScheduledType());
// 
// 		std::string scheduledTypeStr = DataConversion::toString(scheduledType);
// 
// 		std::ostringstream dpValueSStream;
// 		dpValueSStream<<MmsRequest.getDpValue();
// 		std::string dpValueStr = dpValueSStream.str();
// 
// 	 
// 
// 		// extract the time from the struct and make it into a string
// 		struct tm planDateStruct;
// 		timeb planDate=MmsRequest.getPlanDate();
// 		char planDateString[24];
// 		if (ACE_OS::localtime_r(&planDate.time, &planDateStruct) != NULL)
// 		{
// 
// 			sprintf(planDateString, "%02d/%02d/%04d %02d:%02d:%02d", planDateStruct.tm_mday, planDateStruct.tm_mon + 1,
// 				planDateStruct.tm_year + 1900, planDateStruct.tm_hour, planDateStruct.tm_min, planDateStruct.tm_sec, 
// 				planDate.millitm);
// 		}
// 	
// 		// extract the time from the struct and make it into a string
// 		struct tm createDateStruct;
// 		timeb createDate=MmsRequest.getCreateDate();
// 		char createDateString[24];
// 		if (ACE_OS::localtime_r(&createDate.time, &createDateStruct) != NULL)
// 		{
// 
// 			sprintf(createDateString, "%02d/%02d/%04d %02d:%02d:%02d", createDateStruct.tm_mday, createDateStruct.tm_mon + 1,
// 				createDateStruct.tm_year + 1900, createDateStruct.tm_hour, createDateStruct.tm_min, createDateStruct.tm_sec, 
// 				createDate.millitm);
// 		}
// 
// 		std::string formatSQL="";
// 		formatSQL += "insert into mms_request_scheduled (ENTITYKEY,SCHEDULEDTYPE,EQUIPMENTID,METERCODE,EQUIPMENTDESCRIPTION,";
// 		formatSQL += "DPVALUE,PLANDATE,CREATEDATE) ";
// 		formatSQL += "values (" + entityKeyStr + ",";
// 		formatSQL += std::string(scheduledTypeStr) + ", ''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getEquipmentID()) + "'', ''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getMeterCode()) + "'', ''";
// 		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getEquipmentDescription()) + "'', ";
// 		formatSQL += dpValueStr + ",TO_TIMESTAMP(''";
// 		formatSQL += std::string(planDateString) + "'',''dd/mm/yyyy hh24:mi:ss''),TO_TIMESTAMP(''";
// 		formatSQL += std::string(createDateString)+ "'',''dd/mm/yyyy hh24:mi:ss'') )";
// 		return formatSQL;
// 
// 	}

	std::string  MmsRequestAccessFactory::getDeleteSQL(IDatabase* databaseConnection, std::vector<MmsRequestAlarmData*>& MmsRequests)
	{
		std::string formatSQL="";
		//formatSQL += "delete mms_request_alarm where pkey in (" ; 
		int pkeyCount = 0;

		std::string pkeyString;

		for (int i=0; i< MmsRequests.size(); i++)
		{
			if(MmsRequests[i] != NULL)
			{
				unsigned long pkey=MmsRequests[i]->getKey();
				pkeyString = DataConversion::toString(pkey);
				if(pkeyCount>0)
				{
					formatSQL += ",";
				}
				formatSQL += std::string(pkeyString);
				pkeyCount++;
			}
			
		}
		//formatSQL += ")";
		if(pkeyCount>0)
		{
			return formatSQL;
		}
		else 
		{
			return "";
		}
		
	}

	std::string  MmsRequestAccessFactory::getDeleteSQL(IDatabase* databaseConnection, std::vector<MmsRequestSchematicData*>& MmsRequests)
	{
		std::string formatSQL="";
		//formatSQL += "delete mms_request_schematic where pkey in (" ; 
		int pkeyCount = 0;

		std::string pkeyString;

		for (int i=0; i< MmsRequests.size(); i++)
		{
			if(MmsRequests[i] != NULL)
			{
				unsigned long pkey=MmsRequests[i]->getKey();
				pkeyString = DataConversion::toString(pkey);
				if(pkeyCount>0)
				{
					formatSQL += ",";
				}
				formatSQL += std::string(pkeyString);
				pkeyCount++;
			}

		}

		//formatSQL += ")";
		if(pkeyCount>0)
		{
			return formatSQL;
		}
		else 
		{
			return "";
		}
	}

	std::string  MmsRequestAccessFactory::getDeleteSQL(IDatabase* databaseConnection, std::vector<MmsRequestScheduledData*>& MmsRequests)
	{
		std::string formatSQL="";
		//formatSQL += "delete mms_request_scheduled where pkey in (" ; 
		int pkeyCount = 0;

		std::string pkeyString;

		for (int i=0; i< MmsRequests.size(); i++)
		{
			if(MmsRequests[i] != NULL)
			{
				unsigned long pkey=MmsRequests[i]->getKey();
				pkeyString = DataConversion::toString(pkey);
				if(pkeyCount>0)
				{
					formatSQL += ",";
				}
				formatSQL += pkeyString;
				pkeyCount++;
			}

		}

		//formatSQL += ")";
		if(pkeyCount>0)
		{
			return formatSQL;
		}
		else 
		{
			return "";
		}
	}

	bool  MmsRequestAccessFactory::saveMmsRequestAlarm(MmsRequestAlarmData& MmsRequest)
	{
		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::saveMmsRequestAlarm in");
		// Now we can get on with writing the entity to the database.
		// Convert non-string elements to char*'s
	
		// Get the database connection
		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest, Write);
		// generate the SQL string to write the entity (reuse the string)


		unsigned  long  severityKey=MmsRequest.getAlarmSeverity();
		std::string severityKeyStr = DataConversion::toString(severityKey);

		std::string isAutoTriggerString;
		MmsRequest.getIsAutoTrigger()? isAutoTriggerString="1": isAutoTriggerString="0";

		// Convert time data into formatted strings
		struct tm sourceTimeStruct;

		// extract the time from the struct and make it into a string
		char  sourceTimeString[24] = {0};
		timeb sourceTime=MmsRequest.getAlarmDate();

		if (ACE_OS::localtime_r(&sourceTime.time, &sourceTimeStruct) != NULL)
		{
			sprintf(sourceTimeString, "%02d/%02d/%04d %02d:%02d:%02d", sourceTimeStruct.tm_mday, sourceTimeStruct.tm_mon + 1,
				sourceTimeStruct.tm_year + 1900, sourceTimeStruct.tm_hour, sourceTimeStruct.tm_min, sourceTimeStruct.tm_sec,
				sourceTime.millitm);
		}

		// do it again with the transactiveTime
		struct tm targetTimeStruct;
		timeb targetTime=MmsRequest.getTargetDate();
		char targetTimeString[24] = {0};
		if (ACE_OS::localtime_r(&targetTime.time, &targetTimeStruct) != NULL)
		{

			sprintf(targetTimeString, "%02d/%02d/%04d %02d:%02d:%02d", targetTimeStruct.tm_mday, targetTimeStruct.tm_mon + 1,
				targetTimeStruct.tm_year + 1900, targetTimeStruct.tm_hour, targetTimeStruct.tm_min, targetTimeStruct.tm_sec, 
				targetTime.millitm);
		}

// 		std::string insertionSQL=getInsertSQL(databaseConnection,MmsRequest);
// 
// 		std::string formatSQL="";
// 		formatSQL +=  "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
// 		formatSQL += getLocalDatabaseName();
// 		formatSQL += "','GROUP','";
// 		formatSQL += insertionSQL;
// 		formatSQL += "'); END;";	

		SQLStatement strSql;

//		strSql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_ALARM_INSERT_73001, getLocalDatabaseName(), databaseConnection->escapeAQSQLString(MmsRequest.getAlarmID()),

		databaseConnection->prepareSQLStatement(strSql, MMS_REQUEST_ALARM_INSERT_73001, getLocalDatabaseName(), databaseConnection->escapeAQSQLString(MmsRequest.getAlarmID()),
			std::string(sourceTimeString),   std::string(targetTimeString) , severityKeyStr,  
			databaseConnection->escapeAQSQLString( MmsRequest.getAlarmEquipmentID() ), databaseConnection->escapeAQSQLString( MmsRequest.getAlarmFaultCode()  ),
			databaseConnection->escapeAQSQLString(MmsRequest.getAlarmDescription()),databaseConnection->escapeAQSQLString(MmsRequest.getProblemReport()),
			databaseConnection->escapeAQSQLString( MmsRequest.getRequestor() ),  isAutoTriggerString);
//		TA_ASSERT(strSql.size() > 0, "MmsRequestAccessFactory::saveMmsRequestAlarm(): Prepare SQL Statement error");
		databaseConnection->executeModification(strSql);
 

		//remove the simple db directly, otherwise it will cause memory.
		//TODO: The DAI layer should provide a solution to fix the problem.
		TA_Base_Core::DatabaseFactory::getInstance().delDatabase(databaseConnection);  


		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::saveMmsRequestAlarm out");
		return true;
		 
	}

// 	bool MmsRequestAccessFactory::saveMmsRequestAlarms(std::vector<MmsRequestAlarmData*>& MmsRequests )
// 	{
// 		std::vector<MmsRequestAlarmData*>::iterator iter=MmsRequests.begin();
// 
// 		IDatabase* databaseConnection =
// 			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MmsRequest, Write);
// 	 
// 		std::string insertionSQLs="";
// 		for (;iter!=MmsRequests.end();iter++)
// 		{
// 			std::string insertionSQL="";
// 			insertionSQL=getInsertSQL(databaseConnection,*(*iter));
// 			insertionSQLs+=insertionSQL + ";" ;
// 		}		
// 		std::string formatSQL="";
// 		formatSQL +=  "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
// 		formatSQL += getLocalDatabaseName();
// 		formatSQL += "','GROUP','";
// 		formatSQL += "begin " + insertionSQLs + " end; ";
// 		formatSQL += "'); END;";	 
// 
// 		databaseConnection->executeModification(formatSQL);
// 
// 		//remove the simple db directly, otherwise it will cause memory.
// 		//TODO: The DAI layer should provide a solution to fix the problem.
// 		TA_Base_Core::DatabaseFactory::getInstance().delDatabase(databaseConnection);  
// 
// 		return true;
// 	}
// 

	bool MmsRequestAccessFactory::saveMmsRequestSchematic(MmsRequestSchematicData& MmsRequest)
	{
		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::saveMmsRequestSchematic in");

		// Now we can get on with writing the entity to the database.
		// Convert non-string elements to char*'s

		// Get the database connection
		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest, Write);
		// generate the SQL string to write the entity (reuse the string)

// 		std::string insertionSQL=getInsertSQL(databaseConnection,MmsRequest);
// 
// 		std::string formatSQL="";
// 		formatSQL +=  "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
// 		formatSQL += getLocalDatabaseName();
// 		formatSQL += "','GROUP','";
// 		formatSQL += insertionSQL;
// 		formatSQL += "'); END;";	 

		unsigned long entityKey=MmsRequest.getEntityKey();

		std::string entityKeyStr = DataConversion::toString(entityKey);


		// extract the time from the struct and make it into a string
		struct tm targetTimeStruct;
		timeb targetTime=MmsRequest.getTargetDate();
		char targetTimeString[24];
		if (ACE_OS::localtime_r(&targetTime.time, &targetTimeStruct) != NULL)
		{

			sprintf(targetTimeString, "%02d/%02d/%04d %02d:%02d:%02d", targetTimeStruct.tm_mday, targetTimeStruct.tm_mon + 1,
				targetTimeStruct.tm_year + 1900, targetTimeStruct.tm_hour, targetTimeStruct.tm_min, targetTimeStruct.tm_sec, 
				targetTime.millitm);
		}

		std::string formatSQL="";
		formatSQL += "insert into mms_request_schematic (ENTITYKEY,REQUESTOR,EQUIPMENTID,PROBLEMREPORT,";
		formatSQL += "TARGETDATE) ";
		formatSQL += "values (" + entityKeyStr + ", ''";
		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getRequestor()) + "'', ''";
		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getEquipmentID()) + "'', ''";
		formatSQL += databaseConnection->escapeAQSQLString(MmsRequest.getProblemReport()) + "'',TO_TIMESTAMP(''";
		formatSQL += std::string(targetTimeString)+ "'',''dd/mm/yyyy hh24:mi:ss'') )";		 
		
		SQLStatement strSql;

//		strSql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_SCHEMATIC_INSERT_73101, getLocalDatabaseName(),entityKeyStr ,			 

		databaseConnection->prepareSQLStatement(strSql, MMS_REQUEST_SCHEMATIC_INSERT_73101, getLocalDatabaseName(),entityKeyStr ,
			databaseConnection->escapeAQSQLString( MmsRequest.getRequestor() ), databaseConnection->escapeAQSQLString( MmsRequest.getEquipmentID()  ),
			databaseConnection->escapeAQSQLString(MmsRequest.getProblemReport()), std::string(targetTimeString)	 );

//		TA_ASSERT(strSql.size() > 0, "MmsRequestAccessFactory::saveMmsRequestSchematic(): Prepare SQL Statement error");
		databaseConnection->executeModification(strSql);


		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::saveMmsRequestSchematic out");
		return true;
	} 

	bool MmsRequestAccessFactory::saveMmsRequestScheduled(MmsRequestScheduledData& MmsRequest)
	{
		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::MmsRequestScheduledData in");

		// Now we can get on with writing the entity to the database.
		// Convert non-string elements to char*'s

		// Get the database connection
		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest, Write);
		// generate the SQL string to write the entity (reuse the string)

// 		std::string insertionSQL=getInsertSQL(databaseConnection,MmsRequest);
// 
// 		std::string formatSQL="";
// 		formatSQL +=  "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
// 		formatSQL += getLocalDatabaseName();
// 		formatSQL += "','GROUP','";
// 		formatSQL += insertionSQL;
// 		formatSQL += "'); END;";	 

		unsigned long entityKey=MmsRequest.getEntityKey();

		std::string entityKeyStr = DataConversion::toString(entityKey);

		unsigned long scheduledType = static_cast<unsigned long>(MmsRequest.getScheduledType());

		std::string scheduledTypeStr = DataConversion::toString(scheduledType);

		std::ostringstream dpValueSStream;
        // Double precision is 8
		dpValueSStream << std::setiosflags(ios::fixed) << std::setprecision(8) << MmsRequest.getDpValue();
		std::string dpValueStr = dpValueSStream.str();

		// extract the time from the struct and make it into a string
		struct tm planDateStruct;
		timeb planDate=MmsRequest.getPlanDate();
		char planDateString[24] = {0};
		if (ACE_OS::localtime_r(&planDate.time, &planDateStruct) != NULL)
		{

			sprintf(planDateString, "%02d/%02d/%04d %02d:%02d:%02d", planDateStruct.tm_mday, planDateStruct.tm_mon + 1,
				planDateStruct.tm_year + 1900, planDateStruct.tm_hour, planDateStruct.tm_min, planDateStruct.tm_sec, 
				planDate.millitm);
		}

		// extract the time from the struct and make it into a string
		struct tm createDateStruct;
		timeb createDate=MmsRequest.getCreateDate();
		char createDateString[24] = {0};
		if (ACE_OS::localtime_r(&createDate.time, &createDateStruct) != NULL)
		{

			sprintf(createDateString, "%02d/%02d/%04d %02d:%02d:%02d", createDateStruct.tm_mday, createDateStruct.tm_mon + 1,
				createDateStruct.tm_year + 1900, createDateStruct.tm_hour, createDateStruct.tm_min, createDateStruct.tm_sec, 
				createDate.millitm);
		}
	 
		SQLStatement strSql;

//		strSql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_SCHEDULED_INSERT_73201, getLocalDatabaseName(),entityKeyStr ,	std::string(scheduledTypeStr),		 

		databaseConnection->prepareSQLStatement(strSql, MMS_REQUEST_SCHEDULED_INSERT_73201, getLocalDatabaseName(),entityKeyStr ,	std::string(scheduledTypeStr),
			databaseConnection->escapeAQSQLString( MmsRequest.getEquipmentID() ), databaseConnection->escapeAQSQLString( MmsRequest.getMeterCode()  ),
			databaseConnection->escapeAQSQLString(MmsRequest.getEquipmentDescription()),dpValueStr, std::string(planDateString),std::string(createDateString));

//		TA_ASSERT(strSql.size() > 0, "MmsRequestAccessFactory::MmsRequestScheduledData(): Prepare SQL Statement error");
		databaseConnection->executeModification(strSql);
 

		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::MmsRequestScheduledData out");
		return true;
	}

	std::vector<MmsRequestAlarmData*> MmsRequestAccessFactory::getMmsRequestAlarmData(unsigned long maxSize)
	{	
		//order by alarm severity (1 highest first), alarm date time (latest first)
		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::getMmsRequestAlarmData in");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest, Read);

// 		std::ostringstream sql;
// 		sql << "select PKEY, ALARMID,TO_CHAR(AlarmDate,'YYYYMMDDHH24MISS') AS AlarmDate,ALARMSEVERITY,EQUIPMENTID, ALARMFAULTCODE,";
// 		sql << "ALARMDESCRIPTION,PROBLEMREPORT,REQUESTOR,TO_CHAR(TARGETDATE,'YYYYMMDDHH24MISS') AS TARGETDATE, ISAUTOTRIGGER";
// 		sql << " FROM mms_request_alarm where rownum<= " <<maxSize << " order by ALARMDATE desc";

//		std::string sql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_ALARM_SELECT_73002,maxSize);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, MMS_REQUEST_ALARM_SELECT_73002,maxSize);
//		TA_ASSERT(sql.size() > 0, "MmsRequestAccessFactory::getMmsRequestAlarmData(): Prepare SQL Statement error");

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( "PKEY" );
		columnNames.push_back( "ALARMID");
		columnNames.push_back(  "ALARMDATE" );
		columnNames.push_back( "ALARMSEVERITY");
		columnNames.push_back( "EQUIPMENTID"  );
		columnNames.push_back( "ALARMFAULTCODE");
		columnNames.push_back( "ALARMDESCRIPTION" );
		columnNames.push_back( "PROBLEMREPORT");
		columnNames.push_back( "REQUESTOR" );
		columnNames.push_back( "TARGETDATE");
		columnNames.push_back( "ISAUTOTRIGGER");

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading mms_request_alarm data: %s", sql.str().c_str());

		IData* data = databaseConnection->executeQuery( sql, columnNames ) ;

		std::vector<MmsRequestAlarmData*> mmsRequestAlarms;

		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					mmsRequestAlarms.push_back( createMmsRequestAlarmData( i, data ) );

				}
				catch (TA_Base_Core::DataException&)
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


		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::getMmsRequestAlarmData out");
		return mmsRequestAlarms;
		 
	}

	MmsRequestAlarmData* MmsRequestAccessFactory::createMmsRequestAlarmData( unsigned long row, IData* data )
	{
		unsigned long pkey = data->getUnsignedLongData( row, "PKEY" );
		std::string alarmID = data->getStringData(row, "ALARMID");

		time_t alarmDate_timet = data->getDateData( row,  "ALARMDATE" );
		timeb alarmDate_timeb;
		alarmDate_timeb.time = alarmDate_timet;

		unsigned long alarmSeverity = data->getUnsignedLongData( row, "ALARMSEVERITY" );
		std::string equipmentID = data->getStringData(row, "EQUIPMENTID" );
		std::string alarmFaultCode = data->getStringData(row, "ALARMFAULTCODE");
		std::string alarmDescription = data->getStringData(row, "ALARMDESCRIPTION");
		std::string requestor = data->getStringData(row, "REQUESTOR");
		std::string problemReport = data->getStringData(row, "PROBLEMREPORT");

		time_t targetDate_timet = data->getDateData( row, "TARGETDATE" );
		timeb targetDate_timeb;
		targetDate_timeb.time = targetDate_timet;

		bool isAutoTrigger;
		isAutoTrigger = ( (1 == data->getIntegerData(row,"ISAUTOTRIGGER")) ? true : false);

		return new MmsRequestAlarmData(pkey,alarmID,alarmDate_timeb,alarmSeverity,equipmentID,alarmFaultCode,
			alarmDescription,requestor,problemReport,targetDate_timeb,isAutoTrigger);

	}

	MmsRequestSchematicData* MmsRequestAccessFactory::createMmsRequestSchematicData( unsigned long row, IData* data )
	{
		unsigned long key = data->getUnsignedLongData( row, "PKEY" );
		unsigned long entityKey = data->getUnsignedLongData( row, "ENTITYKEY" );
		std::string equipmentID = data->getStringData(row, "EQUIPMENTID" );
		std::string requestor = data->getStringData(row, "REQUESTOR");
		std::string problemReport = data->getStringData(row, "PROBLEMREPORT");

		time_t targetDate_timet = data->getDateData( row, "TARGETDATE" );
		timeb targetDate_timeb;
		targetDate_timeb.time = targetDate_timet;

		return new MmsRequestSchematicData(key,entityKey,equipmentID,requestor, problemReport,targetDate_timeb);
	}

	MmsRequestScheduledData* MmsRequestAccessFactory::createMmsRequestScheduledData( unsigned long row, IData* data )
	{
		unsigned long key = data->getUnsignedLongData( row, "PKEY" );
		MmsRequestScheduledData::ScheduledType scheduledType = static_cast<MmsRequestScheduledData::ScheduledType> (data->getIntegerData(row, "SCHEDULEDTYPE"));
		unsigned long entityKey = data->getUnsignedLongData( row, "ENTITYKEY" );
		std::string equipmentID = data->getStringData(row, "EQUIPMENTID" );
		std::string meterCode = data->getStringData(row, "METERCODE");
		std::string equipmentDescription = data->getStringData(row, "EQUIPMENTDESCRIPTION");
		
		std::string dpValueStr = data->getStringData(row, "DPVALUE");
		double dpValue = atof(dpValueStr.c_str());
		
		time_t planDate_timet = data->getDateData( row, "PLANDATE" );
		timeb planDate_timeb;
		planDate_timeb.time = planDate_timet;

		time_t createDate_timet = data->getDateData( row, "CREATEDATE" );
		timeb createDate_timeb;
		createDate_timeb.time = createDate_timet;

		return new MmsRequestScheduledData(key, scheduledType, entityKey, equipmentID,meterCode,
			equipmentDescription,dpValue,planDate_timeb,createDate_timeb );

	}

	std::vector<MmsRequestSchematicData*> MmsRequestAccessFactory::getMmsRequestSchematicData(unsigned long maxSize)
	{
		//order by TARGETDATE (earliest first)
		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::getMmsRequestSchematicData in");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest, Read);

// 		std::ostringstream sql;
// 		sql << "select PKEY, ENTITYKEY,REQUESTOR,EQUIPMENTID,PROBLEMREPORT, TO_CHAR(TARGETDATE,'YYYYMMDDHH24MISS') AS TARGETDATE";
// 			sql << " FROM mms_request_schematic where rownum<= "<< maxSize << " order by TARGETDATE desc";
//		std::string sql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_SCHEMATIC_SELECT_73102,maxSize);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, MMS_REQUEST_SCHEMATIC_SELECT_73102,maxSize);
//		TA_ASSERT(sql.size() > 0, "MmsRequestAccessFactory::getMmsRequestSchematicData(): Prepare SQL Statement error");


		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( "PKEY" );
		columnNames.push_back( "ENTITYKEY");
		columnNames.push_back( "REQUESTOR" );
		columnNames.push_back( "EQUIPMENTID" );
		columnNames.push_back( "PROBLEMREPORT" );
		columnNames.push_back( "TARGETDATE");

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading mms_request_schematic data: %s", sql.str().c_str());

		IData* data = databaseConnection->executeQuery( sql, columnNames ) ;

		std::vector<MmsRequestSchematicData*> mmsRequestAlarms;

		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					mmsRequestAlarms.push_back( createMmsRequestSchematicData( i, data ) );

				}
				catch (TA_Base_Core::DataException&)
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

		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::getMmsRequestSchematicData out");
		return mmsRequestAlarms;

	}
	std::vector<MmsRequestScheduledData*> MmsRequestAccessFactory::getMmsRequestScheduledData(unsigned long maxSize)
	{
		//order by PlanDate desc (latest first)
		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::getMmsRequestScheduledData in");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest, Read);

// 		std::ostringstream sql;
// 		sql << "select PKEY, SCHEDULEDTYPE,ENTITYKEY,EQUIPMENTID,METERCODE,EQUIPMENTDESCRIPTION, DPVALUE,TO_CHAR(PLANDATE,'YYYYMMDDHH24MISS') AS PLANDATE,TO_CHAR(CREATEDATE,'YYYYMMDDHH24MISS') AS CREATEDATE";
// 			sql << " FROM mms_request_scheduled  where rownum<= " << maxSize << " order by PLANDATE desc ";

//		std::string sql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_SCHEDULED_SELECT_73202,maxSize);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, MMS_REQUEST_SCHEDULED_SELECT_73202,maxSize);
//		TA_ASSERT(sql.size() > 0, "MmsRequestAccessFactory::getMmsRequestScheduledData(): Prepare SQL Statement error");

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( "PKEY" );
		columnNames.push_back("SCHEDULEDTYPE");
		columnNames.push_back( "ENTITYKEY");
		columnNames.push_back( "EQUIPMENTID"  );
		columnNames.push_back( "METERCODE");
		columnNames.push_back( "EQUIPMENTDESCRIPTION" );
		columnNames.push_back( "DPVALUE");
		columnNames.push_back( "PLANDATE" );
		columnNames.push_back( "CREATEDATE");

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading mms_request_scheduled data: %s", sql.str().c_str());

		IData* data = databaseConnection->executeQuery( sql, columnNames ) ;

		std::vector<MmsRequestScheduledData*> mmsRequestAlarms;

		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					mmsRequestAlarms.push_back( createMmsRequestScheduledData( i, data ) );

				}
				catch (TA_Base_Core::DataException&)
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

		LOG0 (SourceInfo, DebugUtil::DebugTrace, "MmsRequestAccessFactory::getMmsRequestScheduledData out");
		return mmsRequestAlarms;
	}


	bool MmsRequestAccessFactory::deleteMmsRequestAlarmData(std::vector<MmsRequestAlarmData*>& vec )
	{
		if(vec.size()==0)
		{
			return true;
		}

		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest, Read); //only need to delete on Central Database
		
		std::string deleteSQL=getDeleteSQL(databaseConnection,vec);
		
		if(""!= deleteSQL)
		{
// 			std::string formatSQL="";
// 			formatSQL +=  "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
// 			formatSQL += getLocalDatabaseName();
// 			formatSQL += "','GROUP','";
// 			formatSQL += deleteSQL;
// 			formatSQL += "'); END;";

		SQLStatement strSql;
//			strSql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_ALARM_DELETE_73003, getLocalDatabaseName(),deleteSQL);	

		databaseConnection->prepareSQLStatement(strSql, MMS_REQUEST_ALARM_DELETE_73003, getLocalDatabaseName(),deleteSQL);
//			TA_ASSERT(strSql.size() > 0, "MmsRequestAccessFactory::deleteMmsRequestAlarmData(): Prepare SQL Statement error");
			databaseConnection->executeModification(strSql);
		}

		return true;
	}

	bool MmsRequestAccessFactory::deleteMmsRequestSchematicData(std::vector<MmsRequestSchematicData*>& vec)
	{
		if(vec.size()==0)
		{
			return true;
		}

		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest, Read); //only need to delete on Central Database

		std::string deleteSQL=getDeleteSQL(databaseConnection,vec);

		if(""!= deleteSQL)
		{
// 			std::string formatSQL="";
// 			formatSQL +=  "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
// 			formatSQL += getLocalDatabaseName();
// 			formatSQL += "','GROUP','";
// 			formatSQL += deleteSQL;
// 			formatSQL += "'); END;";
		SQLStatement strSql;
//			strSql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_SCHEMATIC_DELETE_73103, getLocalDatabaseName(),deleteSQL);	

		databaseConnection->prepareSQLStatement(strSql, MMS_REQUEST_SCHEMATIC_DELETE_73103, getLocalDatabaseName(),deleteSQL);
//			TA_ASSERT(strSql.size() > 0, "MmsRequestAccessFactory::deleteMmsRequestSchematicData(): Prepare SQL Statement error");
			databaseConnection->executeModification(strSql); 
		}

		return true;
	}

	bool MmsRequestAccessFactory::deleteMmsRequestScheduledData(std::vector<MmsRequestScheduledData*>& vec)
	{
		if(vec.size()==0)
		{
			return true;
		}

		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(MMSRequest,Read); //only need to delete on Central Database

		std::string deleteSQL=getDeleteSQL(databaseConnection,vec);

		if(""!= deleteSQL)
		{
// 			std::string formatSQL="";
// 			formatSQL +=  "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
// 			formatSQL += getLocalDatabaseName();
// 			formatSQL += "','GROUP','";
// 			formatSQL += deleteSQL;
// 			formatSQL += "'); END;";

		SQLStatement strSql;
//			strSql  = databaseConnection->prepareSQLStatement(MMS_REQUEST_SCHEDULED_DELETE_73303, getLocalDatabaseName(),deleteSQL);	

		databaseConnection->prepareSQLStatement(strSql, MMS_REQUEST_SCHEDULED_DELETE_73303, getLocalDatabaseName(),deleteSQL);
//			TA_ASSERT(strSql.size() > 0, "MmsRequestAccessFactory::deleteMmsRequestScheduledData(): Prepare SQL Statement error");
			databaseConnection->executeModification(strSql); 
		}

		return true;
	}
 

} //closes TA_Base_Core




 
