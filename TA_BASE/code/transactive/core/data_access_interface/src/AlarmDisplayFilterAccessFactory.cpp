/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/AlarmDisplayFilterAccessFactory.cpp $
 * @author:  Noel R. Tajanlangit
 * @version: $Revision: #10 $
 *
 * Last modification: $DateTime: 2013/05/02 14:05:26 $
 * Last modified by:  $Author: Noel R. Tajanlangit $
 *
 * AlarmDisplayFilterAccessFactory is an interface to the Alarm_Display_Filter table. It provides read access
 * for agents and user applicaitons.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/AlarmDisplayFilterAccessFactory.h"
#include "core/data_access_interface/src/AlarmDisplayFilterData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	AlarmDisplayFilterAccessFactory* AlarmDisplayFilterAccessFactory::m_pInstance = NULL;

	/**
	 * getInstance
	 */
	AlarmDisplayFilterAccessFactory& AlarmDisplayFilterAccessFactory::getInstance()
	{
		if (NULL == m_pInstance)
		{
			m_pInstance = new AlarmDisplayFilterAccessFactory();
		}

		return *m_pInstance;
	}

	/**
	 * freeInstance
	 */
	void AlarmDisplayFilterAccessFactory::freeInstance()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	/**
	 * getFilterData
	 */
	IAlarmDisplayFilterData* AlarmDisplayFilterAccessFactory::getFilterData(const unsigned long& _pkey)
	{
		FUNCTION_ENTRY("getFilterData");
		IAlarmDisplayFilterData* pRetVal = NULL;

		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_DISPLAY_FILTERING_SELECT_84001, _pkey);

		std::string locationKey = "a.LOCATIONKEY";
		std::string alarmType = "a.ALARMTYPE";
		std::string subsystemKey = "a.SUBSYSTEMKEY";
		std::string alarmLocationKey = "a.ALARMLOCATIONKEY";
		std::string displayCondition = "a.DISPLAYCONDITION";
		std::string deleted = "a.DELETED";

		std::vector<std::string> columnNames;
		columnNames.push_back(locationKey);
		columnNames.push_back(alarmType);
		columnNames.push_back(subsystemKey);
		columnNames.push_back(alarmLocationKey);
		columnNames.push_back(displayCondition);
		columnNames.push_back(deleted);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery(sql,columnNames);

		// Need to bring in the DataException
		using TA_Base_Core::DataException;

		if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified alarm_id
		{
			delete data;
			data = NULL;
			char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No filter found for pkey = %d", _pkey);
			throw DataException(reasonMessage,DataException::NO_VALUE,"");
		}
		else if (1 < data->getNumRows()) // More than one entry found for the alarm_id
		{
			delete data;
			data = NULL;
			char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one filter found for pkey = %d", _pkey);
			throw DataException(reasonMessage,DataException::NOT_UNIQUE,"");
		}

		// Assign the data as appropriate to the member variables.
		// These calls can throw DataExceptions, as documented in the comment of this method.
		pRetVal = new AlarmDisplayFilterData( _pkey,
			data->getUnsignedLongData(0,locationKey),
			data->getUnsignedLongData(0,alarmType),
			data->getUnsignedLongData(0,subsystemKey),
			data->getUnsignedLongData(0,alarmLocationKey),
			data->getUnsignedLongData(0,displayCondition),
			data->getUnsignedLongData(0,deleted));

		// Now that we're done with the IData object, it's our responsibility to delete it
		delete data;
		data = NULL;

		FUNCTION_EXIT;
		return pRetVal;
	}

	/**
	 * getLocationsFilterData
	 */
	std::vector<IAlarmDisplayFilterData*> AlarmDisplayFilterAccessFactory::getLocationsFilterData(const unsigned long& _locationKey)
	{
		FUNCTION_ENTRY("getLocationsFilterData");
		std::vector<IAlarmDisplayFilterData*> retVal;

		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_DISPLAY_FILTERING_SELECT_84002, _locationKey);
	
		std::string pkey = "a.PKEY";
		std::string locationKey = "a.LOCATIONKEY";
		std::string alarmType = "a.ALARMTYPE";
		std::string subsystemKey = "a.SUBSYSTEMKEY";
		std::string alarmLocationKey = "a.ALARMLOCATIONKEY";
		std::string displayCondition = "a.DISPLAYCONDITION";
		std::string deleted = "a.DELETED";

		std::vector<std::string> columnNames;
		columnNames.push_back(pkey);
		columnNames.push_back(locationKey);
		columnNames.push_back(alarmType);
		columnNames.push_back(subsystemKey);
		columnNames.push_back(alarmLocationKey);
		columnNames.push_back(displayCondition);
		columnNames.push_back(deleted);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery(sql,columnNames);

		// Need to bring in the DataException
		using TA_Base_Core::DataException;

		if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified alarm_id
		{
			delete data;
			data = NULL;
			char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No filter found for location = %d", _locationKey);
			throw DataException(reasonMessage,DataException::NO_VALUE,"");
		}

		do
		{
			// Assign the data as appropriate to the member variables.
			// These calls can throw DataExceptions, as documented in the comment of this method.
			for (unsigned long idx=0; idx<data->getNumRows(); idx++)
			{
				AlarmDisplayFilterData* tempData = new AlarmDisplayFilterData( 
					data->getUnsignedLongData(idx,pkey),
					data->getUnsignedLongData(idx,locationKey),
					data->getUnsignedLongData(idx,alarmType),
					data->getUnsignedLongData(idx,subsystemKey),
					data->getUnsignedLongData(idx,alarmLocationKey),
					data->getUnsignedLongData(idx,displayCondition),
					data->getUnsignedLongData(idx,deleted));

				retVal.push_back(tempData);
			}
		
			// Now that we're done with the IData object, it's our responsibility to delete it
			delete data;
			data = NULL;
		}
		while ( databaseConnection->moreData(data) );

		FUNCTION_EXIT;
		return retVal;
	}

	/**
	 * getAllFilterData
	 */
	std::vector<IAlarmDisplayFilterData*> AlarmDisplayFilterAccessFactory::getAllFilterData()
	{
		FUNCTION_ENTRY("getAllFilterData");
		std::vector<IAlarmDisplayFilterData*> retVal;

		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_DISPLAY_FILTERING_SELECT_84003);

		std::string pkey = "a.PKEY";
		std::string locationKey = "a.LOCATIONKEY";
		std::string alarmType = "a.ALARMTYPE";
		std::string subsystemKey = "a.SUBSYSTEMKEY";
		std::string alarmLocationKey = "a.ALARMLOCATIONKEY";
		std::string displayCondition = "a.DISPLAYCONDITION";
		std::string deleted = "a.DELETED";

		std::vector<std::string> columnNames;
		columnNames.push_back(pkey);
		columnNames.push_back(locationKey);
		columnNames.push_back(alarmType);
		columnNames.push_back(subsystemKey);
		columnNames.push_back(alarmLocationKey);
		columnNames.push_back(displayCondition);
		columnNames.push_back(deleted);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery(sql,columnNames);

		// Need to bring in the DataException
		using TA_Base_Core::DataException;

		if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified alarm_id
		{
			delete data;
			data = NULL;
			throw DataException("No filters found",DataException::NO_VALUE,"");
		}

		do
		{
			// Assign the data as appropriate to the member variables.
			// These calls can throw DataExceptions, as documented in the comment of this method.
			for (unsigned long idx=0; idx<data->getNumRows(); idx++)
			{
				AlarmDisplayFilterData* tempData = new AlarmDisplayFilterData( 
					data->getUnsignedLongData(idx,pkey),
					data->getUnsignedLongData(idx,locationKey),
					data->getUnsignedLongData(idx,alarmType),
					data->getUnsignedLongData(idx,subsystemKey),
					data->getUnsignedLongData(idx,alarmLocationKey),
					data->getUnsignedLongData(idx,displayCondition),
					data->getUnsignedLongData(idx,deleted));

				retVal.push_back(tempData);
			}

			// Now that we're done with the IData object, it's our responsibility to delete it
			delete data;
			data = NULL;
		}
		while ( databaseConnection->moreData(data) );

		FUNCTION_EXIT;
		return retVal;
	}

	std::string AlarmDisplayFilterAccessFactory::getLocalDatabaseName()
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
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

}

