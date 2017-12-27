/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TisConfigAccess.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is a utility class. It's used to retrieve from the database
  * various TIS related configuration data items.
  *
  */

#include "StdAfx.h"
#include <sstream>
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryTable.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "TisConfigAccess.h"
#include "StationZonesProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TisConfigAccess

std::string TisConfigAccess::getSTisMessageText(const TA_Base_Core::TISMessage& message)
{
    FUNCTION_ENTRY( "getSTisMessageText" );
    FUNCTION_EXIT;
    return getTisMessageText(message, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);
}


std::string TisConfigAccess::getTTisMessageText(const TA_Base_Core::TISMessage& message)
{
    FUNCTION_ENTRY( "getTTisMessageText" );
    FUNCTION_EXIT;
    return getTisMessageText(message, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS);
}


std::string TisConfigAccess::getTisMessageText(const TA_Base_Core::TISMessage& message, const std::string& libraryType)
{
    FUNCTION_ENTRY( "getTisMessageText" );

    std::string messageText("");

    if (message.messageId != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID)
    {
        try
        {
            // Retrieve message text from the database.
            // Format as follows: "[<msg type>] <msg id>: <msg text>"
            std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary> msgLib(
                TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(
                    message.libraryVersion, libraryType));

            TA_Base_Core::PredefinedMessage* predefMsg = msgLib->getMessage(message.librarySection, message.messageId);

            if (predefMsg == NULL)
            {
                TA_THROW(TA_Base_Core::TransactiveException("Specified message not found in the source library."));
            }

            messageText = predefMsg->message;
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            std::ostringstream error;
            error << "Failed to resolve " << libraryType << " message " << message.messageId
                  << " in library version " << message.libraryVersion << ", section " << message.librarySection
                  << " (" << ex.what() << ")";

            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", error.str().c_str());
        }
        catch (...)
        {
            std::ostringstream error;
            error << "Failed to resolve " << libraryType << " message " << message.messageId
                  << " in library version " << message.libraryVersion << ", section " << message.librarySection;

            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", error.str().c_str());
        }
    }

    FUNCTION_EXIT;
    return messageText;
}


TisConfigAccess::PredefinedTisMessageList TisConfigAccess::getSTisMessages(unsigned short libraryVersion)
{
    FUNCTION_ENTRY( "getSTisMessages" );

    PredefinedTisMessageList messages;

    getTisMessages(libraryVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS, messages);

    FUNCTION_EXIT;
    return messages;
}


TisConfigAccess::PredefinedTisMessageList TisConfigAccess::getTTisMessages(unsigned short libraryVersion)
{
    FUNCTION_ENTRY( "getTTisMessages" );

    PredefinedTisMessageList messages;

    getTisMessages(libraryVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS, messages);

    FUNCTION_EXIT;
    return messages;
}


