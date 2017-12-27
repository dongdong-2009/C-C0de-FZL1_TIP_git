/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ConsoleAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * ConsoleAccessFactory is a singleton that is used to retrieve Console objects either from the
  * database or newly created. All Console objects returned will adhear to the IConsole interface.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/Console.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{

    const std::string ConsoleAccessFactory::KEY      = "PKey";
    const std::string ConsoleAccessFactory::CONSOLEKEY = "ConsoleKey";

    ConsoleAccessFactory* ConsoleAccessFactory::m_instance = 0;

    ConsoleAccessFactory::ConsoleAccessFactory()
    {
    }

    ConsoleAccessFactory& ConsoleAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ConsoleAccessFactory();
        }
        return *m_instance;
    }

    IConsole* ConsoleAccessFactory::getConsole(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the name of the Console
        // this also doubles as a check to ensure a Console with the specified
        // PKEY actually exists.
        /*char sql[256];
		sprintf(sql,
			"SELECT e.pkey FROM entity_v e, entitytype t WHERE e.pkey = %lu AND e.typekey = t.pkey AND t.name = '%s' and e.deleted = 0",
			key,
			Console::getStaticType().c_str() );	*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			Entitytype_STD_SELECT_4012, key, Console::getStaticType());
//		std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4012, key, Console::getStaticType());
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4012, key, Console::getStaticType());
//		TA_ASSERT(sql.size() > 0, "getConsole: Get SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);
        
        // Bring the DataException into the namespace
            using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No data found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one entry found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Retrieve the key from the returned data.
        // There should only be a single returned item (as any other circumstance is 
        // specifically prevented by the above checks). We don't need to check what it is - 
        // the test was only that it existed.
        // Commented out, as not needed
        // unsigned long returnedKey = data->getUnsignedLongData(0,KEY);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the Console object to represent this object. Initially only populate it
        // with key and name. We will populate the rest of the data on demand.
        IConsole* theConsole = dynamic_cast<IConsole*>( Console::clone( key ) );

        // Return the Console pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theConsole;
    }


    IConsole* ConsoleAccessFactory::getConsole(const std::string& name,const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the name of the Console
        // this also doubles as a check to ensure an Console with the specified
        // PKEY actually exists.
        /*char sql[512];
		sprintf(sql,"SELECT e.pkey FROM entity_v e, entitytype t WHERE e.name = '%s' AND e.typekey = t.pkey AND t.name = '%s' and e.deleted = 0",
            (databaseConnection->escapeQueryString(name)).c_str(),
			Console::getStaticType().c_str());*/	
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4013,
//		std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4013,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4013,
			databaseConnection->escapeQueryString(name), Console::getStaticType());
//		TA_ASSERT(sql.size() > 0, "getConsole: Get SQL Statement error");


        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        // Need to bring in the DataException
            using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No data found for name = %s",name.c_str());			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one entry found for name = %s",name.c_str());			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Retrieve the pkey as an unsigned long - the name of this Console - from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        unsigned long key = data->getUnsignedLongData(0,KEY);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the Console object to represent this object. Initially only populate it
        // with key. We will populate the rest of the data on demand.
        IConsole* theConsole = dynamic_cast<IConsole*>( Console::clone( key ) );

        // Return the Console pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theConsole;
    }

    IConsole* ConsoleAccessFactory::getConsoleFromSession(const std::string& sessionId,
        const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the name of the Console
        // this also doubles as a check to ensure an Console with the specified
        // PKEY actually exists.
        /*char sql[512];
		sprintf(sql,"SELECT consolekey FROM ta_session WHERE pkey = '%s'",databaseConnection->escapeQueryString(sessionId).c_str());*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			ta_session_STD_SELECT_11001, databaseConnection->escapeQueryString(sessionId));
//		std::string sql  = databaseConnection->prepareSQLStatement(TA_SESSION_SELECT_11001, databaseConnection->escapeQueryString(sessionId));
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TA_SESSION_SELECT_11001, databaseConnection->escapeQueryString(sessionId));
//		TA_ASSERT(sql.size() > 0, "getConsoleFromSession: Get SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(CONSOLEKEY);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        // Need to bring in the DataException
            using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No data found for PMSESSION key = %s",sessionId.c_str());			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one entry found for TA_Session string = %s",
                sessionId.c_str());			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        unsigned long key = data->getUnsignedLongData(0,CONSOLEKEY);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Use getConsole to create the Console object based upon the key we just pulled
        // out of the database.
        return getConsole(key,readWrite);
    }

    IConsole* ConsoleAccessFactory::getConsoleFromAddress(const std::string& address, const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the name of the Console
        // this also doubles as a check to ensure an Console with the specified
        // PKEY actually exists.
        /*std::ostringstream sql;
        sql << "SELECT e.pkey FROM entity_v e, entitytype t WHERE lower(e.address) = lower('"
            << (databaseConnection->escapeQueryString(address)).c_str()
            << "') AND e.typekey = t.pkey AND t.name = '"
            << Console::getStaticType().c_str() << "' and e.deleted = 0";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4014,
//		std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4014,
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4014,
			databaseConnection->escapeQueryString(address),Console::getStaticType());
//		TA_ASSERT(sql.size() > 0, "getConsoleFromAddress: Get SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql, columnNames);

        // Need to bring in the DataException
            using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No data found for address = %s",address.c_str());			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one entry found for address = %s",address.c_str());			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Retrieve the pkey as an unsigned long - the name of this Console - from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        unsigned long key = data->getUnsignedLongData(0,KEY);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the Console object to represent this object. Initially only populate it
        // with key. We will populate the rest of the data on demand.
        IConsole* theConsole = dynamic_cast<IConsole*>( Console::clone( key ) );

        // Return the Console pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theConsole;
    }


    std::vector<IConsole*> ConsoleAccessFactory::getAllConsoles(const bool readWrite /*= false*/)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve all Consoles
        /*char sql[256];
		sprintf(sql,
			"SELECT e.pkey FROM entity_v e, entitytype t WHERE e.typekey = t.pkey AND t.name = '%s' and e.deleted = 0",
			Console::getStaticType().c_str() );*/	
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			Entitytype_STD_SELECT_4015, Console::getStaticType());
//		std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4015, Console::getStaticType());
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4015, Console::getStaticType());
//		TA_ASSERT(sql.size() > 0, "getAllConsoles: Get SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);
        
        // Bring the DataException into the namespace
            using TA_Base_Core::DataException;

        // the vector containing pointers
        std::vector<IConsole*> consoles;

        do 
        {
            // Retrieve the keys from the returned data.
            for (unsigned int i = 0; i < data->getNumRows(); i++)
            {
                try
                {
                    unsigned long key = data->getUnsignedLongData(i,KEY);
                    consoles.push_back( dynamic_cast<IConsole*>( Console::clone( key ) ) );
                }
                catch (DataException& de)
                {
                    // this means a Pkey could not be converted to an unsigned int
                    LOG_EXCEPTION_CATCH( SourceInfo, "DataException", de.what());
                }
            }

            // Now that we're finished with the IData object, we need to delete it.
            delete data;
            data = NULL;
        }
        while ( databaseConnection->moreData( data ) );

        return consoles;
    }


	IConsole* ConsoleAccessFactory::getConsoleByKey( const unsigned long consoleKey, const bool readWrite /* = false */ )
	{
        // Create the Console object to represent this object. Initially only populate it
        // with key. We will populate the rest of the data on demand.
        IConsole* theConsole = dynamic_cast<IConsole*>( Console::clone( consoleKey ) );
		
        // Return the Console pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theConsole;

	}

} // closes TA_Base_Core


