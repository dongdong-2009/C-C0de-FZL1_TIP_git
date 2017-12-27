// INamingPort.cpp: implementation of the INamingPort class.
//
//////////////////////////////////////////////////////////////////////

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/NamingPort.h"

#include "core/utilities/src/FunctionEntry.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/SQLCode.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
/**
  * 
  * NamingPort is an implementation of INamingPort. It holds the data specific to a agent entry
  * in the database, and allows read-only access to that data.
  *
  * This class will access to the following DB tables: SN_SYSTEM_CONTROLLER, SN_MANAGED_PROCESS, ENTITY, NAMING_PORTS.
  * This class is added for CL-21230
  */

	static const int Control_Mode = 0;
	static const int Monitor_Mode = 1;

	NamingPort::NamingPort(const std::string agentName)
		: m_isValidData(false)
		, m_agentName(agentName)
		, m_primaryHostname("")
		, m_secondaryHostname("")
		, m_primaryPort(0)
		, m_secondaryPort(0)
	{
		FUNCTION_ENTRY("NamingPort");
		FUNCTION_EXIT;
	}

	void NamingPort::loadDataFromDb()
	{
		FUNCTION_ENTRY("loadDataFromDb");
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Naming_Cd, Read);
		int nDbServerType = databaseConnection->getDbServerType();

		// query host name of agent
	/*	std::string sql = "select sc.NAME as HOSTNAME, mp.DEFAULT_OPERATING_MODE, et.NAME as AGNENTNAME, et.PKEY as ENTITYKEY ";
		sql += "from SN_SYSTEM_CONTROLLER sc, SN_MANAGED_PROCESS mp, ENTITY et ";
		sql += "where sc.SNSCON_ID = mp.SNSCON_ID and mp.MANAGED_PROCESS_PKEY = et.PARENTKEY and et.NAME = '";
		sql += databaseConnection->escapeAQSQLString(m_agentName) + "'";*/
//		std::string strSql = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23511,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23511,
			databaseConnection->escapeAQSQLString(m_agentName));

		// Set up the columnNames vector to be passed to executeQuery()
		std::string hostnameColumn		= "HOSTNAME";
		std::string operatingModeColumn = "DEFAULT_OPERATING_MODE";
		std::string agentNameColumn		= "AGNENTNAME";
		std::string entityKeyColumn		= "ENTITYKEY";

		std::vector<std::string> columnNames;
		columnNames.push_back(hostnameColumn);
		columnNames.push_back(operatingModeColumn);
		columnNames.push_back(agentNameColumn);
		columnNames.push_back(entityKeyColumn);

		// Execute the query
		IData* data = databaseConnection->executeQuery(strSql, columnNames);

		// 
		if(NULL == data || 0 == data->getNumRows()) // No entry found with the specified message_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No hostname found for agentName = %s",m_agentName.c_str());			
            throw DataException(reasonMessage,TA_Base_Core::DataException::NO_VALUE,"");
        }
		else if(2 < data->getNumRows())
		{
			delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than two hostname found for agentName = %s",m_agentName.c_str());			
            throw DataException(reasonMessage,TA_Base_Core::DataException::NOT_UNIQUE,"");
		}

		for(int i = 0; i < (int)data->getNumRows(); ++i)
		{
			if(data->getIntegerData(i, operatingModeColumn) == Control_Mode)
			{
				m_primaryHostname = data->getStringData(i, hostnameColumn);
			}
			else
			{
				m_secondaryHostname = data->getStringData(i, hostnameColumn);
			}
		}

		unsigned long entityKey = data->getUnsignedLongData(0, entityKeyColumn);
		delete data;
		data = NULL;

		// get naming service port
		/*std::ostringstream querySql;
		querySql << "select PRIMARY_PORT, SECONDARY_PORT from NAMING_PORTS where ENTITY_KEY = "
			<< entityKey;*/
	//	strSql = defPrepareSQLStatement(nDbServerType, SN_SYSTEM_CONTROLLER_STD_SELECT_23512, entityKey);
//		strSql = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23512, entityKey);

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23512, entityKey);

		// setup columns vector
		columnNames.clear();
		std::string primaryPortColumn	= "PRIMARY_PORT";
		std::string secondaryPortColumn = "SECONDARY_PORT";
		columnNames.push_back(primaryPortColumn);
		columnNames.push_back(secondaryPortColumn);

		data = databaseConnection->executeQuery(strSql, columnNames);

		if(NULL == data || 0 == data->getNumRows()) // No entry found with the specified message_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No naming_port found for agentName = %s",m_agentName.c_str());			
            throw DataException(reasonMessage,TA_Base_Core::DataException::NO_VALUE,"");
        }
		else if(1 < data->getNumRows())
		{
			delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one naming_port found for agentName = %s",m_agentName.c_str());			
            throw DataException(reasonMessage,TA_Base_Core::DataException::NOT_UNIQUE,"");
		}

		m_primaryPort = data->getUnsignedLongData(0, primaryPortColumn);
		m_secondaryPort = data->getUnsignedLongData(0, secondaryPortColumn);

		delete data;
		data = NULL;

		m_isValidData = true;

		FUNCTION_EXIT;
	}

	std::string NamingPort::getName()
	{
		FUNCTION_ENTRY("getName");
		FUNCTION_EXIT;
		return m_agentName;
	}

	std::string NamingPort::getPrimaryHostname()
	{
		FUNCTION_ENTRY("getPrimaryHostname");
		ThreadGuard guard(m_dataLoadLock);
		if(!m_isValidData)
		{
			loadDataFromDb();
		}

		FUNCTION_EXIT;
		return m_primaryHostname;
	}

	std::string NamingPort::getSecondaryHostname()
	{
		FUNCTION_ENTRY("getSecondaryHostname");
		ThreadGuard guard(m_dataLoadLock);
		if(!m_isValidData)
		{
			loadDataFromDb();
		}

		FUNCTION_EXIT;
		return m_secondaryHostname;
	}

	unsigned long NamingPort::getPrimaryPort()
	{
		FUNCTION_ENTRY("getPrimaryPort");
		ThreadGuard guard(m_dataLoadLock);
		if(!m_isValidData)
		{
			loadDataFromDb();
		}

		FUNCTION_EXIT;
		return m_primaryPort;
	}

	unsigned long NamingPort::getSecondaryPort()
	{
		FUNCTION_ENTRY("getSecondaryPort");
		ThreadGuard guard(m_dataLoadLock);
		if(!m_isValidData)
		{
			loadDataFromDb();
		}

		FUNCTION_EXIT;
		return m_secondaryPort;
	}

	unsigned long NamingPort::getNsPort(const std::string hostname)
	{
		FUNCTION_ENTRY("getNsPort");

		ThreadGuard guard(m_dataLoadLock);
		if(!m_isValidData)
		{
			loadDataFromDb();
		}

		unsigned long nsPort = 0;
		if(hostname.compare(m_primaryHostname) == 0)
		{
			nsPort = m_primaryPort;
		}
		else if(hostname.compare(m_secondaryHostname) == 0)
		{
			nsPort = m_secondaryPort;
		}

		FUNCTION_EXIT;
		return nsPort;
	}
} // closes TA_Base_Core

