// ScadaBusinessManagerAccessFactoryImpl.cpp: implementation of the ScadaBusinessManagerAccessFactoryImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "core/data_access_interface/src/ScadaBusinessManagerAccessFactoryImpl.h"
#include "core/utilities/src/FunctionEntry.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/database/src/DBException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/sqlmacrodef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
    static const std::string ScadaBusinessTypeName[] =
    {
		"UNKNOWN",				// SCADA_BUSINESS_TYPE_UNKNOWN = 0
        "HEARTBEAT",			// SCADA_BUSINESS_TYPE_HEARTBEAT
        "MONITOR_AND_WRITE",	// SCADA_BUSINESS_TYPE_MONITOR_AND_WRITE
        ""
    };

	static const std::string ScadaBusinessSubsystemName[] =
	{
	    "UNKNOWN", 				// SCADA_BUSINESS_SUBSYSTEM_UNKNOWN = 0,
		"PA", 					// SCADA_BUSINESS_SUBSYSTEM_PA,
		"CCTV", 				// SCADA_BUSINESS_SUBSYSTEM_CCTV,
		"PIDS", 				// SCADA_BUSINESS_SUBSYSTEM_PIDS,
		"BAS", 					// SCADA_BUSINESS_SUBSYSTEM_BAS,
		"FAS", 					// SCADA_BUSINESS_SUBSYSTEM_FAS
		""
	};

	ScadaBusinessManagerAccessFactoryImpl::ScadaBusinessManagerAccessFactoryImpl()
		: m_item_map()
	{	
	}
	
	ScadaBusinessManagerAccessFactoryImpl::~ScadaBusinessManagerAccessFactoryImpl()
	{	
		ScadaBusinessItemMap::iterator iter = m_item_map.begin();
		while (iter != m_item_map.end())
		{
			ScadaBusinessItemVector* vec = iter->second;
			if (vec != NULL)
			{
				unsigned int size = vec->size();
				for (unsigned int i = 0; i < size; ++i)
				{
					delete (*vec)[i];
				}
				delete vec;
			}
			++iter;
		}
		m_item_map.clear();
	}

	const ScadaBusinessItemVector & ScadaBusinessManagerAccessFactoryImpl::getScadaBusinessItems(const std::string& agentName)
	{
		FUNCTION_ENTRY("getScadaBusinessItems");
		
		ScadaBusinessItemVector * ivec = NULL;
			
		ScadaBusinessItemMap::iterator iter = m_item_map.find(agentName);
		if (iter == m_item_map.end())
		{
			ivec = loadDataFromDb(agentName);
			m_item_map.insert(ScadaBusinessItemMap::value_type(agentName, ivec));
		}
		else
		{
			ivec = iter->second;
		}

		FUNCTION_EXIT;
		return *ivec;
	}
	
	ScadaBusinessItemVector * ScadaBusinessManagerAccessFactoryImpl::loadDataFromDb(const std::string& agentName)
	{
		FUNCTION_ENTRY("loadDataFromDb");
		ScadaBusinessItemVector * ivec = new ScadaBusinessItemVector;
		TA_ASSERT( NULL != ivec, "Failed to create ScadaBusinessItemVector");

		if (agentName.empty())
		{
			FUNCTION_EXIT;
			return ivec;			
		}

		IDatabase* databaseConnection = NULL;
		try
		{
			databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
		}
		catch (const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "getDatabase DatabaseException", ex.what());
			FUNCTION_EXIT;
			return ivec;
		}
		TA_ASSERT( NULL != databaseConnection, "Failed to create databaseConnection");

		try 
		{
			int cnt = 0;
	        while (ScadaBusinessTypeName[cnt] != "")
	        {
				EScadaBusinessType btype = static_cast<EScadaBusinessType>(cnt);
	            switch (btype)
	            {
				case SCADA_BUSINESS_TYPE_HEARTBEAT:
					loadHeartBeat(ivec, databaseConnection, agentName);
					break;

				case SCADA_BUSINESS_TYPE_MONITOR_AND_WRITE:
					loadMonitorAndWrite(ivec, databaseConnection, agentName);
					break;

				default:
					break;                
	            }
	            ++cnt;
	        }
		}
		catch (const DBException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "query DBException", ex.what());
		}
		catch (const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "query DatabaseException", ex.what());
		}
		catch (const TA_Base_Core::DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "query DataException", ex.what());
		}
		
		FUNCTION_EXIT;
		return ivec;
	}

	void ScadaBusinessManagerAccessFactoryImpl::loadHeartBeat(ScadaBusinessItemVector* ivec , 
		IDatabase* databaseConnection, const std::string& agentName)
	{
		FUNCTION_ENTRY("loadHeartBeat");
		
		/*std::string sql = "select sh.INTERVAL_TIME as INTERVAL_TIME, sh.ENTITY_LIST as ENTITY_LIST ";
		sql += "from SC_SCADA_BUSINESS sb, SC_SCADA_HEARTBEAT sh ";
		sql += "where sb.ITEMKEY = sh.PKEY ";
		sql += "and trim(sb.ITEMTYPE) = '";
		sql += databaseConnection->escapeQueryString(ScadaBusinessTypeName[SCADA_BUSINESS_TYPE_HEARTBEAT]) + "' ";
		sql += "and trim(sb.ENTITYNAME) = '";
		sql += databaseConnection->escapeQueryString(agentName) + "'";*/

		std::string typeName = databaseConnection->escapeQueryString(ScadaBusinessTypeName[SCADA_BUSINESS_TYPE_HEARTBEAT]);
		std::string strAgentName = databaseConnection->escapeQueryString(agentName);
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SC_SCADA_BUSINESS_SELECT_107002, typeName, strAgentName);
		
		std::string intervalTimeColumn	= "INTERVAL_TIME";
		std::string entityListColumn 	= "ENTITY_LIST";

		std::vector<std::string> columnNames;
		columnNames.push_back(intervalTimeColumn);
		columnNames.push_back(entityListColumn);

		IData* data = databaseConnection->executeQuery(sql, columnNames);

		unsigned long interval = 0;
		std::string entityList = "";
		ScadaBusinessHeartbeat * sbitem = NULL;

		unsigned long size = data->getNumRows();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Loaded %d heartbeat for %s", size, agentName.c_str());
		
		for (unsigned long i = 0; i < size; ++i)
		{
			interval = data->getUnsignedLongData(i, intervalTimeColumn);
			entityList = data->getStringData(i, entityListColumn);
			sbitem = new ScadaBusinessHeartbeat(agentName, interval);
			ivec->push_back(sbitem);
			splitString(entityList, sbitem->getOutputEntitiesRef());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Created heartbeat %d, interval=%d", i, interval);
		}

		delete data;
		FUNCTION_EXIT;
	}

	void ScadaBusinessManagerAccessFactoryImpl::loadMonitorAndWrite(ScadaBusinessItemVector* ivec , 
		IDatabase* databaseConnection, const std::string& agentName)
	{
		FUNCTION_ENTRY("loadMonitorAndWrite");
			
		/*std::string sql = "select sh.FEP_LOCATION as FEP_LOCATION, trim(sh.FEP_SUBSYSTEM) as FEP_SUBSYSTEM, ";
		sql += "sh.ENTITY_LIST as ENTITY_LIST, sh.OUTPUT_ORDER as OUTPUT_ORDER ";			
		sql += "from SC_SCADA_BUSINESS sb, SC_SCADA_MONITOR_AND_WRITE sh ";
		sql += "where sb.ITEMKEY = sh.PKEY ";
		sql += "and trim(sb.ITEMTYPE) = '";
		sql += databaseConnection->escapeQueryString(ScadaBusinessTypeName[SCADA_BUSINESS_TYPE_MONITOR_AND_WRITE]) + "' ";
		sql += "and trim(sb.ENTITYNAME) = '";
		sql += databaseConnection->escapeQueryString(agentName) + "'";*/
		
		std::string typeName = databaseConnection->escapeQueryString(ScadaBusinessTypeName[SCADA_BUSINESS_TYPE_MONITOR_AND_WRITE]);
		std::string strAgentName = databaseConnection->escapeQueryString(agentName);
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SC_SCADA_BUSINESS_SELECT_107001, typeName, strAgentName);
			
		std::string fepLocationColumn	= "FEP_LOCATION";
		std::string fepSubsystemColumn 	= "FEP_SUBSYSTEM";
		std::string entityListColumn 	= "ENTITY_LIST";
		std::string OutputOrderColumn 	= "OUTPUT_ORDER";

		std::vector<std::string> columnNames;
		columnNames.push_back(fepLocationColumn);
		columnNames.push_back(fepSubsystemColumn);
		columnNames.push_back(entityListColumn);
		columnNames.push_back(OutputOrderColumn);

		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);
		
		unsigned long fepLocation = 0;
		std::string fepSubsystem = "";
		std::string entityList = "";
		std::string outputOrder = "";
		ScadaBusinessMonitorAndWrite * sbitem = NULL;

		unsigned long size = data->getNumRows();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Loaded %d monitor_and_write for %s", size, agentName.c_str());

		for (unsigned long i = 0; i < size; ++i)
		{
			fepLocation = data->getUnsignedLongData(i, fepLocationColumn);
			fepSubsystem = data->getStringData(i, fepSubsystemColumn);
			entityList = data->getStringData(i, entityListColumn);
			outputOrder = data->getStringData(i, OutputOrderColumn);

			sbitem = new ScadaBusinessMonitorAndWrite(agentName, fepLocation, getSubsystemEnumFromString(fepSubsystem));
			ivec->push_back(sbitem);
			splitString(entityList, sbitem->getInputEntitesRef());
			splitString(outputOrder, sbitem->getOutputEntitiesOrderingRef());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Created monitor_and_write %d, fep=%s(%d)", i, fepSubsystem.c_str(), fepLocation);
		}

		delete data;
		FUNCTION_EXIT;
	}

	EScadaBusinessSubsystem ScadaBusinessManagerAccessFactoryImpl::getSubsystemEnumFromString(const std::string& fepSubsystem)
	{
		FUNCTION_ENTRY("getSubsystemEnumFromString");
		
		EScadaBusinessSubsystem senum = SCADA_BUSINESS_SUBSYSTEM_UNKNOWN;
		
		int cnt = 0;
        while (ScadaBusinessSubsystemName[cnt] != "")
        {
			if (ScadaBusinessSubsystemName[cnt] == fepSubsystem)
			{
				senum = static_cast<EScadaBusinessSubsystem>(cnt);
				break;
            }
            ++cnt;
        }
		FUNCTION_EXIT;
		return senum;
	}

	void ScadaBusinessManagerAccessFactoryImpl::splitString(const std::string & str, ScadaBusinessEntitiesVector & vec)
	{
		FUNCTION_ENTRY("splitString");
		std::vector<std::string> strvec;
		getTokensFromString(str, strvec);

		unsigned long value = 0;
		unsigned int size = strvec.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (isNumberString(strvec[i]))
			{
				value = atol(strvec[i].c_str());
				vec.push_back(value);
			}			
		}
		
		FUNCTION_EXIT;
	}

	void ScadaBusinessManagerAccessFactoryImpl::getTokensFromString(const std::string& theString,
		std::vector<std::string> & strvec)
	{
		FUNCTION_ENTRY("getTokensFromString");
		
		std::string token;
	    std::string::size_type pos = 0;
	    
	    while ( pos < theString.length() )
	    {
	        if ( theString[pos] == ',' )
	        {
	            if ( pos > 0 && theString[pos - 1] != ',' )
	            {
					trimString(token);
				    if (!token.empty())
					{
						strvec.push_back(token);
				    }
	                token = "";
	            }
	        }
	        else
	        {
	            token += theString[pos];
	        }
			++pos;
	    }

		trimString(token);
	    if (!token.empty())
		{
			strvec.push_back(token);
	    }
		FUNCTION_EXIT;
	}

	void ScadaBusinessManagerAccessFactoryImpl::trimString(std::string & arg)
	{
		FUNCTION_ENTRY("trimString");

		if (!arg.empty())
		{
			std::string::size_type x = arg.find_first_not_of(" ");
			if (x != std::string::npos)
			{
				std::string::size_type y = arg.find_last_not_of(" ");
				arg = arg.substr(x, y - x + 1);
			}
			else
			{
				arg = "";
			}
		}

		FUNCTION_EXIT;
	}

	bool ScadaBusinessManagerAccessFactoryImpl::isNumberString(const std::string & target)
	{
		FUNCTION_ENTRY("isNumberString");
		bool valid = false;

		if (!target.empty())
		{
			std::string::size_type pos = target.find_first_not_of("0123456789");
			if (pos == std::string::npos)
			{
				static char tempBuffer[256];
				sprintf(tempBuffer, "%lu", ULONG_MAX);
				static const std::string maxul(tempBuffer);

				if (target.size() >= maxul.size())
				{
					int numberComparison = 1;
					if (target.size() == maxul.size())
					{
						numberComparison = target.compare(maxul);
					}

					if (numberComparison <= 0)
					{
						valid = true;
					}
				}
				else
				{
					valid = true;
				}
			}
		}

		FUNCTION_EXIT;
		return valid;
	}
}
