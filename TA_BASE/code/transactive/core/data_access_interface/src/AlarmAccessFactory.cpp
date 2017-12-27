/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmAccessFactory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * AlarmAccessFactory is a singleton that is used to retrieve AlarmData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/AlarmAccessFactory.h"
#include "core/data_access_interface/src/AlarmData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    AlarmAccessFactory* AlarmAccessFactory::m_instance = 0;
   
    AlarmAccessFactory& AlarmAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new AlarmAccessFactory();
        }
        return *m_instance;
    }

	void AlarmAccessFactory::freeInstance()
	{
		if (0 != m_instance)
		{
			delete m_instance;
			m_instance = 0;
		}
	}

    
    IAlarmData* AlarmAccessFactory::getAlarm(const std::string& key)
    {
        FUNCTION_ENTRY("getAlarm");

		// TD17632
//		std::ostringstream sql;
//        sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
//            << "a.DESCRIPTION,a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE,a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
//            << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
//            << "e.LOCATIONKEY, e.NAME, e.TYPEKEY, (select o.NAME from TA_SESSION s, OPERATOR o where o.pkey (+) = s.operatorkey AND s.PKEY (+) = a.ACKNOWLEDGED_BY) as operator_name "
//            << "from ALARM a, ENTITY e "
//            << "where a.ENTITYKEY = e.PKEY and a.ALARM_ID = '" << key << "'";
//
//		std::vector<IAlarmData*> alarms;
//        runQueryForAlarms(sql.str(), alarms);
//
//        if ( 0 == alarms.size())
//        {
//            std::ostringstream message;
//			message << "No data found for alarm with key " << key;
//            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
//        }
//
//		TA_ASSERT(1 == alarms.size(), "Alarm key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;		
// 		return alarms[0];

		// Return the pointer. And set the m_isNew = false for updating
		return new AlarmData(key);
		// TD17632
    }

    
    std::vector<IAlarmData*> AlarmAccessFactory::getActiveAlarms()
    {
        FUNCTION_ENTRY("getActiveAlarms");

		/*std::ostringstream sql;
        sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
            << "a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE,a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
            << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
            << "e.LOCATIONKEY, e.TYPEKEY,  a.OPERATOR_NAME "
            << "from ALARM a, ENTITY e "
            << "where (a.STATE = "
            <<  AlarmData::OPEN
            << " or a.ACKNOWLEDGED_BY IS NULL) "
            << "AND e.PKEY = a.ENTITYKEY ";*/

		// get a connection to the database and retrieve the DB type
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_Oracle_SELECT_3551, AlarmData::OPEN);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3551, AlarmData::OPEN);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3551, AlarmData::OPEN);
//		TA_ASSERT(sql.size() > 0, "getActiveAlarms: Prepare SQL Statement error");

		std::vector<IAlarmData*> alarms;
        runQueryForAlarms(databaseConnection, sql, alarms);

		FUNCTION_EXIT;
        return alarms;
    }

	std::vector<IAlarmData*> AlarmAccessFactory::getActiveAlarms(unsigned long maxCount)
    {
        FUNCTION_ENTRY("getActiveAlarms");
		 
// 		std::ostringstream sql;
//         sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
//             << "a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE,a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
//             << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
//             << "e.LOCATIONKEY, e.TYPEKEY,  a.OPERATOR_NAME "
//             << "from ALARM a, ENTITY e "
//             << "where (a.STATE = "
//             <<  AlarmData::OPEN
//             << " or a.ACKNOWLEDGED_BY IS NULL) "
//             << "AND e.PKEY = a.ENTITYKEY  and rownum <=" << maxCount;
		 
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
		 
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3554, AlarmData::OPEN, maxCount);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3554, AlarmData::OPEN, maxCount);
//		TA_ASSERT(sql.size() > 0, "getActiveAlarms: Prepare SQL Statement error");

		std::vector<IAlarmData*> alarms;
		runQueryForAlarms(databaseConnection, sql, alarms);
		
		FUNCTION_EXIT;
        return alarms;
    }

    std::vector<IAlarmData*> AlarmAccessFactory::getActiveAlarmsForLocation(unsigned long location)
    {
		FUNCTION_ENTRY("getActiveAlarmsForLocation");

		/*std::ostringstream sql;
        sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
            << "a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE, a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
            << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
            << "e.LOCATIONKEY, e.TYPEKEY, a.OPERATOR_NAME "
            << "from ALARM a, ENTITY e "
            << "where ( (a.STATE = " << AlarmData::OPEN
            << " or a.ACKNOWLEDGED_BY IS NULL) "
            << "and e.LOCATIONKEY = " << location
            << ") AND (e.PKEY = a.ENTITYKEY)";*/
		 
		
		// get a connection to the database and retrieve the DB type
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
		//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_Oracle_SELECT_3552,
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3552,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3552,
			AlarmData::OPEN, location);
