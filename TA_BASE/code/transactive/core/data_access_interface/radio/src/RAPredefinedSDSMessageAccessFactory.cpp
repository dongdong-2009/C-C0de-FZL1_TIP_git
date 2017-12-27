 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAPredefinedSDSMessageAccessFactory.cpp $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RAPredefinedSDSMessageAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IRAPredefinedSDSMessage interface.
  */
#if 0
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif
#endif

#include <sstream>

#include "core/data_access_interface/radio/src/RAPredefinedSDSMessageAccessFactory.h"
#include "core/data_access_interface/radio/src/RAPredefinedSDSMessage.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    TA_Base_Core::NonReEntrantThreadLockable RAPredefinedSDSMessageAccessFactory::m_singletonLock;
	RAPredefinedSDSMessageAccessFactory* RAPredefinedSDSMessageAccessFactory::s_instance = NULL;


    RAPredefinedSDSMessageAccessFactory& RAPredefinedSDSMessageAccessFactory::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if (s_instance == NULL)
        {
            s_instance = new RAPredefinedSDSMessageAccessFactory;
        }
        return *s_instance;
    }


    RAPredefinedSDSMessageAccessFactory::~RAPredefinedSDSMessageAccessFactory()
    {

    }


    IRAPredefinedSDSMessage* RAPredefinedSDSMessageAccessFactory::getByShortName(std::string shortName)
    {
        // Construct a new RAPredefinedSDSMessage type with the key we have
        IRAPredefinedSDSMessage* myRAPredefinedSDSMessage = new RAPredefinedSDSMessage(shortName);

        // Force it to load the details so we know if this key is valid or not. If this key is not valid
        // this call will throw an exception
        try
        {
            myRAPredefinedSDSMessage->getMessage(); // was getName() TODO: check this
        }
        catch ( ... )
        {
            delete myRAPredefinedSDSMessage;
            throw;
        }

        return myRAPredefinedSDSMessage;
    }

    
    std::vector<IRAPredefinedSDSMessage*> RAPredefinedSDSMessageAccessFactory::getList(const bool readWrite) const
    {
		// Perform SQL here
		std::vector< IRAPredefinedSDSMessage* > theList;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);

		// create the SQL string 
        /*std::stringstream s;
        s   << "select  "
            << "	   SHORT_NAME, "
            << "	   MESSAGE  "
            << " from  "
            << "	 RA_PREDEFINED_SDS_MESSAGES  "
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_PREDEFINED_SDS_MESSAGES_STD_SELECT_35002);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_PREDEFINED_SDS_MESSAGES_SELECT_35002);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_SDS_MESSAGES_SELECT_35002);
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string shortNameColumn("SHORT_NAME");
		std::string messageColumn("MESSAGE");
        std::vector<std::string> columnNames;
        columnNames.push_back(shortNameColumn);
        columnNames.push_back(messageColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					// Create the subsystem
                    IRAPredefinedSDSMessage* element = new RAPredefinedSDSMessage( 
                        data->getStringData(i, shortNameColumn ),
                        data->getStringData(i, messageColumn ),
                        false); // false indicates not a new session
                    
                    theList.push_back( element );
                }
                catch (const TA_Base_Core::DataException&)
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
        while ( databaseConnection->moreData(data) );

		return theList;

    }


    
    IRAPredefinedSDSMessage* RAPredefinedSDSMessageAccessFactory::createMessage(IRAPredefinedSDSMessage * x)
    {
        // dummy method to conform to AF usage, RAPredefinedSDSMessage(...) constructor more 
        // than effective at creating new object, no need to wrap here. 
        // usage: new_subscriber = createSubscriber(new RASubscriber(...))
        return x;
    }


    void RAPredefinedSDSMessageAccessFactory::deleteMessage(IRAPredefinedSDSMessage* message)
    {
        if (message == NULL)
        {
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Write);

        /*std::stringstream s;
        s   << "delete from RA_PREDEFINED_SDS_MESSAGES where SHORT_NAME = '"
            << databaseConnection->escapeQueryString(message->getShortName(false)) << "' " << std::ends;
        std::string sql(s.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_PREDEFINED_SDS_MESSAGES_STD_DELETE_35401,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_PREDEFINED_SDS_MESSAGES_DELETE_35401,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_SDS_MESSAGES_DELETE_35401,
			databaseConnection->escapeQueryString(message->getShortName(false)));
        databaseConnection->executeModification(strSql);
    }


} //end namespace TA_Base_Core


