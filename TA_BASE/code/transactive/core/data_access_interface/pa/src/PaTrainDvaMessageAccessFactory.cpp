/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaTrainDvaMessageAccessFactory is a singleton that is used to retrieve PaTrainDvaMessage objects either from the
  * database or newly created. All PaTrainDvaMessage objects returned will adhere to the IPaTrainDvaMessage interface.
  */
#pragma warning(disable:4786 4290)

#include <sstream>
#include "core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/PaTrainDvaMessage.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaTrainDvaMessage.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SQLCode.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"


namespace TA_Base_Core
{
    PaTrainDvaMessageAccessFactory* PaTrainDvaMessageAccessFactory::m_instance = 0;

    static const std::string KEY_COL_NAME                       = "PATDME_ID";
    static const std::string PA_TRAIN_DVA_MESSAGE_TABLE_NAME    = "PA_TRAIN_DVA_MESSAGE";
    static const std::string LIBRARY_VERSION_TOKEN              = "PaTrainDvaMessageLibraryVersion";
    static const std::string GLOBAL_PARAMETER_COL_VALUE_NAME    = "VALUE";
    static const std::string PA_SUBSYSTEM_NAME = "PA";

    PaTrainDvaMessageAccessFactory& PaTrainDvaMessageAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaTrainDvaMessageAccessFactory();
        }
        return *m_instance;
    }


    void PaTrainDvaMessageAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }

    unsigned long PaTrainDvaMessageAccessFactory::getLibraryVersion()
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string to retrieve the name of the PaTrainDvaMessage
        // this is a check to ensure an PaTrainDvaMessage with the specified
        // PATDME_ID actually exists.        
       /* std::ostringstream sql;
        sql << "select " << COMMENTS_COL_NAME << " from user_tab_comments where table_name = " 
            << "'" << PA_TRAIN_DVA_MESSAGE_TABLE_NAME << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_TRAIN_DVA_MESSAGE_Oracle_SELECT_44051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_SELECT_44051);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GLOBAL_PARAMETER_SELECT_16003, LIBRARY_VERSION_TOKEN, PA_SUBSYSTEM_NAME);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(GLOBAL_PARAMETER_COL_VALUE_NAME);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // Should never happen

            // clean up the pointer
            delete data;
            data = NULL;

            TA_THROW(DataException("No data found for table comment",DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // Should never happen

            // clean up the pointer
            delete data;
            data = NULL;

            TA_THROW(DataException("No unique data found for table comment",DataException::NO_VALUE,""));
        }

        unsigned long libraryVersion = data->getUnsignedLongData(0, GLOBAL_PARAMETER_COL_VALUE_NAME);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;
   
        return libraryVersion;
    }

    void PaTrainDvaMessageAccessFactory::setLibraryVersion(unsigned long libraryVersion)
    {      
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);


        std::stringstream libVersionString;
        libVersionString << libraryVersion;
        // create the SQL string to retrieve the name of the PaTrainDvaMessage
        // this is a check to ensure an PaTrainDvaMessage with the specified
        // PATDME_ID actually exists.
      /*  std::ostringstream formatSQL;
		formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'PUBLIC', '";
        formatSQL << "comment on table  " << PA_TRAIN_DVA_MESSAGE_TABLE_NAME << " is ''";
		formatSQL << tableComment.str() << "''');";
        formatSQL << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_TRAIN_DVA_MESSAGE_Oracle_UPDATE_44201,
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_UPDATE_44201,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GLOBAL_PARAMETER_UPDATE_16202,
			getLocalDatabaseName(), libVersionString.str(), LIBRARY_VERSION_TOKEN, PA_SUBSYSTEM_NAME);

//        std::ostringstream sql;
//        sql << "comment on table " 
//            << PA_TRAIN_DVA_MESSAGE_TABLE_NAME << " is '"
//            << tableComment.str() << "'";

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        databaseConnection->executeModification(strSql);
    }

    IPaTrainDvaMessage* PaTrainDvaMessageAccessFactory::getPaTrainDvaMessage(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string to retrieve the name of the PaTrainDvaMessage
        // this is a check to ensure an PaTrainDvaMessage with the specified
        // PATDME_ID actually exists.        
      /*  std::ostringstream sql;
        sql << "select " << KEY_COL_NAME 
            << " from " << PA_TRAIN_DVA_MESSAGE_TABLE_NAME
            << " where " << KEY_COL_NAME << " = " << key;  */ 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_TRAIN_DVA_MESSAGE_STD_SELECT_44001, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TRAIN_DVA_MESSAGE_SELECT_44001, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_TRAIN_DVA_MESSAGE_SELECT_44001, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL_NAME);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "No data found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "More than one entry found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the PaTrainDvaMessage object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaTrainDvaMessage* thePaTrainDvaMessage;
        
        // decide which PaTrainDvaMessage object to return
        if (readWrite) // need a config PaTrainDvaMessage
        {
            thePaTrainDvaMessage = new ConfigPaTrainDvaMessage(key);
        }
        else // need a standard PaTrainDvaMessage
        {
            thePaTrainDvaMessage = new PaTrainDvaMessage(key);
        }

        // Return the PaTrainDvaMessage pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaTrainDvaMessage;
    }


    IPaTrainDvaMessages PaTrainDvaMessageAccessFactory::getAllPaTrainDvaMessages( const bool readWrite)
    {
        return getPaTrainDvaMessagesReturnedFromStatement("", readWrite);
    }


    IPaTrainDvaMessages PaTrainDvaMessageAccessFactory::getPaTrainDvaMessagesReturnedFromStatement(const std::string& whereClause, const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaTrainDvaMessages, ConfigPaTrainDvaMessage, PaTrainDvaMessage>
                        (PaTrainDvaMessageHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaTrainDvaMessage* PaTrainDvaMessageAccessFactory::createPaTrainDvaMessage(unsigned long primaryKey)
    {      
        ConfigPaTrainDvaMessage* pResult = 0;

        try
        {
            ConfigPaTrainDvaMessage* pResult = new ConfigPaTrainDvaMessage();    
            
            pResult->setKeyForNewMessage(primaryKey);
            
            return pResult;
        }
        catch (const TA_Base_Core::TransactiveException&)
        {
            delete pResult;
            pResult = 0;
            throw;
        }
    }


    IConfigPaTrainDvaMessage* PaTrainDvaMessageAccessFactory::copyPaTrainDvaMessage(const IConfigPaTrainDvaMessage* paTrainDvaMessageToCopy)
    {
        FUNCTION_ENTRY("copyPaTrainDvaMessage");

        TA_ASSERT(paTrainDvaMessageToCopy !=  NULL, "The PaTrainDvaMessage to copy was NULL");
        
        const ConfigPaTrainDvaMessage* cast = dynamic_cast<const ConfigPaTrainDvaMessage*>(paTrainDvaMessageToCopy);

        TA_ASSERT(cast != NULL, "PaTrainDvaMessage passed could not be converted into a ConfigPaTrainDvaMessage");

        FUNCTION_EXIT;
        return new ConfigPaTrainDvaMessage(*cast);

    }

	std::string PaTrainDvaMessageAccessFactory::getLocalDatabaseName()
    {
		std::string localDatabaseName;
		if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw DatabaseException("db-connection-file not set");
		}
		try
		{
			std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			localDatabaseName = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Pa_Cd, Write);

			/*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			DataConnections connections = connectionStrings.getConnectionList(Pa_Cd, Write);
			localDatabaseName = connections[0].first;  */
		}
		catch(...) // most likely a file not found error
		{
			throw DatabaseException("Unable to find database connection infomation");
		}
	return localDatabaseName;
	}

} // closes TA_Base_Core