//		TA_ASSERT(sql.size() > 0, "getActiveAlarmsForLocation: Prepare SQL Statement error");

		std::vector<IAlarmData*> alarms;
		runQueryForAlarms(databaseConnection, sql, alarms);

		FUNCTION_EXIT;
		return alarms;
    }

	std::vector<IAlarmData*> AlarmAccessFactory::getActiveAlarmsForLocation(unsigned long location,unsigned long maxCount)
    {
		FUNCTION_ENTRY("getActiveAlarmsForLocation");
		
// 		std::ostringstream sql;
//         sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
//             << "a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE, a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
//             << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
//             << "e.LOCATIONKEY, e.TYPEKEY, a.OPERATOR_NAME "
//             << "from ALARM a, ENTITY e "
//             << "where ( (a.STATE = " << AlarmData::OPEN
//             << " or a.ACKNOWLEDGED_BY IS NULL) "
//             << "and e.LOCATIONKEY = " << location
//             << ") AND (e.PKEY = a.ENTITYKEY)  and rownum <=" << maxCount;
		
		// get a connection to the database and retrieve the DB type
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
		//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_Oracle_SELECT_3552,
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3555,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3555,
			AlarmData::OPEN, location,maxCount);
//		TA_ASSERT(sql.size() > 0, "getActiveAlarmsForLocation: Prepare SQL Statement error");

		std::vector<IAlarmData*> alarms;
        runQueryForAlarms(databaseConnection, sql, alarms);
		
		FUNCTION_EXIT;
        return alarms;
    }

	/**
	 * getDBActiveAlarmCount
	 *
	 * Returns the quantity of active alarms in the DB
	 */
	unsigned int AlarmAccessFactory::getDBActiveAlarmCount()
	{
		unsigned int count(0);

        /*char state[50];

        sprintf(state, "%d", AlarmData::OPEN);

        std::string sql = "select count(ALARM_ID) ";
                    sql+= "from ALARM ";
                    sql+= "where (STATE = ";
                    sql+= state;
                    sql+= " or ACKNOWLEDGED_BY IS NULL) ";*/

		// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_STD_SELECT_3501, AlarmData::OPEN);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3501, AlarmData::OPEN);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3501, AlarmData::OPEN);
//		TA_ASSERT(sql.size() > 0, "getDBActiveAlarmCount: Prepare SQL Statement error");
		
		std::string countCol = "COUNT(a.ALARM_ID)";
        std::vector<std::string> columnNames;
        columnNames.push_back(countCol);

        IData* data = databaseConnection->executeQuery( sql, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;
        
		try
		{
			if ( 1 == data->getNumRows() )
			{
				count = data->getIntegerData(0, countCol);
			}
			TA_ASSERT(1 == data->getNumRows(), "There is more than 1 row returned by the count SQL statement");
		}
        catch (TA_Base_Core::DataException&)
        {
            // Clean up the data pointer
            delete data;
            data = NULL;
            throw;
        }
		// clean up the pointer
		delete data;
		data = NULL;
 
		return count;
	}

	/**
	 * getDBActiveAlarmCountForLocation
	 *
	 * Returns the quantity of active alarms in the DB
	 *
	 */
	unsigned int AlarmAccessFactory::getDBActiveAlarmCountForLocation(unsigned long locationKey)
	{
		unsigned int count(0);

        /*char loc[255];
        char state[50];

        sprintf(state, "%d", AlarmData::OPEN);
        sprintf(loc, "%lu", locationKey);

        std::string sql = "select count(ALARM_ID) ";
                    sql+= "from ALARM ";
                    sql+= "where (STATE = ";
                    sql+= state;
                    sql+= " or ACKNOWLEDGED_BY IS NULL) ";
                    sql+= "and entitykey in (select pkey from entity where locationkey = ";
                    sql+= loc;
                    sql+= ")";*/
		// get a connection to the database
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_STD_SELECT_3502,
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3502,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3502,
			AlarmData::OPEN, locationKey);
//		TA_ASSERT(sql.size() > 0, "getDBActiveAlarmCountForLocation: Prepare SQL Statement error");
		
		std::string countCol = "COUNT(a.ALARM_ID)";
        std::vector<std::string> columnNames;
        columnNames.push_back(countCol);

        IData* data = databaseConnection->executeQuery( sql, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;
        
		try
		{
			if ( 1 == data->getNumRows() )
			{
				count = data->getIntegerData(0, countCol);
			}
			TA_ASSERT(1 == data->getNumRows(), "There is more than 1 row returned by the count SQL statement");
		}
        catch (TA_Base_Core::DataException&)
        {
            // Clean up the data pointer
            delete data;
            data = NULL;
            throw;
        }
		// clean up the pointer
		delete data;
		data = NULL;
 
		return count;
	}

    void AlarmAccessFactory::runQueryForAlarms(IDatabase* pDatabase, const SQLStatement& query, std::vector<IAlarmData*>& alarms)
    {
		// get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
		TA_ASSERT(0 != pDatabase, "the Database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
		std::string idColumn = "a.ALARM_ID";
        std::string sourceTimeColumn = "a.SOURCETIME";
        std::string transactiveTimeColumn = "a.TRANSACTIVETIME";
        std::string paramColumn = "a.PARAMLIST";
        std::string severityColumn = "a.SEVERITY";
        std::string typeColumn = "a.TYPE";
        std::string contextColumn = "a.CONTEXT";
        std::string stateColumn = "a.STATE";
		std::string mmsStateTypeColumn = "a.MMS_STATE_TYPE";
        std::string acknowledgedColumn = "a.ACKNOWLEDGED_BY";
        std::string entityKeyColumn = "a.ENTITYKEY";
        std::string commentsColumn = "a.COMMENTS";
        std::string assetNameColumn = "a.ASSET_NAME";
        std::string planStatusColumn = "a.PLANSTATUS";
		std::string avlHeadIdColumn = "a.AVL_HEAD_ID";
		std::string avlStatusColumn = "a.AVL_STATUS";
		std::string isUniqueColumn = "a.IS_UNIQUE";
		std::string ackTimeColumn = "a.ACK_TIME";
        std::string closeTimeColumn = "a.CLOSE_TIME";
        std::string subsystemKeyOfAssociatedEntityColumn = "e.SUBSYSTEMKEY";
        std::string physicalSubsystemKeyOfAssociatedEntityColumn = "e.PHYSICAL_SUBSYSTEM_KEY";
        std::string locationKeyOfAssociatedEntityColumn = "e.LOCATIONKEY";
        std::string typeOfAssociatedEntityColumn = "e.TYPEKEY";
        std::string operatorNameForAcknowledgedBySessionKeyColumn = "a.OPERATOR_NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(idColumn);
        columnNames.push_back(sourceTimeColumn);
        columnNames.push_back(transactiveTimeColumn);
        columnNames.push_back(paramColumn);
        columnNames.push_back(severityColumn);
        columnNames.push_back(typeColumn);
        columnNames.push_back(contextColumn);
        columnNames.push_back(stateColumn);
        columnNames.push_back(mmsStateTypeColumn);
        columnNames.push_back(acknowledgedColumn);
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(commentsColumn);
        columnNames.push_back(assetNameColumn);
        columnNames.push_back(planStatusColumn);
		columnNames.push_back(avlHeadIdColumn);
        columnNames.push_back(avlStatusColumn);
        columnNames.push_back(isUniqueColumn);
        columnNames.push_back(ackTimeColumn);
        columnNames.push_back(closeTimeColumn);
        columnNames.push_back(subsystemKeyOfAssociatedEntityColumn);
        columnNames.push_back(physicalSubsystemKeyOfAssociatedEntityColumn);
        columnNames.push_back(locationKeyOfAssociatedEntityColumn);
        columnNames.push_back(typeOfAssociatedEntityColumn);
        columnNames.push_back(operatorNameForAcknowledgedBySessionKeyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery( query, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;
        
        if ( NULL == data || 0 == data->getNumRows() ) // No entry found with the specified alarm_id
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return;
        }

        std::string subsystemKeyString;
        unsigned long subsystemKey;
		
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows();i++)
            {
                try
                {
                    subsystemKeyString = data->getStringData(i, physicalSubsystemKeyOfAssociatedEntityColumn);
                    if (subsystemKeyString.empty())
                    {
                        // No physical subsystem key. Use "logical" subsystem key.
                        subsystemKey = data->getUnsignedLongData(i, subsystemKeyOfAssociatedEntityColumn);
                    }
                    else
                    {
                        // Physical subsystem key exists. Use it.
                        subsystemKey = data->getUnsignedLongData(i, physicalSubsystemKeyOfAssociatedEntityColumn);
                    }

					alarms.push_back(new AlarmData(
						data->getStringData(i, idColumn),
						//data->getStringData(i, descriptionColumn),
						data->getStringData(i, paramColumn),
						data->getTimestampData(i, sourceTimeColumn),
						data->getTimestampData(i, transactiveTimeColumn),
						data->getDateData(i, ackTimeColumn),
                        data->getDateData(i, closeTimeColumn),
						data->getUnsignedLongData(i, severityColumn),
						data->getUnsignedLongData(i, typeColumn),
                        data->getStringData(i, contextColumn),
                        static_cast<TA_Base_Core::IAlarmData::EAlarmState>(data->getIntegerData(i, stateColumn)),
						static_cast<TA_Base_Core::IAlarmData::EMmsState>(data->getIntegerData(i, mmsStateTypeColumn)),
						data->getStringData( i, acknowledgedColumn ),
						data->getUnsignedLongData(i, entityKeyColumn),
						data->getStringData(i, commentsColumn),
                        data->getStringData(i, assetNameColumn),
                        data->getStringData(i, planStatusColumn),
                        subsystemKey,
                        data->getUnsignedLongData(i, locationKeyOfAssociatedEntityColumn),
                        //data->getStringData(i, nameOfAssociatedEntityColumn),
                        data->getUnsignedLongData(i, typeOfAssociatedEntityColumn),
                        data->getStringData(i, operatorNameForAcknowledgedBySessionKeyColumn),
						data->getBooleanData(i, isUniqueColumn),
						data->getStringData(i, avlHeadIdColumn),
						static_cast<TA_Base_Core::IAlarmData::EAlarmAVLStatus>(data->getIntegerData(i, avlStatusColumn)) ) );
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
        while (pDatabase->moreData(data));
    }

    
    IAlarmData* AlarmAccessFactory::createAlarm()
    {
        return new AlarmData();
    }

} // closes TA_Base_Core
