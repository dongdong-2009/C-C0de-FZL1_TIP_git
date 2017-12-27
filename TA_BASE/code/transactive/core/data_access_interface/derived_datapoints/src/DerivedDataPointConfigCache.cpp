/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/DerivedDataPointConfigCache.cpp $
  * @author Bart Golab
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by: $Author: yong.liu $
  *
  * Singleton class used to access and cache derived datapoint configuration data items.
  * Supported currently are derived states and input/output associations.
  * This class is a replacement for the static utility DerivedDataPointHelperAccessFactory
  * (formerly in DerivedDataPointHelper.h).
  *
  */

#ifdef __WIN32__
    #pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#endif

#include <sstream>

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointHelper.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointConfigCache.h"
#include "core/data_access_interface/src/SQLCode.h"


namespace TA_Base_Core
{
    //--------------------------------------------------------------------------------------
    // DerivedStateData
    //--------------------------------------------------------------------------------------

    void DerivedStateData::populate(ConfigItemMap& dataMap) // std::map<unsigned long, std::vector<DerivedState>>
    {
        FUNCTION_ENTRY("populate()");

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_DATAPOINT_PKEY("DERIVED_DP_PKEY");
            const std::string STATE_PKEY("SCDSTA_ID");
            const std::string STATE_VALUE("STATE_VALUE");
            const std::string STATE_DESCRIPTION("STATE_DESCRIPTION");
            const std::string STATE_RETURN_CONDITION("STATE_RETURN_CONDITION");
            const std::string ALARM_ENABLED("ALARM_ENABLED");
            const std::string ALARM_SEVERITY("ALARM_SEVERITY");
            const std::string ALARM_DELAY("ALARM_DELAY");
            const std::string ALARM_MESSAGE("ALARM_MESSAGE");
            const std::string ALARM_MMS("ALARM_MMS");

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_DATAPOINT_PKEY);
            columnNames.push_back(STATE_PKEY);
            columnNames.push_back(STATE_VALUE);
            columnNames.push_back(STATE_DESCRIPTION);
            columnNames.push_back(STATE_RETURN_CONDITION);
            columnNames.push_back(ALARM_ENABLED);
            columnNames.push_back(ALARM_SEVERITY);
            columnNames.push_back(ALARM_DELAY);
            columnNames.push_back(ALARM_MESSAGE);
            columnNames.push_back(ALARM_MMS);

