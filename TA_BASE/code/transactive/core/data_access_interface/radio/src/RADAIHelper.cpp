// RADAIHelper.cpp: implementation of the RADAIHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "RADAIHelper.h"


//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"

#include "core/utilities/src/RunParams.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Core
{

	RADAIHelper::RADAIHelper()
	{

	}

	RADAIHelper::~RADAIHelper()
	{

	}


	std::string RADAIHelper::getLocalDatabaseName()
	{
		FUNCTION_ENTRY("getLocalDatabaseName");
		std::string localDatabase;

		// have to find out what is the local db name
		// this should be the first in the connection list so grab it
		if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw DatabaseException("db-connection-file not set");
		}
		try
		{
			std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Radio_Cd, Write);

			/*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			DataConnections connections = connectionStrings.getConnectionList(Radio_Cd, Write);
			localDatabase = connections[0].first;*/  
		}
		catch(...) // most likely a file not found error
		{
			throw DatabaseException("Unable to find database connection infomation");
		}

		FUNCTION_EXIT;
		return localDatabase;
	}

	long RADAIHelper::getLocationKey()
	{
		FUNCTION_ENTRY("getLocationKey");
		try
		{
			std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			long locationKey = 0;

			TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID);
			locationKey = console->getLocation();
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Location Key = %d", locationKey);

			delete console;

			FUNCTION_EXIT;
			return locationKey;
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught while getting LocationKey");
			return 0;
		}
	}
}