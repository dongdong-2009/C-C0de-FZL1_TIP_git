 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASubscribers.cpp $
  * @author Glen Kidd
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * RASubscribers is an implementation of IRASubscribers. It holds the data specific to an RASubscribers entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/radio/src/RASubscribers.h"
#include "core/data_access_interface/radio/src/RASubscribersAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    RASubscribers::RASubscribers(const std::string& TSI)
        : m_rasubsId(TSI),
        m_subscriberName(""),
        m_locationName(""),
        m_subscriberType(IRASubscribers::INVALID),
        m_locationKey(0),
        m_isValidData( false ), // forces reload
        m_isWritten( true )
    {
    }


    RASubscribers::RASubscribers(
        const std::string& groupTSI,
        const std::string& subsName,
        SubscriberType subsType,
        const std::string& location,
        unsigned long locationKey,
        bool isNew
        )
        : m_rasubsId(groupTSI),
        m_subscriberName(subsName),
        m_subscriberType(subsType),
        m_locationName(location),
        m_locationKey(locationKey),
        m_isValidData( true && !isNew ), // assuming that if !new (ie. loaded from db, it is valid)
        m_isWritten( !isNew )
    {
    }


    RASubscribers::~RASubscribers() 
	{
    }

    std::string RASubscribers::getTSI(bool reloadFlag) // key
    {
        TA_ASSERT(m_rasubsId != "", "The TSI is not set");

        // If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_rasubsId == "" )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( reloadFlag && m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_rasubsId;
    }


    std::string RASubscribers::getSubscriberName()
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


    std::string RASubscribers::getLocation()
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


    IRASubscribers::SubscriberType RASubscribers::getSubscriberType()
	{
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_subscriberType == IRASubscribers::INVALID)
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_subscriberType;
    }


    unsigned long RASubscribers::getLocationKey()
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
	

    IRASubscribers& RASubscribers::setTSI(const std::string& TSI)
    {
        if (!m_isValidData && std::string("") != m_rasubsId)
        {
            reload();
        }
        m_rasubsId = TSI;
        return *this;
    }


    IRASubscribers& RASubscribers::setSubscriberName(const std::string& name)
    {
        if (!m_isValidData && std::string("") != m_rasubsId)
        {
            reload();
        }
        m_subscriberName = name;
        return *this;
    }

    
    IRASubscribers& RASubscribers::setLocation(const std::string& location)
    {
        if (!m_isValidData && std::string("") != m_rasubsId)
        {
            reload();
        }
        m_locationName = location;
        return *this;
    }

    
    IRASubscribers& RASubscribers::setSubscriberType(const SubscriberType& type)
    {
        if (!m_isValidData && std::string("") != m_rasubsId)
        {
            reload();
        }
        m_subscriberType = type;
        return *this;
    }

    
    IRASubscribers& RASubscribers::setLocationKey(const unsigned long& locationKey)
    {
        if (!m_isValidData && std::string("") != m_rasubsId)
        {
            reload();
        }
        m_locationKey = locationKey;
        return *this;
    }

    
    void RASubscribers::invalidate()
    {
        m_isValidData = false;  
    }

	//TD18317, zhanghongzhi
	std::string RASubscribers::getLocalDatabaseName()
    {
		std::string localDatabaseName;    
		if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw DatabaseException("db-connection-file not set");
		}
		try
		{
			std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			localDatabaseName = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Radio_Cd, Write);
			/*
			DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			DataConnections connections = connectionStrings.getConnectionList(Radio_Cd, Write);
			localDatabaseName = connections[0].first;  */
		}
		catch(...) // most likely a file not found error
		{
			throw DatabaseException("Unable to find database connection infomation");
		}
		return localDatabaseName;
	}

    void RASubscribers::applyChanges()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "RASubscribers::applyChanges" );     
		TA_ASSERT( !m_rasubsId.empty(), "TSI not set, can not apply" );

		// NOTE: Sessions can only be created or destroyed - nothing else. 
		// There is no need to handle updating records, only creating new records

		// Ensure the keys are set correctly, do we need to?
        /*if ( check fields... blah blah ) // Need to throw an exception
        {
			TA_THROW(TA_Base_Core::DataConfigurationException(
				"Operator data not fully specified. Operator cannot be written to database"
				,fieldNames));
        }*/
        
        // This test is wrong !!! during createXXX() we won't have set the flag. 
#if 0
        if (m_isWritten) 
        {
            // delete existing record first
            RASubscribersAccessFactory::getInstance().deleteSubscriber(this);
            m_isWritten = false;
        }
#endif
    