void TisConfigAccess::getTisMessages(unsigned short libraryVersion, const std::string& libraryType, PredefinedTisMessageList& messages)
{
    FUNCTION_ENTRY( "getTisMessages" );

    try
    {
        // Retrieve from the database all messages matching the specified library type and version.
        std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary> msgLib(
            TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(libraryVersion, libraryType));

        TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList predefMsgs(msgLib->getMessages());
        messages.reserve(predefMsgs.size());

        for (TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList::const_iterator mIt(predefMsgs.begin());
             mIt != predefMsgs.end(); mIt++)
        {
            TA_Base_Core::PredefinedMessage* predefMsg = *mIt;

            if (predefMsg != NULL)
            {
                messages.push_back(*predefMsg);
            }
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream error;
        error << "Failed to retrieve " << libraryType << " messages from library version" << libraryVersion
              << " (" << ex.what() << ")";

        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", error.str().c_str());
    }
    catch (...)
    {
        std::ostringstream error;
        error << "Failed to retrieve " << libraryType << " messages from library version" << libraryVersion;

        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", error.str().c_str());
    }

    FUNCTION_EXIT;
}

TisConfigAccess::TisStationNameSet TisConfigAccess::getPidsZones()
{
	FUNCTION_ENTRY( "getPidsZones" );
	std::vector<std::string> tmpvector = makezones();
	FUNCTION_EXIT;
	return TisStationNameSet(tmpvector.begin(),tmpvector.end());
}

TisConfigAccess::TisStationNameSet TisConfigAccess::getSTisStations()
{
    FUNCTION_ENTRY( "getSTisStations" );

    TisStationNameSet stations;

    try
    {
        // This SQL query will return the unique names of all stations that have associated TIS PIDs.
        const std::string STATION_COLUMN("STATION_NAME");

        //std::ostringstream sql;
        //sql << " SELECT UNIQUE SUBSTR(NAME, 1, INSTR(NAME, '.', 1, 1) - 1) AS " << STATION_COLUMN
        //    << " FROM ENTITY_V "
        //    << " WHERE NAME LIKE '%.TIS.%.%.%' "
        //    << "   AND NAME NOT LIKE '%.TIS.STIS.%.%' "
        //    << "   AND NAME NOT LIKE '%.TIS.ISCS.%.%' "
        //    << "   AND TYPEKEY = (SELECT PKEY "
        //    <<                  " FROM ENTITYTYPE "
        //    <<                  " WHERE NAME = '" << TA_Base_Core::DataPointEntityData::getStaticType() << "')";

        std::vector<std::string> columnNames;
        columnNames.push_back(STATION_COLUMN);

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it,
        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);
        //std::auto_ptr<TA_Base_Core::IData> data(database->executeQuery(sql.str(), columnNames));
        
		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, ENTITY_V_SELECT_2528,
			TA_Base_Core::DataPointEntityData::getStaticType());
			
		std::auto_ptr<TA_Base_Core::IData> data(database->executeQuery(strSql, columnNames));


        if (data->getNumRows() == 0)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "There are no TIS-enabled stations");
        }

        // Transfer the retrieved data to the station name container.
        for (unsigned long i(0); i < data->getNumRows(); i++)
        {
            std::string stationName(data->getStringData(i, STATION_COLUMN));

            if (!stationName.empty())
            {
                stations.insert(stationName);
            }
        }

        FUNCTION_EXIT;
        return stations;
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream error;
        error << "Failed to retrieve TIS station names from database."
              << " (" << ex.what() << ")";

        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", error.str().c_str());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Failed to retrieve TIS station names from database.");
    }

    stations.clear();

    FUNCTION_EXIT;
    return stations;
}


TisConfigAccess::TisStationLevelSet TisConfigAccess::getSTisLevels(const std::string& stationName)
{
    FUNCTION_ENTRY( "getSTisLevels" );

    TisStationLevelSet levels;

    try
    {
        // This SQL query will return the unique station level names at the specified station.
        // If no station name is specifed, then station levels of ALL stations are returned.
        const std::string LEVEL_COLUMN("STATION_LEVEL");

        std::string stationMask(stationName.empty()? "%" : stationName);

        //std::ostringstream sql;
        //sql << " SELECT UNIQUE SUBSTR(NAME, INSTR(NAME, '.', 1, 2) + 1, INSTR(NAME, '.', 1, 3) - INSTR(NAME, '.', 1, 2) - 1) "
        //    << " AS " << LEVEL_COLUMN
        //    << " FROM ENTITY_V "
        //    << " WHERE NAME LIKE '" << stationMask << ".TIS.%.%.%' "
        //    << "   AND NAME NOT LIKE '" << stationMask << ".TIS.STIS.%.%' "
        //    << "   AND NAME NOT LIKE '" << stationMask << ".TIS.ISCS.%.%' "
        //    << "   AND TYPEKEY = (SELECT PKEY "
        //    <<                  " FROM ENTITYTYPE "
        //    <<                  " WHERE NAME = '" << TA_Base_Core::DataPointEntityData::getStaticType() << "')";

        std::vector<std::string> columnNames;
        columnNames.push_back(LEVEL_COLUMN);

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it,
        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);
        //std::auto_ptr<TA_Base_Core::IData> data(database->executeQuery(sql.str(), columnNames));
        
		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, ENTITY_V_SELECT_2530,
			  stationMask, stationMask, stationMask, TA_Base_Core::DataPointEntityData::getStaticType());
		std::auto_ptr<TA_Base_Core::IData> data(database->executeQuery(strSql, columnNames));
      

        if (data->getNumRows() == 0)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "There are no TIS-enabled stations");
        }

        // Transfer the retrieved data to the station level container.
        for (unsigned long i(0); i < data->getNumRows(); i++)
        {
            std::string stationLevel(data->getStringData(i, LEVEL_COLUMN));

            if (!stationLevel.empty())
            {
                levels.insert(stationLevel);
            }
        }

        FUNCTION_EXIT;
        return levels;
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream error;
        error << "Failed to retrieve TIS station levels from database."
              << " (" << ex.what() << ")";

        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", error.str().c_str());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Failed to retrieve TIS station levels from database.");
    }

    levels.clear();

    FUNCTION_EXIT;
    return levels;
}


