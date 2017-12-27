 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASpeedDialSetAccessFactory.cpp $
  * @author Glen Kidd
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * RASpeedDialSetAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IRASpeedDialSet interface.
  */
#if 0
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif
#endif

#include <sstream>

#include "core/data_access_interface/radio/src/RASpeedDialSetAccessFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialSet.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    TA_Base_Core::NonReEntrantThreadLockable RASpeedDialSetAccessFactory::m_singletonLock;
	RASpeedDialSetAccessFactory* RASpeedDialSetAccessFactory::s_instance = NULL;


    RASpeedDialSetAccessFactory& RASpeedDialSetAccessFactory::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if (s_instance == NULL)
        {
            s_instance = new RASpeedDialSetAccessFactory;
        }
        return *s_instance;
    }


    RASpeedDialSetAccessFactory::~RASpeedDialSetAccessFactory()
    {

    }


    IRASpeedDialSet* RASpeedDialSetAccessFactory::getRASpeedDialSetDetails(unsigned long key)
    {
        // Construct a new RASpeedDialSet type with the key we have
        IRASpeedDialSet* myRASpeedDialSet = new RASpeedDialSet(key);

        // Force it to load the details so we know if this key is valid or not. If this key is not valid
        // this call will throw an exception
        try
        {
            myRASpeedDialSet->getKey(); 
        }
        catch ( ... )
        {
            delete myRASpeedDialSet;
            throw;
        }

        return myRASpeedDialSet;
    }


    IRASpeedDialSet* RASpeedDialSetAccessFactory::createSpeedDialSet(IRASpeedDialSet * x)
    {
        // dummy method to conform to AF usage, RASpeedDialSet(...) constructor more 
        // than effective at creating new object, no need to wrap here. 
        // usage: new_subscriber = createSubscriber(new RASubscriber(...))
        return x;
    }


    void RASpeedDialSetAccessFactory::deleteSpeedDialSet(IRASpeedDialSet* x)
    {
        if (x == NULL)
        {
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Write);

       /* std::stringstream s;
        s   << "delete from RA_SPEED_DIAL_SET where RASDSE_ID = "
            << x->getKey(false) << " " << std::ends;
        std::string sql(s.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_SET_STD_DELETE_36901,x->getKey(false));
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_SET_DELETE_36901,x->getKey(false));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_SET_DELETE_36901,x->getKey(false));
        databaseConnection->executeModification(strSql);
    }

    IRASpeedDialSet* RASpeedDialSetAccessFactory::getRASpeedDialSetByProfile(unsigned long profileID)
    {
        // Construct a new RASpeedDialSet type with the key we have
       /* std::stringstream buildSql;
        buildSql << "select RASDSE_ID, IS_SYSTEM, OPERATOR_KEY, SEPROF_ID, SET_NAME from RA_SPEED_DIAL_SET"
            << " where SEPROF_ID = " << profileID;*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::string colKey("RASDSE_ID");
        std::string colIsSystem("IS_SYSTEM");
        std::string colOperatorKey("OPERATOR_KEY");
        std::string colProfileId("SEPROF_ID");
        std::string colSetName("SET_NAME");

        std::vector<std::string> columnNames;
		columnNames.push_back(colKey);
		columnNames.push_back(colIsSystem);
		columnNames.push_back(colOperatorKey);
        columnNames.push_back(colProfileId);
        columnNames.push_back(colSetName);

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_SET_STD_SELECT_36502, profileID);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_SET_SELECT_36502, profileID);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_SET_SELECT_36502, profileID);


        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data; data = 0;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No speed dial set found for profile ID = %ld", profileID );			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data; data = 0;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More speed dial set found for profile ID = %ld", profileID );
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // create the object
        IRASpeedDialSet* myRASpeedDialSet = new RASpeedDialSet(data->getUnsignedLongData(0,colKey), // key
            data->getBooleanData(0,colIsSystem),            // IS_SYSTEM
            data->getUnsignedLongData(0,colOperatorKey),    // OPERATOR_KEY
            profileID,                                      // SEPROF_ID
            data->getStringData(0,colSetName),              // SET_NAME
            true);                                          // isNew flag = always true

        // And return it
        return myRASpeedDialSet;
    }


} //end namespace TA_Base_Core