#if 1
        // Do existing record check the hard way...
        //TD18083
		if(m_subscriberType == PATCH_GROUP)
		{
			IDatabase* databaseConnection1 = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);
		/*	std::stringstream s1;
			s1  << "select SUBSCRIBER_NAME from RA_SUBSCRIBERS "
				<< " where SUBSCRIBER_NAME = '" << databaseConnection1->escapeQueryString(m_subscriberName) << "'"
				<< "and SUBSCRIBER_TYPE = 'P'"
				<< std::ends;
			std::string sql1(s1.str());*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection1->GetDbServerType(), RA_SUBSCRIBERS_STD_SELECT_34502,
//			std::string strSql  = databaseConnection1->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34502,
		SQLStatement strSql;
		databaseConnection1->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34502,
				databaseConnection1->escapeQueryString(m_subscriberName));
			// Set up the columnNames vector to be passed to executeQuery()
			std::string colSUBSCRIBER_NAME1("SUBSCRIBER_NAME");
			std::vector<std::string> columnNames1;
			columnNames1.push_back(colSUBSCRIBER_NAME1);
			IData* data1 = databaseConnection1->executeQuery(strSql,columnNames1);
			if (data1->getNumRows() >=1) // more than one entry found for the name - record already exist, should ignore the applychanges
			{
				return;
			}
		}		
		// TD18083 end

        try 
        {
            {
                RASubscribers checkExisting(m_rasubsId);
                checkExisting.getTSI(); // force load
            }
            // delete only called if record found (no data exception causes delete to be skipped)
            RASubscribersAccessFactory::getInstance().deleteSubscriber(
                new RASubscribers(m_rasubsId)); // use temp obj 'cause we need ourselves
        }
        catch (DataException& e)
        {
            if (e.getFailType() != DataException::NO_VALUE) 
            {
                throw e;
            }
        }
#endif

        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Write);


        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Writing RASubscriber (id: %s) to the database.",
            m_rasubsId.c_str());
        
	 
        std::stringstream s;
        s   
#if 0 // disable pl/sql for now
            << " declare "
            << " 	   cursor x is select * from ra_subscribers where rasubs_id = '"
            << m_rasubsId << "' for update; "
            << " 	   y ra_subscribers%rowtype; "
            << " begin "
            << " 	 open x; "
            << " 	 fetch x into y; "
            << " 	 if x%FOUND then "
            << " 	 	delete from ra_subscribers where current of x; "
            << " 	 end if; "
#endif		//TD18317, zhanghongzhi
			<< "BEGIN audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
            << "insert into RA_SUBSCRIBERS (RASUBS_ID, SUBSCRIBER_NAME, SUBSCRIBER_TYPE, LOCATION, LOCATION_KEY) "
            << " values ( "
            << " ''" << m_rasubsId << "'', "
            << " ''" << databaseConnection->escapeInsertString(m_subscriberName) << "'', "
            << " ''" << static_cast<char>(m_subscriberType) << "'', "
            << " ''" << databaseConnection->escapeInsertString(m_locationName) << "'', "
            << " ''" << m_locationKey << "'' "
            << " )" 
			<< "'); END;"
#if 0
            << " end; "
#endif
            << std::ends;
        std::string sql(s.str());

//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_INSERT_34853,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_INSERT_34853,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_INSERT_34853,
			getLocalDatabaseName(), m_rasubsId, databaseConnection->escapeInsertString(m_subscriberName), static_cast<char>(m_subscriberType),
			databaseConnection->escapeInsertString(m_locationName), m_locationKey);

        // This will throw an exception upon failure - ready to catch??
        databaseConnection->executeModification(strSql);
//        databaseConnection->executeProcedure(sql);

		// Set the data to be valid - no need to force a reload after a succesful write
		m_isValidData = true;
		m_isWritten = true;

    }


    void RASubscribers::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
       /* std::stringstream s;
        s   << "select RASUBS_ID, SUBSCRIBER_NAME, SUBSCRIBER_TYPE, LOCATION, nvl(LOCATION_KEY,0) as LOCATION_KEY "
            << " from RA_SUBSCRIBERS "
            << " where RASUBS_ID = '" << databaseConnection->escapeQueryString(m_rasubsId) << "'"
            << std::ends;
        std::string sql(s.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SUBSCRIBERS_Oracle_SELECT_34555,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SUBSCRIBERS_SELECT_34555,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SUBSCRIBERS_SELECT_34555,
			databaseConnection->escapeQueryString(m_rasubsId));

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
			sprintf(reasonMessage, "No Subscriber found for TSI = %s", m_rasubsId.c_str() );			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey - SHOULD NEVER HAPPEN (unique key constraint)
        {
            delete data; data = 0;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one Subscriber found with TSI = %s", m_rasubsId.c_str() );
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_rasubsId       = data->getStringData(0,colRASUBS_ID);
        m_subscriberName = data->getStringData(0,colSUBSCRIBER_NAME);
        m_subscriberType = static_cast<IRASubscribers::SubscriberType>(data->getStringData(0,colSUBSCRIBER_TYPE)[0]); // check
        m_locationName   = data->getStringData(0,colLOCATION);
        m_locationKey    = data->getUnsignedLongData(0,colLOCATION_KEY);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data; data = 0;
        
        // Need to record that we now have valid data
        m_isValidData = true;
		m_isWritten = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }

} // closes TA_Base_Core
