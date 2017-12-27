 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAGroupMembers.cpp $
  * @author Glen Kidd
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * RAGroupMembers is an implementation of IRAGroupMembers. It holds the data specific to an RAGroupMembers entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/radio/src/RAGroupMembers.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/uuid/src/TAuuid.h"

#error "Not implemented"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    RAGroupMembers::RAGroupMembers(const unsigned long key)
        : m_key(key)
    {
    }

    RAGroupMembers::~RAGroupMembers() 
	{
    }

    std::string RAGroupMembers::getSubscriberName()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_subscriberName == "" )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_subscriberName;
    }


    std::string RAGroupMembers::getLocation()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_locationName == "" )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_locationName;
    }

#if 0
    IRAGroupMembers::SubscriberType RAGroupMembers::getSubscriberType()
	{
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_subscriberType == IRAGroupMembers::SubscriberType::INVALID)
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_subscriberType;
    }
#endif

    unsigned long RAGroupMembers::getLocationKey()
	{
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_locationKey == ULONG_MAX)
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_locationKey;
    }
	
	


    void RAGroupMembers::invalidate()
    {
        m_isValidData = false;  
    }

    void RAGroupMembers::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );
#if 1
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        /*char sql[256];
		sprintf(sql,"select RASUBS_ID, SUBSCRIBER_NAME, SUBSCRIBER_TYPE, LOCATION, LOCATION_KEY from RA_SUBSCRIBERS where RASUBS_ID = '%ld'",
			m_key);*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_STD_SELECT_34501, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34501, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34501, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string colRASUBS_ID("RASUBS_ID");
        std::string colSUBSCRIBER_NAME("SUBSCRIBER_NAME");
        std::string colSUBSCRIBER_TYPE("SUBSCRIBER_TYPE");
        std::string colLOCATION("LOCATION");
        std::string colLOCATION_KEY("LOCATION_KEY");

        std::vector<std::string> columnNames;
		columnNames.push_back(colRASUBS_ID);
		columnNames.push_back(colSUBSCRIBER_NAME);
		columnNames.push_back(colSUBSCRIBER_TYPE);
        columnNames.push_back(colLOCATION);
        columnNames.push_back(colLOCATION_KEY);

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
			sprintf(reasonMessage, "No session found for pkey = %ld", m_key );			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data; data = 0;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one session found with pkey = %ld", m_key );
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key            = data->getUnsignedLongData(0,colRASUBS_ID);
        m_subscriberName = data->getStringData(0,colSUBSCRIBER_NAME);
//        m_subscriberType = static_cast<SubscriberType>(data->getIntegerData(0,colSUBSCRIBER_TYPE)); // check
        m_locationName   = data->getStringData(0,colLOCATION);
        m_locationKey    = data->getUnsignedLongData(0,colLOCATION_KEY);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data; data = 0;
        
        // Need to record that we now have valid data
        m_isValidData = true;
		m_isWritten = true;
#endif
        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }

} // closes TA_Base_Core
