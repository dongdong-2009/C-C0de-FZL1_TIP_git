/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaDvaMessageAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaDvaMessageAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa/src/PaDvaMessageAccessFactory.h"


#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/pa/src/PaDvaMessage.h"
#include "core/data_access_interface/pa/src/ConfigPaDvaMessage.h"
#include "core/data_access_interface/pa/src/PaTisMessageMap.h"
#include "core/data_access_interface/pa/src/ConfigPaTisMessageMap.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaDvaMessageAccessFactory* PaDvaMessageAccessFactory::m_instance = 0;

    static const std::string PA_STATION_DVA_MESSAGE_TABLE_NAME	= "PA_DVA_MESSAGE";
	
	static const std::string DVA_MSG_TABLE_KEY_COL				= "PA_DVA_MESSAGE.PADMES_ID";
	static const std::string DVA_MSG_TABLE_LOCATIONKEY_COL		= "PA_DVA_MESSAGE.LOCATIONKEY";
    static const std::string DVA_MSG_TABLE_ID_COL				= "PA_DVA_MESSAGE.ID";
	static const std::string DVA_MSG_TABLE_LABEL_COL			= "PA_DVA_MESSAGE.LABEL";
	static const std::string DVA_MSG_TABLE_DESCRIPTION_COL		= "PA_DVA_MESSAGE.MSG_DESCRIPTION";
	static const std::string DVA_MSG_TABLE_TYPE_COL				= "PA_DVA_MESSAGE.TYPE";
	
    static const std::string PA_DVA_MESSAGE_SEQUENCE_COUNTER	= "PADMES_SEQ";
	
    static const std::string DVA_MSG_TABLE_DATEMODIFIED_COL		= "PA_DVA_MESSAGE.DATE_MODIFIED";
    static const std::string DVA_MSG_TABLE_DATECREATED_COL		= "PA_DVA_MESSAGE.DATE_CREATED";


    PaDvaMessageAccessFactory& PaDvaMessageAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaDvaMessageAccessFactory();
        }
        return *m_instance;
    }


    void PaDvaMessageAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaDvaMessage* PaDvaMessageAccessFactory::getPaDvaMessage(const unsigned long key,const bool readWrite)
    {
//        // get a connection to the database
//        TA_Base_Core::IDatabase* databaseConnection = 
//                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//
//        // create the SQL string to retrieve the name of the PaDvaMessage
//        // this is a check to ensure an PaDvaMessage with the specified
//        // PAZONE_ID actually exists.        
//       /* std::ostringstream sql;
//        sql << "select " 
//			<< DVA_MSG_TABLE_KEY_COL << ", "
//			<< DVA_MSG_TABLE_LOCATIONKEY_COL << ", " 
//			<< DVA_MSG_TABLE_ID_COL << ", "
//			<< DVA_MSG_TABLE_LABEL_COL << ", "
//            << DVA_MSG_TABLE_TYPE_COL << ", "
//            << "TO_CHAR(" << DVA_MSG_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), " 
//            << "TO_CHAR(" << DVA_MSG_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')"
//            << " from "
//			<< PA_STATION_DVA_MESSAGE_TABLE_NAME
//            << " where "
//			<< DVA_MSG_TABLE_KEY_COL << " = " << key << std::endl;*/
////		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_TIS_MESSAGE_MAP_Oracle_SELECT_41051, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_TIS_MESSAGE_MAP_SELECT_41051, key);
//		
//        // Set up the columnNames vector to be passed to executeQuery()       
//        std::vector<std::string> columnNames;
//        columnNames.push_back(DVA_MSG_TABLE_KEY_COL);
//        columnNames.push_back(DVA_MSG_TABLE_LOCATIONKEY_COL);
//        columnNames.push_back(DVA_MSG_TABLE_ID_COL);
//        columnNames.push_back(DVA_MSG_TABLE_LABEL_COL);
//        columnNames.push_back(DVA_MSG_TABLE_TYPE_COL);
//        columnNames.push_back(DVA_MSG_TABLE_DATECREATED_COL);
//        columnNames.push_back(DVA_MSG_TABLE_DATEMODIFIED_COL);
//
//        // Execute the query. The method can throw a DatabaseException.
//        // This is documented in the comment of this method.
//        // We are responsible for deleting the returned IData object when we're done with it
//        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
//        
//        // Bring the DataException into the namespace
//        // using TA_Base_Core::DataException;
//
//        if (0 == data->getNumRows()) // No entry found with the specified pkey
//        {
//            // clean up the pointer
//            delete data;
//            data = NULL;
//
//            char reasonMessage[256];
//			sprintf(reasonMessage, "No data found for pkey = %lu",key);			
//                TA_THROW(TA_Base_Core::DataException(reasonMessage,DataException::NO_VALUE,""));
//        }
//        else if (1 < data->getNumRows()) // More than one entry found for the pkey
//        {
//            // clean up the pointer
//            delete data;
//            data = NULL;
//
//            char reasonMessage[256];
//			sprintf(reasonMessage, "More than one entry found for pkey = %lu",key);			
//                TA_THROW(TA_Base_Core::DataException(reasonMessage,DataException::NOT_UNIQUE,""));
//        }
//
//        // Don't need to do anything with the data - the check was only that a single item was returned.
//        // Now that we're finished with the IData object, we need to delete it.
//
//        // Create the PaDvaMessage object to represent this object. Initially only populate it
//        // with id. We will populate the rest of the data on demand.
//        IPaDvaMessage* thePaDvaMessage = NULL;
//        
//        // decide which PaDvaMessage object to return
//        if (readWrite) // need a config PaDvaMessage
//        {
//            thePaDvaMessage = new ConfigPaDvaMessage(0, *data);
//        }
//        else // need a standard PaDvaMessage
//        {
//            thePaDvaMessage = new PaDvaMessage(0, *data);
//        }
//
//        delete data;
//        data = NULL;
//
//        // Return the PaDvaMessage pointer. The class that recieves this pointer is responsible
//        // for deleting it.
//        return thePaDvaMessage;

        std::ostringstream sql;
        sql << DVA_MSG_TABLE_KEY_COL << " = " << key;
        
        IPaDvaMessages dvaMessages = getPaDvaMessagesReturnedFromStatement(sql.str(), readWrite);
        if ( dvaMessages.size() > 0)
        {
            return dvaMessages[0];
        }
        else
        {
            return NULL;
        }
    }


    IPaDvaMessages PaDvaMessageAccessFactory::getAllPaDvaMessages(const bool readWrite )
    {
        return getPaDvaMessagesReturnedFromStatement("", readWrite); 
    }  


    IPaDvaMessages PaDvaMessageAccessFactory::getPaDvaMessagesByLocationKey(unsigned long locationKey, const bool readWrite )
    {
        std::ostringstream sql;
        sql << DVA_MSG_TABLE_LOCATIONKEY_COL << " = " << locationKey;

        return getPaDvaMessagesReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaDvaMessages PaDvaMessageAccessFactory::getPaDvaMessagesReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
		// get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
		
        // create the SQL string to retrieve the data from the pa_station_dva_message and the
        // pa_tis_message_map tables based upon the primary key of a PaDvaMessage record        
        std::ostringstream sql;        
      /*  sql << "select " 
			<< DVA_MSG_TABLE_KEY_COL << ", "
			<< DVA_MSG_TABLE_LOCATIONKEY_COL << ", " 
			<< DVA_MSG_TABLE_ID_COL << ", "
			<< DVA_MSG_TABLE_LABEL_COL << ", "
			<< DVA_MSG_TABLE_DESCRIPTION_COL << ", "
			<< DVA_MSG_TABLE_TYPE_COL << ", "
            << "TO_CHAR(" << DVA_MSG_TABLE_DATECREATED_COL << ", 'YYYYMMDDHH24MISS'), " 
            << "TO_CHAR(" << DVA_MSG_TABLE_DATEMODIFIED_COL << ", 'YYYYMMDDHH24MISS')"
            << " from "  
            << PA_STATION_DVA_MESSAGE_TABLE_NAME; */
		if( "" != whereClause )
		{
			sql << " where " << whereClause;
		}
		//sql << std::endl;
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_TIS_MESSAGE_MAP_Oracle_SELECT_41053, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_DVA_MESSAGE_SELECT_41553, sql.str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_DVA_MESSAGE_SELECT_41553, sql.str());
        // Set up the columnNames vector to be passed to executeQuery()       
        std::vector<std::string> columnNames;
        columnNames.push_back(DVA_MSG_TABLE_KEY_COL);
        columnNames.push_back(DVA_MSG_TABLE_LOCATIONKEY_COL);
        columnNames.push_back(DVA_MSG_TABLE_ID_COL);
		columnNames.push_back(DVA_MSG_TABLE_LABEL_COL);
		columnNames.push_back(DVA_MSG_TABLE_DESCRIPTION_COL);
        columnNames.push_back(DVA_MSG_TABLE_TYPE_COL);
		columnNames.push_back(DVA_MSG_TABLE_DATECREATED_COL);
		columnNames.push_back(DVA_MSG_TABLE_DATEMODIFIED_COL);
		
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
		
		// create the vector of pointers to ITrackElement
        IPaDvaMessages returnValue;
		
        do
        {
            unsigned long rowCount = data->getNumRows();
			
            for (unsigned long i = 0; i < rowCount; ++i)
            {
                if (readWrite == true)
                {
                    returnValue.push_back(new ConfigPaDvaMessage(i, *data));
                }
                else
                {
                    returnValue.push_back(new PaDvaMessage(i, *data));
                }
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);


		return returnValue;
		


//        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
//                    <IPaDvaMessages, ConfigPaDvaMessage, PaDvaMessage>
//                    (PaDvaMessageHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaDvaMessage* PaDvaMessageAccessFactory::createPaDvaMessage()
    {            
		//TA_ASSERT(false, "this function should be call when use distribute DB");
		LOG_GENERIC( SourceInfo,DebugUtil::DebugError, "The createPaDvaMessage function should not be called");
        return new ConfigPaDvaMessage();    
    }


    IConfigPaDvaMessage* PaDvaMessageAccessFactory::copyPaDvaMessage(const IConfigPaDvaMessage* paDvaMessageToCopy)
    {
		//TA_ASSERT(false, "this function should be call when use distribute DB");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "The copyPaDvaMessage function should not be called");


//        FUNCTION_ENTRY("copyPaDvaMessage");
//
//        TA_ASSERT(paDvaMessageToCopy !=  NULL, "The PaDvaMessage to copy was NULL");
//        
//        const ConfigPaDvaMessage* cast = dynamic_cast<const ConfigPaDvaMessage*>(paDvaMessageToCopy);
//
//        TA_ASSERT(cast != NULL, "PaDvaMessage passed could not be converted into a ConfigPaDvaMessage");
//
//        FUNCTION_EXIT;
//        return new ConfigPaDvaMessage(*cast);
		return new ConfigPaDvaMessage;

    }


} // closes TA_Base_Core