            // Build an SQL query for extracting the derived states of all derived data points.
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_DATAPOINT_PKEY << ", " 
                             << STATE_PKEY << ", " 
                             << STATE_VALUE << ", " 
                             << STATE_DESCRIPTION + ", "
                             << "NVL(" << STATE_RETURN_CONDITION << ", ' ') as STATE_RETURN_CONDITION, " 
                             << "NVL(" << ALARM_ENABLED << ", 0) as ALARM_ENABLED, "
                             << "NVL(" << ALARM_SEVERITY << ", 0) as ALARM_SEVERITY, "
                             << "NVL(" << ALARM_DELAY << ", 0) as ALARM_DELAY, "
                             << "NVL(" << ALARM_MESSAGE << ", ' ') as ALARM_MESSAGE, "
                             << "NVL(" << ALARM_MMS << ", ' ') as  ALARM_MMS"
                << "FROM SC_DERIVED_STATE";*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_DERIVED_STATE_Oracle_SELECT_25551);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_DERIVED_STATE_SELECT_25551);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_SELECT_25551);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        // For the current record associated with a given derived data point, 
                        // try to locate the corresponding entry in the derived state map.
                        // If not found then add one.
                        unsigned long derivedDataPointKey(data->getUnsignedLongData(i, DERIVED_DATAPOINT_PKEY));
                        ConfigItemMap::iterator dataMapIt(dataMap.find(derivedDataPointKey));

                        if (dataMapIt == dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(dataMap.insert(std::make_pair(derivedDataPointKey, ConfigItemList())));

                            dataMapIt = res.first;
                        }

                        // Add this derived state to the relevant derived state map element as located earlier.
                        DerivedState derivedState;
                        derivedState.stateKey = data->getUnsignedLongData(i, STATE_PKEY);
                        derivedState.stateValue = data->getIntegerData(i, STATE_VALUE);
                        derivedState.stateDescription = trim(data->getStringData(i, STATE_DESCRIPTION));
                        derivedState.stateReturnCondition = trim(data->getStringData(i, STATE_RETURN_CONDITION));
                        derivedState.isAlarmEnabled = data->getBooleanData(i, ALARM_ENABLED);
                        derivedState.alarmSeverity = data->getIntegerData(i, ALARM_SEVERITY);
                        derivedState.alarmDelay = data->getIntegerData(i, ALARM_DELAY);
                        derivedState.alarmMessage = trim(data->getStringData(i, ALARM_MESSAGE));
                        derivedState.alarmMms = trim(data->getStringData(i, ALARM_MMS));

                        dataMapIt->second.push_back(derivedState);

                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for derived state data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all derived state data retrieved from database. "
                        "The returned derived state map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving derived state data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving derived state data from database.");
        }

        FUNCTION_EXIT;
    }

	
	DdpConfigData<DerivedState>::ConfigItemList DerivedStateData::populateData(unsigned long key)
	{
        FUNCTION_ENTRY("populateData");

		ConfigItemList newDdpState;
        try
        {
            // Build the list of required column names.
            const std::string DERIVED_DATAPOINT_PKEY("DERIVED_DP_PKEY");
            const std::string STATE_PKEY("SCDSTA_ID");
            const std::string STATE_VALUE("STATE_VALUE");
            const std::string STATE_DESCRIPTION("STATE_DESCRIPTION");
            const std::string STATE_RETURN_CONDITION("STATE_RETURN_CONDITION");
            const std::string ALARM_ENABLED("ALARM_ENABLED");
            const std::string ALARM_SEVERITY("ALARM_SEVERITY");
            const std::string ALARM_DELAY("ALARM_DELAY");
            const std::string ALARM_MESSAGE("ALARM_MESSAGE");
            const std::string ALARM_MMS("ALARM_MMS");

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_DATAPOINT_PKEY);
            columnNames.push_back(STATE_PKEY);
            columnNames.push_back(STATE_VALUE);
            columnNames.push_back(STATE_DESCRIPTION);
            columnNames.push_back(STATE_RETURN_CONDITION);
            columnNames.push_back(ALARM_ENABLED);
            columnNames.push_back(ALARM_SEVERITY);
            columnNames.push_back(ALARM_DELAY);
            columnNames.push_back(ALARM_MESSAGE);
            columnNames.push_back(ALARM_MMS);

            // Build an SQL query for extracting the derived states of all derived data points.
           /* std::ostringstream sql;
            sql << "SELECT " << DERIVED_DATAPOINT_PKEY << ", " 
                             << STATE_PKEY << ", " 
                             << STATE_VALUE << ", " 
                             << STATE_DESCRIPTION + ", "
                             << "NVL(" << STATE_RETURN_CONDITION << ", ' ') as STATE_RETURN_CONDITION, " 
                             << "NVL(" << ALARM_ENABLED << ", 0) as ALARM_ENABLED, "
                             << "NVL(" << ALARM_SEVERITY << ", 0) as ALARM_SEVERITY, "
                             << "NVL(" << ALARM_DELAY << ", 0) as ALARM_DELAY, "
                             << "NVL(" << ALARM_MESSAGE << ", ' ') as ALARM_MESSAGE, "
                             << "NVL(" << ALARM_MMS << ", ' ') as ALARM_MMS "
                << "FROM SC_DERIVED_STATE WHERE " << DERIVED_DATAPOINT_PKEY << " = " << key  ;*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_DERIVED_STATE_Oracle_SELECT_25552, key);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_DERIVED_STATE_SELECT_25552, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_SELECT_25552, key);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        // Add this derived state to the relevant derived state map element as located earlier.
                        DerivedState derivedState;
                        derivedState.stateKey = data->getUnsignedLongData(i, STATE_PKEY);
                        derivedState.stateValue = data->getIntegerData(i, STATE_VALUE);
                        derivedState.stateDescription = trim(data->getStringData(i, STATE_DESCRIPTION));
                        derivedState.stateReturnCondition = trim(data->getStringData(i, STATE_RETURN_CONDITION));
                        derivedState.isAlarmEnabled = data->getBooleanData(i, ALARM_ENABLED);
                        derivedState.alarmSeverity = data->getIntegerData(i, ALARM_SEVERITY);
                        derivedState.alarmDelay = data->getIntegerData(i, ALARM_DELAY);
                        derivedState.alarmMessage = trim(data->getStringData(i, ALARM_MESSAGE));
                        derivedState.alarmMms = trim(data->getStringData(i, ALARM_MMS));

                        newDdpState.push_back(derivedState);

                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for derived state data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all derived state data retrieved from database. "
                        "The returned derived state map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving derived state data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving derived state data from database.");
        }

		m_dataMap.insert( std::make_pair(key, newDdpState) ) ;		
		//m_ddpLoaded.insert( std::make_pair(key, true) );

        FUNCTION_EXIT;
		return newDdpState;
	}


    std::string DerivedStateData::trim(const std::string& text) const
    {
        if (!text.empty() && text.find_first_not_of(" \t") == text.npos)
        {
            return "";
        }
         
        return text;   
    }

	void DerivedStateData::bulkLoadByAgentKey(unsigned long agentKey)
	{
        FUNCTION_ENTRY("bulkLoadByAgentKey()");

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_DATAPOINT_PKEY("DERIVED_DP_PKEY");
            const std::string STATE_PKEY("SCDSTA_ID");
            const std::string STATE_VALUE("STATE_VALUE");
            const std::string STATE_DESCRIPTION("STATE_DESCRIPTION");
            const std::string STATE_RETURN_CONDITION("STATE_RETURN_CONDITION");
            const std::string ALARM_ENABLED("ALARM_ENABLED");
            const std::string ALARM_SEVERITY("ALARM_SEVERITY");
            const std::string ALARM_DELAY("ALARM_DELAY");
            const std::string ALARM_MESSAGE("ALARM_MESSAGE");
            const std::string ALARM_MMS("ALARM_MMS");

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_DATAPOINT_PKEY);
            columnNames.push_back(STATE_PKEY);
            columnNames.push_back(STATE_VALUE);
            columnNames.push_back(STATE_DESCRIPTION);
            columnNames.push_back(STATE_RETURN_CONDITION);
            columnNames.push_back(ALARM_ENABLED);
            columnNames.push_back(ALARM_SEVERITY);
            columnNames.push_back(ALARM_DELAY);
            columnNames.push_back(ALARM_MESSAGE);
            columnNames.push_back(ALARM_MMS);

            // Build an SQL query
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_DATAPOINT_PKEY << ", " 
                             << STATE_PKEY << ", " 
                             << STATE_VALUE << ", " 
                             << STATE_DESCRIPTION << ", "
                             << "NVL(" << STATE_RETURN_CONDITION << ", ' ') as STATE_RETURN_CONDITION, " 
                             << "NVL(" << ALARM_ENABLED << ", 0) as ALARM_ENABLED, "
                             << "NVL(" << ALARM_SEVERITY << ", 0) as ALARM_SEVERITY, "
                             << "NVL(" << ALARM_DELAY << ", 0) as ALARM_DELAY, "
                             << "NVL(" << ALARM_MESSAGE << ", ' ') as ALARM_MESSAGE, "
                             << "NVL(" << ALARM_MMS << ", ' ') as  ALARM_MMS"
                << " FROM SC_DERIVED_STATE"
				<< " WHERE " << DERIVED_DATAPOINT_PKEY << " IN (SELECT PKEY FROM ENTITY WHERE AGENTKEY = "
				<< agentKey << ")";*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_DERIVED_STATE_Oracle_SELECT_25553, agentKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_DERIVED_STATE_SELECT_25553, agentKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_SELECT_25553, agentKey);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        unsigned long derivedDataPointKey(data->getUnsignedLongData(i, DERIVED_DATAPOINT_PKEY));
                        ConfigItemMap::iterator dataMapIt(m_dataMap.find(derivedDataPointKey));

                        if (dataMapIt == m_dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(m_dataMap.insert(std::make_pair(derivedDataPointKey, ConfigItemList())));
							//m_ddpLoaded.insert( std::make_pair(derivedDataPointKey, true) );
                            dataMapIt = res.first;
                        }

                        // Add this derived state to the relevant derived state map element as located earlier.
                        DerivedState derivedState;
                        derivedState.stateKey = data->getUnsignedLongData(i, STATE_PKEY);
                        derivedState.stateValue = data->getIntegerData(i, STATE_VALUE);
                        derivedState.stateDescription = trim(data->getStringData(i, STATE_DESCRIPTION));
                        derivedState.stateReturnCondition = trim(data->getStringData(i, STATE_RETURN_CONDITION));
                        derivedState.isAlarmEnabled = data->getBooleanData(i, ALARM_ENABLED);
                        derivedState.alarmSeverity = data->getIntegerData(i, ALARM_SEVERITY);
                        derivedState.alarmDelay = data->getIntegerData(i, ALARM_DELAY);
                        derivedState.alarmMessage = trim(data->getStringData(i, ALARM_MESSAGE));
                        derivedState.alarmMms = trim(data->getStringData(i, ALARM_MMS));

                        dataMapIt->second.push_back(derivedState);

                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
            //m_hasBulkLoaded = true;
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for derived state data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all derived state data retrieved from database. The returned derived state map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving derived state data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving derived state data from database.");
        }

        FUNCTION_EXIT;	
	}

    void DerivedStateData::bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey)
    {
        FUNCTION_ENTRY("bulkLoadByEquipmentDataNodeKey");

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_DATAPOINT_PKEY("DERIVED_DP_PKEY");
            const std::string STATE_PKEY("SCDSTA_ID");
            const std::string STATE_VALUE("STATE_VALUE");
            const std::string STATE_DESCRIPTION("STATE_DESCRIPTION");
            const std::string STATE_RETURN_CONDITION("STATE_RETURN_CONDITION");
            const std::string ALARM_ENABLED("ALARM_ENABLED");
            const std::string ALARM_SEVERITY("ALARM_SEVERITY");
            const std::string ALARM_DELAY("ALARM_DELAY");
            const std::string ALARM_MESSAGE("ALARM_MESSAGE");
            const std::string ALARM_MMS("ALARM_MMS");

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_DATAPOINT_PKEY);
            columnNames.push_back(STATE_PKEY);
            columnNames.push_back(STATE_VALUE);
            columnNames.push_back(STATE_DESCRIPTION);
            columnNames.push_back(STATE_RETURN_CONDITION);
            columnNames.push_back(ALARM_ENABLED);
            columnNames.push_back(ALARM_SEVERITY);
            columnNames.push_back(ALARM_DELAY);
            columnNames.push_back(ALARM_MESSAGE);
            columnNames.push_back(ALARM_MMS);

            // Build an SQL query
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_DATAPOINT_PKEY << ", " 
                             << STATE_PKEY << ", " 
                             << STATE_VALUE << ", " 
                             << STATE_DESCRIPTION << ", "
                             << "NVL(" << STATE_RETURN_CONDITION << ", ' ') as STATE_RETURN_CONDITION, " 
                             << "NVL(" << ALARM_ENABLED << ", 0) as ALARM_ENABLED, "
                             << "NVL(" << ALARM_SEVERITY << ", 0) as ALARM_SEVERITY, "
                             << "NVL(" << ALARM_DELAY << ", 0) as ALARM_DELAY, "
                             << "NVL(" << ALARM_MESSAGE << ", ' ') as ALARM_MESSAGE, "
                             << "NVL(" << ALARM_MMS << ", ' ') as  ALARM_MMS"
                << " FROM SC_DERIVED_STATE"
				<< " WHERE " << DERIVED_DATAPOINT_PKEY << " IN (SELECT PKEY FROM ENTITY WHERE PARENTKEY = "
				<< dataNodeKey << ")";*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_DERIVED_STATE_Oracle_SELECT_25554, dataNodeKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_DERIVED_STATE_SELECT_25554, dataNodeKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_SELECT_25554, dataNodeKey);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        unsigned long derivedDataPointKey(data->getUnsignedLongData(i, DERIVED_DATAPOINT_PKEY));
                        ConfigItemMap::iterator dataMapIt(m_dataMap.find(derivedDataPointKey));

                        if (dataMapIt == m_dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(m_dataMap.insert(std::make_pair(derivedDataPointKey, ConfigItemList())));
							//m_ddpLoaded.insert( std::make_pair(derivedDataPointKey, true) );
                            dataMapIt = res.first;
                        }

                        // Add this derived state to the relevant derived state map element as located earlier.
                        DerivedState derivedState;
                        derivedState.stateKey = data->getUnsignedLongData(i, STATE_PKEY);
                        derivedState.stateValue = data->getIntegerData(i, STATE_VALUE);
                        derivedState.stateDescription = trim(data->getStringData(i, STATE_DESCRIPTION));
                        derivedState.stateReturnCondition = trim(data->getStringData(i, STATE_RETURN_CONDITION));
                        derivedState.isAlarmEnabled = data->getBooleanData(i, ALARM_ENABLED);
                        derivedState.alarmSeverity = data->getIntegerData(i, ALARM_SEVERITY);
                        derivedState.alarmDelay = data->getIntegerData(i, ALARM_DELAY);
                        derivedState.alarmMessage = trim(data->getStringData(i, ALARM_MESSAGE));
                        derivedState.alarmMms = trim(data->getStringData(i, ALARM_MMS));

                        dataMapIt->second.push_back(derivedState);

                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
            //m_hasBulkLoaded = true;
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for derived state data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all derived state data retrieved from database. The returned derived state map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving derived state data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving derived state data from database.");
        }

        FUNCTION_EXIT;
    }


    //--------------------------------------------------------------------------------------
    // InputAssociationData
    //--------------------------------------------------------------------------------------

    void InputAssociationData::populate(ConfigItemMap& dataMap) // std::map<unsigned long, std::vector<InputAssociation>>
    {
        FUNCTION_ENTRY("populate()");

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_DATAPOINT_PKEY = "MAIN_ENTITY_PKEY";
            const std::string INPUT_ASSOCIATION_PKEY = "SCIASS_ID";
            const std::string INPUT_DATAPOINT_PKEY = "INPUT_ENTITY_PKEY";
            const std::string INPUT_CALCULATION_ORDER = "INPUT_CALC_ORDER";
            const std::string INPUT_VALUE = "PREDEFINED_INPUT_VALUE";

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_DATAPOINT_PKEY);
            columnNames.push_back(INPUT_ASSOCIATION_PKEY);
            columnNames.push_back(INPUT_DATAPOINT_PKEY);
            columnNames.push_back(INPUT_CALCULATION_ORDER);
            columnNames.push_back(INPUT_VALUE);

            // Build an SQL query for extracting the input associations of all derived data points.
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_DATAPOINT_PKEY << ", "
                             << INPUT_ASSOCIATION_PKEY << ", " 
                             << INPUT_DATAPOINT_PKEY << ", " 
                             << INPUT_CALCULATION_ORDER << ", " 
                             << INPUT_VALUE << " "
                << "FROM SC_INPUT_ASSOCIATION";*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_INPUT_ASSOCIATION_STD_SELECT_26001);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_INPUT_ASSOCIATION_SELECT_26001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_INPUT_ASSOCIATION_SELECT_26001);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        // For the current record associated with a given derived data point, 
                        // try to locate the corresponding entry in the input association map.
                        // If not found then add one.
                        unsigned long derivedDataPointKey(data->getUnsignedLongData(i, DERIVED_DATAPOINT_PKEY));
                        ConfigItemMap::iterator dataMapIt(dataMap.find(derivedDataPointKey));

                        if (dataMapIt == dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(dataMap.insert(std::make_pair(derivedDataPointKey, ConfigItemList())));

                            dataMapIt = res.first;
                        }

                        // Add this input association to the relevant input association map element as located earlier.
                        InputAssociation inputAssociation;
                        inputAssociation.inputAssociationKey = data->getUnsignedLongData(i, INPUT_ASSOCIATION_PKEY);
                        inputAssociation.inputDataPointKey = data->getUnsignedLongData(i, INPUT_DATAPOINT_PKEY);
                        inputAssociation.inputCalculationOrder = data->getIntegerData(i, INPUT_CALCULATION_ORDER);
                        inputAssociation.inputValue = data->getRealData(i, INPUT_VALUE);

                        dataMapIt->second.push_back(inputAssociation);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for input association data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all input association data retrieved from database. "
                        "The returned input association map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving input association data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving input association data from database.");
        }

        FUNCTION_EXIT;
    }

	DdpConfigData<InputAssociation>::ConfigItemList InputAssociationData::populateData(unsigned long key)
	{
        FUNCTION_ENTRY("InputAssociationData::populateData");

		ConfigItemList newInputAssociationData;

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_DATAPOINT_PKEY = "MAIN_ENTITY_PKEY";
            const std::string INPUT_ASSOCIATION_PKEY = "SCIASS_ID";
            const std::string INPUT_DATAPOINT_PKEY = "INPUT_ENTITY_PKEY";
            const std::string INPUT_CALCULATION_ORDER = "INPUT_CALC_ORDER";
            const std::string INPUT_VALUE = "PREDEFINED_INPUT_VALUE";

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_DATAPOINT_PKEY);
            columnNames.push_back(INPUT_ASSOCIATION_PKEY);
            columnNames.push_back(INPUT_DATAPOINT_PKEY);
            columnNames.push_back(INPUT_CALCULATION_ORDER);
            columnNames.push_back(INPUT_VALUE);

            // Build an SQL query for extracting the input associations of all derived data points.
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_DATAPOINT_PKEY << ", "
                             << INPUT_ASSOCIATION_PKEY << ", " 
                             << INPUT_DATAPOINT_PKEY << ", " 
                             << INPUT_CALCULATION_ORDER << ", " 
                             << INPUT_VALUE << " "
                << "FROM SC_INPUT_ASSOCIATION WHERE " << DERIVED_DATAPOINT_PKEY << " = " << key;*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_INPUT_ASSOCIATION_STD_SELECT_26002, key);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_INPUT_ASSOCIATION_SELECT_26002, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_INPUT_ASSOCIATION_SELECT_26002, key);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
//                        // For the current record associated with a given derived data point, 
//                        // try to locate the corresponding entry in the input association map.
//                        // If not found then add one.
//                        unsigned long derivedDataPointKey(data->getUnsignedLongData(i, DERIVED_DATAPOINT_PKEY));
//                        ConfigItemMap::iterator dataMapIt(dataMap.find(derivedDataPointKey));
//
//                        if (dataMapIt == dataMap.end())
//                        {
//                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
//                            InsertResult res(dataMap.insert(std::make_pair(derivedDataPointKey, ConfigItemList())));
//
//                            dataMapIt = res.first;
//                        }

                        // Add this input association to the relevant input association map element as located earlier.
                        InputAssociation inputAssociation;
                        inputAssociation.inputAssociationKey = data->getUnsignedLongData(i, INPUT_ASSOCIATION_PKEY);
                        inputAssociation.inputDataPointKey = data->getUnsignedLongData(i, INPUT_DATAPOINT_PKEY);
                        inputAssociation.inputCalculationOrder = data->getIntegerData(i, INPUT_CALCULATION_ORDER);
                        inputAssociation.inputValue = data->getRealData(i, INPUT_VALUE);

                        newInputAssociationData.push_back(inputAssociation);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for input association data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all input association data retrieved from database. "
                        "The returned input association map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving input association data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving input association data from database.");
        }

		m_dataMap.insert( std::make_pair(key, newInputAssociationData) ) ;		
		//m_ddpLoaded.insert( std::make_pair(key, true) );

        FUNCTION_EXIT;

		return newInputAssociationData;
	}

	void InputAssociationData::bulkLoadByAgentKey(unsigned long agentKey)
	{
        FUNCTION_ENTRY("bulkLoadByAgentKey");

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_DATAPOINT_PKEY = "MAIN_ENTITY_PKEY";
            const std::string INPUT_ASSOCIATION_PKEY = "SCIASS_ID";
            const std::string INPUT_DATAPOINT_PKEY = "INPUT_ENTITY_PKEY";
            const std::string INPUT_CALCULATION_ORDER = "INPUT_CALC_ORDER";
            const std::string INPUT_VALUE = "PREDEFINED_INPUT_VALUE";

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_DATAPOINT_PKEY);
            columnNames.push_back(INPUT_ASSOCIATION_PKEY);
            columnNames.push_back(INPUT_DATAPOINT_PKEY);
            columnNames.push_back(INPUT_CALCULATION_ORDER);
            columnNames.push_back(INPUT_VALUE);

            // Build an SQL query for extracting the input associations of all derived data points.
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_DATAPOINT_PKEY << ", "
                             << INPUT_ASSOCIATION_PKEY << ", " 
                             << INPUT_DATAPOINT_PKEY << ", " 
                             << INPUT_CALCULATION_ORDER << ", " 
                             << INPUT_VALUE << " "
                << "FROM SC_INPUT_ASSOCIATION" << " "
				<< "WHERE " << DERIVED_DATAPOINT_PKEY <<  " IN ( SELECT PKEY FROM ENTITY WHERE AGENTKEY = " 
				<< agentKey << ")";*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_INPUT_ASSOCIATION_STD_SELECT_26003, agentKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_INPUT_ASSOCIATION_SELECT_26003, agentKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_INPUT_ASSOCIATION_SELECT_26003, agentKey);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        unsigned long derivedDataPointKey(data->getUnsignedLongData(i, DERIVED_DATAPOINT_PKEY));
                        ConfigItemMap::iterator dataMapIt(m_dataMap.find(derivedDataPointKey));

                        if (dataMapIt == m_dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(m_dataMap.insert(std::make_pair(derivedDataPointKey, ConfigItemList())));

                            dataMapIt = res.first;

							//m_ddpLoaded.insert( std::make_pair(derivedDataPointKey, true) );		
                        }

                        // Add this input association to the relevant input association map element as located earlier.
                        InputAssociation inputAssociation;
                        inputAssociation.inputAssociationKey = data->getUnsignedLongData(i, INPUT_ASSOCIATION_PKEY);
                        inputAssociation.inputDataPointKey = data->getUnsignedLongData(i, INPUT_DATAPOINT_PKEY);
                        inputAssociation.inputCalculationOrder = data->getIntegerData(i, INPUT_CALCULATION_ORDER);
                        inputAssociation.inputValue = data->getRealData(i, INPUT_VALUE);

                        dataMapIt->second.push_back(inputAssociation);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
            //m_hasBulkLoaded = true;
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for input association data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all input association data retrieved from database. "
                        "The returned input association map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving input association data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving input association data from database.");
        }

        FUNCTION_EXIT;  
		
	}

    void InputAssociationData::bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey)
    {
        FUNCTION_ENTRY("bulkLoadByEquipmentDataNodeKey");

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_DATAPOINT_PKEY = "MAIN_ENTITY_PKEY";
            const std::string INPUT_ASSOCIATION_PKEY = "SCIASS_ID";
            const std::string INPUT_DATAPOINT_PKEY = "INPUT_ENTITY_PKEY";
            const std::string INPUT_CALCULATION_ORDER = "INPUT_CALC_ORDER";
            const std::string INPUT_VALUE = "PREDEFINED_INPUT_VALUE";

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_DATAPOINT_PKEY);
            columnNames.push_back(INPUT_ASSOCIATION_PKEY);
            columnNames.push_back(INPUT_DATAPOINT_PKEY);
            columnNames.push_back(INPUT_CALCULATION_ORDER);
            columnNames.push_back(INPUT_VALUE);

            // Build an SQL query for extracting the input associations of all derived data points.
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_DATAPOINT_PKEY << ", "
                             << INPUT_ASSOCIATION_PKEY << ", " 
                             << INPUT_DATAPOINT_PKEY << ", " 
                             << INPUT_CALCULATION_ORDER << ", " 
                             << INPUT_VALUE << " "
                << "FROM SC_INPUT_ASSOCIATION" << " "
				<< "WHERE " << DERIVED_DATAPOINT_PKEY <<  " IN ( SELECT PKEY FROM ENTITY WHERE PARENTKEY = " 
				<< dataNodeKey << ")";*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_INPUT_ASSOCIATION_STD_SELECT_26004,dataNodeKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_INPUT_ASSOCIATION_SELECT_26004,dataNodeKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_INPUT_ASSOCIATION_SELECT_26004,dataNodeKey);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        unsigned long derivedDataPointKey(data->getUnsignedLongData(i, DERIVED_DATAPOINT_PKEY));
                        ConfigItemMap::iterator dataMapIt(m_dataMap.find(derivedDataPointKey));

                        if (dataMapIt == m_dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(m_dataMap.insert(std::make_pair(derivedDataPointKey, ConfigItemList())));

                            dataMapIt = res.first;

							//m_ddpLoaded.insert( std::make_pair(derivedDataPointKey, true) );		
                        }

                        // Add this input association to the relevant input association map element as located earlier.
                        InputAssociation inputAssociation;
                        inputAssociation.inputAssociationKey = data->getUnsignedLongData(i, INPUT_ASSOCIATION_PKEY);
                        inputAssociation.inputDataPointKey = data->getUnsignedLongData(i, INPUT_DATAPOINT_PKEY);
                        inputAssociation.inputCalculationOrder = data->getIntegerData(i, INPUT_CALCULATION_ORDER);
                        inputAssociation.inputValue = data->getRealData(i, INPUT_VALUE);

                        dataMapIt->second.push_back(inputAssociation);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
            //m_hasBulkLoaded = true;
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for input association data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all input association data retrieved from database. "
                        "The returned input association map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving input association data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving input association data from database.");
        }

        FUNCTION_EXIT;
    }

    //--------------------------------------------------------------------------------------
    // OutputAssociationData
    //--------------------------------------------------------------------------------------

    void OutputAssociationData::populate(ConfigItemMap& dataMap) // std::map<unsigned long, std::vector<OutputAssociation>>
    {
        FUNCTION_ENTRY("populate()");

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_STATE_PKEY = "SCDSTA_ID";
            const std::string OUTPUT_ASSOCIATION_PKEY = "SCDOAS_ID";
            const std::string OUTPUT_DATAPOINT_PKEY = "OUTPUT_DP_PKEY";
            const std::string OUTPUT_VALUE = "OUTPUT_VALUE";

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_STATE_PKEY);
            columnNames.push_back(OUTPUT_ASSOCIATION_PKEY);
            columnNames.push_back(OUTPUT_DATAPOINT_PKEY);
            columnNames.push_back(OUTPUT_VALUE);

            // Build an SQL query for extracting the input associations of all derived data points.
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_STATE_PKEY << ", " 
                             << OUTPUT_ASSOCIATION_PKEY << ", " 
                             << OUTPUT_DATAPOINT_PKEY << ", " 
                             << OUTPUT_VALUE << " "
                << "FROM SC_DERIVED_OUTPUT_ASSOCIATION";*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_DERIVED_OUTPUT_ASSOCIATION_STD_SELECT_26501);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_DERIVED_OUTPUT_ASSOCIATION_SELECT_26501);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_OUTPUT_ASSOCIATION_SELECT_26501);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        // For the current record associated with a given derived state, 
                        // try to locate the corresponding entry in the output association map.
                        // If not found then add one.
                        unsigned long derivedStateKey(data->getUnsignedLongData(i, DERIVED_STATE_PKEY));
                        ConfigItemMap::iterator dataMapIt(dataMap.find(derivedStateKey));

                        if (dataMapIt == dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(dataMap.insert(std::make_pair(derivedStateKey, ConfigItemList())));

                            dataMapIt = res.first;
                        }

                        // Add this output association to the relevant output association map element as located earlier.
                        OutputAssociation outputAssociation;
                        outputAssociation.outputAssociationKey = data->getUnsignedLongData(i, OUTPUT_ASSOCIATION_PKEY);
                        outputAssociation.outputDataPointKey = data->getUnsignedLongData(i, OUTPUT_DATAPOINT_PKEY);
                        outputAssociation.outputValue = data->getIntegerData(i, OUTPUT_VALUE);

                        dataMapIt->second.push_back(outputAssociation);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for output association data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all output association data retrieved from database. "
                        "The returned output association map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving output association data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving output association data from database.");
        }

        FUNCTION_EXIT;
    }

	DdpConfigData<OutputAssociation>::ConfigItemList OutputAssociationData::populateData(unsigned long key)
	{
		FUNCTION_ENTRY("populateData");
		ConfigItemList newOutputAssociationData;

		//If buldLoad ready, then no need to get it from database.
		if (m_hasBulkLoaded)
		{
			FUNCTION_EXIT;
			return newOutputAssociationData;	
		}
		
        try
        {
            // Build the list of required column names.
            const std::string DERIVED_STATE_PKEY = "SCDSTA_ID";
            const std::string OUTPUT_ASSOCIATION_PKEY = "SCDOAS_ID";
            const std::string OUTPUT_DATAPOINT_PKEY = "OUTPUT_DP_PKEY";
            const std::string OUTPUT_VALUE = "OUTPUT_VALUE";

            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_STATE_PKEY);
            columnNames.push_back(OUTPUT_ASSOCIATION_PKEY);
            columnNames.push_back(OUTPUT_DATAPOINT_PKEY);
            columnNames.push_back(OUTPUT_VALUE);

            // Build an SQL query for extracting the input associations of all derived data points.
            /*std::ostringstream sql;
            sql << "SELECT " << DERIVED_STATE_PKEY << ", " 
                             << OUTPUT_ASSOCIATION_PKEY << ", " 
                             << OUTPUT_DATAPOINT_PKEY << ", " 
                             << OUTPUT_VALUE << " "
                << "FROM SC_DERIVED_OUTPUT_ASSOCIATION WHERE " << DERIVED_STATE_PKEY << " = " << key;*/

            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_DERIVED_OUTPUT_ASSOCIATION_STD_SELECT_26502,key);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_DERIVED_OUTPUT_ASSOCIATION_SELECT_26502,key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_OUTPUT_ASSOCIATION_SELECT_26502,key);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
		    
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());

                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
