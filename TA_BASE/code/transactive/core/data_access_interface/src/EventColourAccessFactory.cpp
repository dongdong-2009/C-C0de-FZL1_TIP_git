/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EventColourAccessFactory.cpp $
 * @author:  Derrick Liew
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * EventColourAccessFactory is a singleton that is used to retrieve EventColour objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

// EventColourAccessFactory.cpp: implementation of the EventColourAccessFactory class.
//
//////////////////////////////////////////////////////////////////////
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "EventColourAccessFactory.h"
#include "core/data_access_interface/src/EventColourAccessFactory.h"
#include "core/data_access_interface/src/EventColourData.h"
#include "core/data_access_interface/src/ConfigEventColourData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    EventColourAccessFactory* EventColourAccessFactory::m_instance = 0;


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	EventColourAccessFactory::~EventColourAccessFactory()
	{
	}

    EventColourAccessFactory& EventColourAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new EventColourAccessFactory();
        }
        return *m_instance;
    }

    void EventColourAccessFactory::getEventColourBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector< IEventColourData* >& eventColours, const bool readWrite)
    {
        // get a connection to the database
        //IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		TA_ASSERT(NULL != pDatabase, "database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("PKEY");
		columnNames.push_back("NAME");
        columnNames.push_back("OPEN_ACKED_FG_COLOUR1");
        columnNames.push_back("OPEN_ACKED_BG_COLOUR1");
        columnNames.push_back("OPEN_UNACKED_FG_COLOUR1");
        columnNames.push_back("OPEN_UNACKED_BG_COLOUR1");
        columnNames.push_back("CLOSED_UNACKED_FG_COLOUR1");
        columnNames.push_back("CLOSED_UNACKED_BG_COLOUR1");
        columnNames.push_back("CLOSED_ACKED_FG_COLOUR1");
        columnNames.push_back("CLOSED_ACKED_BG_COLOUR1");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql,columnNames);

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
						eventColours.push_back( new ConfigEventColourData( i, *data ) );
                    }
                    else
                    {
						eventColours.push_back( new EventColourData( i, *data ) );
                    }
                }
                catch (DataException&)
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
        while ( pDatabase->moreData(data) );
    }
	

    IEventColourData* EventColourAccessFactory::getEventColourByKey(const unsigned long key,const bool readWrite)
    {
		FUNCTION_ENTRY("getEventColourByKey");

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENTCOLOUR_Oracle_SELECT_15551, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENTCOLOUR_SELECT_15551, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENTCOLOUR_SELECT_15551, key);
//		TA_ASSERT(strSql.size() > 0, "EventColourAccessFactory::getEventColourByKey(): Prepare SQL Statement error");

		// create the SQL string to retrieve the data of the Severity
       /* std::ostringstream sql;
        sql << " select PKEY, NAME,"
			<< " OPEN_ACKED_FG_COLOUR1, OPEN_ACKED_BG_COLOUR1,"
			<< " OPEN_UNACKED_FG_COLOUR1, OPEN_UNACKED_BG_COLOUR1,"
			<< " CLOSED_UNACKED_FG_COLOUR1, CLOSED_UNACKED_BG_COLOUR1,"
			<< " CLOSED_ACKED_FG_COLOUR1, CLOSED_ACKED_BG_COLOUR1,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from EVENTCOLOUR where PKEY = " << key;*/

		std::vector< IEventColourData* > eventColours;
		getEventColourBySql(databaseConnection, strSql, eventColours, readWrite);
		
        if ( 0 == eventColours.size())
        {
            std::ostringstream message;
			message << "No data found for EventColour with key " << key;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );
            
			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );
            
        }

		TA_ASSERT(1 == eventColours.size(), "EventColour key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return eventColours[0];
    }

    std::vector<IEventColourData*> EventColourAccessFactory::getAllEventColours(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllEventColours");

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENTCOLOUR_Oracle_SELECT_15552);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENTCOLOUR_SELECT_15552);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENTCOLOUR_SELECT_15552);
//		TA_ASSERT(strSql.size() > 0, "EventColourAccessFactory::getAllEventColours(): Prepare SQL Statement error");

		// create the SQL string to retrieve the data of the Severity
        /*std::ostringstream sql;
        sql << " select PKEY, NAME,"
			<< " OPEN_ACKED_FG_COLOUR1, OPEN_ACKED_BG_COLOUR1,"
			<< " OPEN_UNACKED_FG_COLOUR1, OPEN_UNACKED_BG_COLOUR1,"
			<< " CLOSED_UNACKED_FG_COLOUR1, CLOSED_UNACKED_BG_COLOUR1,"
			<< " CLOSED_ACKED_FG_COLOUR1, CLOSED_ACKED_BG_COLOUR1,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from EVENTCOLOUR order by PKEY";*/

		std::vector< IEventColourData* > eventColours;
		getEventColourBySql(databaseConnection, strSql, eventColours, readWrite);

		// Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return eventColours;
    }

} //close namespace