TisConfigAccess::TisPidSet TisConfigAccess::getSTisPids(const std::string& stationName)
{
    FUNCTION_ENTRY( "getSTisPids" );

    TisPidSet pids;
    TA_Base_Core::IData* data = NULL;

    try
    {
        // This SQL query will return the TIS PIDs configured for the specified station.
        // If no station name is specifed, then the PIDs for ALL stations are returned.
        const std::string NAME_COLUMN("NAME");

        std::string stationMask(stationName.empty()? "%" : stationName);

        //std::ostringstream sql;
        //sql << " SELECT UNIQUE (SUBSTR(NAME, 1, INSTR(NAME, '.', 1, 1)) || (SUBSTR(NAME, INSTR(NAME, '.', 1, 2) + 1, INSTR(NAME, '.', -1, 1)-INSTR(NAME, '.', 1, 2) -1 ))) AS " << NAME_COLUMN
        //    << " FROM ENTITY_V "
        //    << " WHERE NAME LIKE '" << stationMask << ".TIS.%.%.%' "
        //    << "   AND NAME NOT LIKE '" << stationMask << ".TIS.STIS.%.%' "
        //    << "   AND NAME NOT LIKE '" << stationMask << ".TIS.ISCS.%.%' "
        //    << "   AND TYPEKEY = (SELECT PKEY "
        //    <<                  " FROM ENTITYTYPE "
        //    <<                  " WHERE NAME = '" << TA_Base_Core::DataPointEntityData::getStaticType() << "')";

        std::vector<std::string> columnNames;
        columnNames.push_back(NAME_COLUMN);     
        
        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it,
        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);
        //data = database->executeQuery(sql.str(), columnNames);
        
		//waitforcheck delete by lin
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, ENTITY_V_SELECT_2531,
			 stationMask, stationMask, stationMask, TA_Base_Core::DataPointEntityData::getStaticType());
		
		data = database->executeQuery(strSql, columnNames);


        if ( 0 == data->getNumRows() )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "There are no STIS PIDs");
        }

        // Transfer the retrieved data to the PID container.
        do
        {
            for (unsigned long i(0); i < data->getNumRows(); i++)
            {
                std::string pid(data->getStringData(i, NAME_COLUMN));

                if (!pid.empty())
                {
                    pids.insert(pid);
                }
            }

            // Delete the data object before getting more.
            delete data;
            data = NULL;
        }
        while (database->moreData(data));

        FUNCTION_EXIT;
        return pids;
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        std::ostringstream error;
        error << "Failed to retrieve STIS PIDs from database."
              << " (" << ex.what() << ")";

        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", error.str().c_str());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Failed to retrieve STIS PIDs from database.");
    }

    delete data;
    data = NULL;

    pids.clear();

    FUNCTION_EXIT;
    return pids;
}