//                        // For the current record associated with a given derived state, 
//                        // try to locate the corresponding entry in the output association map.
//                        // If not found then add one.
//                        unsigned long derivedStateKey(data->getUnsignedLongData(i, DERIVED_STATE_PKEY));
//                        ConfigItemMap::iterator dataMapIt(dataMap.find(derivedStateKey));
//
//                        if (dataMapIt == dataMap.end())
//                        {
//                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
//                            InsertResult res(dataMap.insert(std::make_pair(derivedStateKey, ConfigItemList())));
//
//                            dataMapIt = res.first;
//                        }

                        // Add this output association to the relevant output association map element as located earlier.
                        OutputAssociation outputAssociation;
                        outputAssociation.outputAssociationKey = data->getUnsignedLongData(i, OUTPUT_ASSOCIATION_PKEY);
                        outputAssociation.outputDataPointKey = data->getUnsignedLongData(i, OUTPUT_DATAPOINT_PKEY);
                        outputAssociation.outputValue = data->getIntegerData(i, OUTPUT_VALUE);

                        newOutputAssociationData.push_back(outputAssociation);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;

                        throw;
                    }
                }

                // We are now finished with this data set. Destroy it before asking for more.
			    delete data;
			    data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for output association data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                        "Failed to process all output association data retrieved from database. "
                        "The returned output association map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving output association data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving output association data from database.");
        }

		m_dataMap.insert( std::make_pair(key, newOutputAssociationData) ) ;		
		//m_ddpLoaded.insert( std::make_pair(key, true) );
		
        FUNCTION_EXIT;
		
		return newOutputAssociationData;

	}

	void OutputAssociationData::bulkLoadByAgentKey(unsigned long agentKey)
	{
		FUNCTION_ENTRY("bulkLoadByAgentKey");
		
        try
        {
            // Build the list of required column names.
            const std::string DERIVED_STATE_PKEY = "SCDSTA_ID";
            const std::string OUTPUT_ASSOCIATION_PKEY = "SCDOAS_ID";
            const std::string OUTPUT_DATAPOINT_PKEY = "OUTPUT_DP_PKEY";
            const std::string OUTPUT_VALUE = "OUTPUT_VALUE";
			
            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_STATE_PKEY);
            columnNames.push_back(OUTPUT_ASSOCIATION_PKEY);
            columnNames.push_back(OUTPUT_DATAPOINT_PKEY);
            columnNames.push_back(OUTPUT_VALUE);
			
            // Build an SQL query
            /*std::ostringstream sql;
            sql << "SELECT SC_DERIVED_OUTPUT_ASSOCIATION." << DERIVED_STATE_PKEY << ", " 
				<< OUTPUT_ASSOCIATION_PKEY << ", " 
				<< OUTPUT_DATAPOINT_PKEY << ", " 
				<< OUTPUT_VALUE
                << " FROM SC_DERIVED_OUTPUT_ASSOCIATION,  SC_DERIVED_STATE"
				<< " WHERE SC_DERIVED_OUTPUT_ASSOCIATION.SCDSTA_ID = SC_DERIVED_STATE.SCDSTA_ID AND SC_DERIVED_STATE.DERIVED_DP_PKEY  IN (SELECT PKEY FROM ENTITY WHERE AGENTKEY = "
				<< agentKey << ")";*/

			
            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_DERIVED_OUTPUT_ASSOCIATION_STD_SELECT_26503,agentKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_DERIVED_OUTPUT_ASSOCIATION_SELECT_26503,agentKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_OUTPUT_ASSOCIATION_SELECT_26503,agentKey);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
			
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());
				
                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        unsigned long derivedStateKey(data->getUnsignedLongData(i, DERIVED_STATE_PKEY));
                        ConfigItemMap::iterator dataMapIt(m_dataMap.find(derivedStateKey));
						
                        if (dataMapIt == m_dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(m_dataMap.insert(std::make_pair(derivedStateKey, ConfigItemList())));
							//m_ddpLoaded.insert( std::make_pair(derivedStateKey, true) );
                            dataMapIt = res.first;
                        }
						
                        OutputAssociation outputAssociation;
                        outputAssociation.outputAssociationKey = data->getUnsignedLongData(i, OUTPUT_ASSOCIATION_PKEY);
                        outputAssociation.outputDataPointKey = data->getUnsignedLongData(i, OUTPUT_DATAPOINT_PKEY);
                        outputAssociation.outputValue = data->getIntegerData(i, OUTPUT_VALUE);
						
                        dataMapIt->second.push_back(outputAssociation);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;
						
                        throw;
                    }
                }
				
                // We are now finished with this data set. Destroy it before asking for more.
				delete data;
				data = NULL;
            }
            while (databaseConnection->moreData(data));
            m_hasBulkLoaded = true;
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for output association data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"Failed to process all output association data retrieved from database. The returned output association map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving output association data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving output association data from database.");
        }
		
        FUNCTION_EXIT;
	}

    void OutputAssociationData::bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey)
    {
		FUNCTION_ENTRY("bulkLoadByEquipmentDataNodeKey");

        try
        {
            // Build the list of required column names.
            const std::string DERIVED_STATE_PKEY = "SCDSTA_ID";
            const std::string OUTPUT_ASSOCIATION_PKEY = "SCDOAS_ID";
            const std::string OUTPUT_DATAPOINT_PKEY = "OUTPUT_DP_PKEY";
            const std::string OUTPUT_VALUE = "OUTPUT_VALUE";
			
            std::vector<std::string> columnNames;
            columnNames.push_back(DERIVED_STATE_PKEY);
            columnNames.push_back(OUTPUT_ASSOCIATION_PKEY);
            columnNames.push_back(OUTPUT_DATAPOINT_PKEY);
            columnNames.push_back(OUTPUT_VALUE);
			
            // Build an SQL query
            /*std::ostringstream sql;
            sql << "SELECT SC_DERIVED_OUTPUT_ASSOCIATION." << DERIVED_STATE_PKEY << ", " 
				<< OUTPUT_ASSOCIATION_PKEY << ", " 
				<< OUTPUT_DATAPOINT_PKEY << ", " 
				<< OUTPUT_VALUE
                << " FROM SC_DERIVED_OUTPUT_ASSOCIATION,  SC_DERIVED_STATE"
				<< " WHERE SC_DERIVED_OUTPUT_ASSOCIATION.SCDSTA_ID = SC_DERIVED_STATE.SCDSTA_ID AND SC_DERIVED_STATE.DERIVED_DP_PKEY  IN (SELECT PKEY FROM ENTITY WHERE PARENTKEY = "
				<< dataNodeKey << ")";*/

			
            // Get a connection to the database and execute the query.
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_DERIVED_OUTPUT_ASSOCIATION_STD_SELECT_26504,dataNodeKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(SC_DERIVED_OUTPUT_ASSOCIATION_SELECT_26504,dataNodeKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_OUTPUT_ASSOCIATION_SELECT_26504,dataNodeKey);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
			
            // Iterate over all sets of returned data.
            do
            {
                unsigned long rowCount(data->getNumRows());
				
                // Iterate over the records in each data set.            
                for (unsigned long i(0); i < rowCount; i++)
                {
                    try
                    {
                        unsigned long derivedStateKey(data->getUnsignedLongData(i, DERIVED_STATE_PKEY));
                        ConfigItemMap::iterator dataMapIt(m_dataMap.find(derivedStateKey));
						
                        if (dataMapIt == m_dataMap.end())
                        {
                            typedef std::pair<ConfigItemMap::iterator, bool> InsertResult;
                            InsertResult res(m_dataMap.insert(std::make_pair(derivedStateKey, ConfigItemList())));
							//m_ddpLoaded.insert( std::make_pair(derivedStateKey, true) );
                            dataMapIt = res.first;
                        }
						
                        OutputAssociation outputAssociation;
                        outputAssociation.outputAssociationKey = data->getUnsignedLongData(i, OUTPUT_ASSOCIATION_PKEY);
                        outputAssociation.outputDataPointKey = data->getUnsignedLongData(i, OUTPUT_DATAPOINT_PKEY);
                        outputAssociation.outputValue = data->getIntegerData(i, OUTPUT_VALUE);
						
                        dataMapIt->second.push_back(outputAssociation);
                    }
                    catch (TA_Base_Core::DataException&)
                    {
                        // Clean up the data pointer before bailing out.
                        delete data;
                        data = NULL;
						
                        throw;
                    }
                }
				
                // We are now finished with this data set. Destroy it before asking for more.
				delete data;
				data = NULL;
            }
            while (databaseConnection->moreData(data));
            m_hasBulkLoaded = true;
        }
        catch (const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to access database for output association data.");
        }
        catch (const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"Failed to process all output association data retrieved from database. The returned output association map will be incomplete.");
        }
        catch (const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving output association data from database.");
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown exception");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error while retrieving output association data from database.");
        }
		
        FUNCTION_EXIT;
    }

    //--------------------------------------------------------------------------------------
    // DerivedDataPointConfigCache
    //--------------------------------------------------------------------------------------

    DerivedDataPointConfigCache* DerivedDataPointConfigCache::s_instance = NULL;
    NonReEntrantThreadLockable DerivedDataPointConfigCache::s_instanceLock;


    DerivedDataPointConfigCache& DerivedDataPointConfigCache::getInstance()
    {
		if (s_instance == NULL)
		{
			ThreadGuard guard(s_instanceLock);

			if (s_instance == NULL)
			{
				s_instance = new DerivedDataPointConfigCache;
			}
		}
        return *s_instance;
    }


    DerivedStateList DerivedDataPointConfigCache::getDerivedStates(unsigned long derivedDataPointKey)
    {
        return m_derivedStates.getData(derivedDataPointKey);
    }


    InputAssociationList DerivedDataPointConfigCache::getInputAssociations(unsigned long derivedDataPointKey)
    {
        return m_inputAssociations.getData(derivedDataPointKey);
    }


    OutputAssociationList DerivedDataPointConfigCache::getOutputAssociations(unsigned long derivedStateKey)
    {
        return m_outputAssociations.getData(derivedStateKey);
    }

	void DerivedDataPointConfigCache::bulkLoadByAgentKey(unsigned long agentKey)
	{
		m_derivedStates.bulkLoadByAgentKey( agentKey );
		m_inputAssociations.bulkLoadByAgentKey( agentKey );
		m_outputAssociations.bulkLoadByAgentKey( agentKey );
	}

    void DerivedDataPointConfigCache::bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey)
    {
		m_derivedStates.bulkLoadByEquipmentDataNodeKey( dataNodeKey );
		m_inputAssociations.bulkLoadByEquipmentDataNodeKey( dataNodeKey );
		m_outputAssociations.bulkLoadByEquipmentDataNodeKey( dataNodeKey );
    }

} // closes TA_Base_Core

