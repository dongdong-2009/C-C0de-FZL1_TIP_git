 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASpeedDialSet.cpp $
  * @author Glen Kidd
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * RASpeedDialSet is an implementation of IRASpeedDialSet. It holds the data specific to an RASpeedDialSet entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/radio/src/RASpeedDialSet.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    RASpeedDialSet::RASpeedDialSet(const unsigned long key)
        : m_key(key),
        m_isSystem(true),
        m_operatorKey(0), // really a null
        m_profileId(0), // really a null
        m_setName(""),
        m_isValidData( false ), // forces reload
        m_isWritten( true )
    {
    }
    

    RASpeedDialSet::RASpeedDialSet(const std::string& name)
        : m_key(ULONG_MAX), // force reload() to get by setName
        m_isSystem(true),
        m_operatorKey(0), // really a null
        m_profileId(0), // really a null
        m_setName(name),
        m_isValidData( false ), // forces reload
        m_isWritten( true )
    {
    }
    

    RASpeedDialSet::RASpeedDialSet(
        const unsigned long key,
        const bool isSystem,
        const unsigned long operatorKey,
        const unsigned long profileId,
        const std::string& setName,
        const bool isNew
        )
        : m_key(key),
        m_isSystem(isSystem),
        m_operatorKey(operatorKey), 
        m_profileId(profileId), 
        m_setName(setName),
        m_isValidData( true && !isNew ), // assuming that if !new (ie. loaded from db, it is valid)
        m_isWritten( !isNew )
    {
    }
    

    RASpeedDialSet::~RASpeedDialSet()
    {
    }
    
    
    unsigned long RASpeedDialSet::getKey(bool reloadFlag)
    {
        TA_ASSERT(m_key  != ULONG_MAX, "The [key] is not set");

        // If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == ULONG_MAX )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( reloadFlag && m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_key;
    }
    
    
    bool RASpeedDialSet::isSystem()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == ULONG_MAX )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_isSystem;
    }
    
    
    unsigned long RASpeedDialSet::getOperatorKey()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == ULONG_MAX )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_operatorKey;
    }
    
    
    unsigned long RASpeedDialSet::getProfileId()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == ULONG_MAX )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_profileId;
    }
    
    
    std::string RASpeedDialSet::getSetName()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == ULONG_MAX )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_setName;
    }
    
    IRASpeedDialSet& RASpeedDialSet::setSystem(bool is_system)
    {
        if (!m_isValidData && ULONG_MAX == m_key)
        {
            reload();
        }
        m_isSystem = is_system;
        return *this;
    }
    
    
    IRASpeedDialSet& RASpeedDialSet::setOperatorKey(unsigned long operatorKey)
    {
        if (!m_isValidData && ULONG_MAX == m_key)
        {
            reload();
        }
        m_operatorKey = operatorKey;
        return *this;
    }
    
    
    IRASpeedDialSet& RASpeedDialSet::setProfileId(unsigned long profileId)
    {
        if (!m_isValidData && ULONG_MAX == m_key)
        {
            reload();
        }
        m_profileId = profileId;
        return *this;
    }
    
    
    IRASpeedDialSet& RASpeedDialSet::setSetName(const std::string& setName)
    {
        if (!m_isValidData && ULONG_MAX == m_key)
        {
            reload();
        }
        m_setName = setName;
        return *this;
    }
    
    
    
    void RASpeedDialSet::invalidate()
    {
        m_isValidData = false;  
    }

    IRASpeedDialSet* RASpeedDialSet::createNewSetByName(const std::string& name,const unsigned long profileId)
    {
        RASpeedDialSet* theSet = new RASpeedDialSet(name);

        // get a connection to the database
        IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);

        theSet->m_key = theSet->getNextKey(databaseConnection);
        if (profileId != 0)
        {
            theSet->m_profileId = profileId;
        }
        theSet->m_isWritten = false;

        return theSet;
    }
    
    
    void RASpeedDialSet::applyChanges()
    {
//    TA_ASSERT(false,"Not implemented yet.");

        LOG ( SourceInfo, DebugUtil::FunctionEntry, "RASpeedDialSet::applyChanges" );     
//		TA_ASSERT( ULONG_MAX == m_key , "[key] not set, can not apply" );

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
    
        bool doUpdate = false;  
        // Do existing record check the hard way...
        try 
        {
            {
                RASpeedDialSet checkExisting(m_key);
                checkExisting.getSetName(); // force load
            }
            // update only called if record found (no data exception causes update to be skipped)
            doUpdate = true;
        }
        catch (DataException& e)
        {
            if (e.getFailType() != DataException::NO_VALUE) 
            {
                throw e;
            }
        }

        // get a connection to the database
        IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Write);
        
        //std::stringstream s;
		SQLStatement strSql;
        if (doUpdate) {            
            LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Updating RASpeedDialSet (id: %ld) to the database.",
                m_key);            
            
           /* s   << "update RA_SPEED_DIAL_SET set (IS_SYSTEM, OPERATOR_KEY, SEPROF_ID, SET_NAME) "
                << " = ( select "
                << " '" << (m_isSystem ? 0 : 1) << "', "
                << " '" << m_operatorKey << "', "
                << " '" << m_profileId << "', "
                << " '" << databaseConnection->escapeInsertString(m_setName) << "' "
                << "  from dual ) " 
                << " where RASDSE_ID = " << m_key 
                << std::ends;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_SET_STD_UPDATE_36651, 
//			strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_SET_UPDATE_36651, 

		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_SET_UPDATE_36651,
				(m_isSystem ? 0 : 1), m_operatorKey, m_profileId, databaseConnection->escapeInsertString(m_setName), m_key);
        }
        else 
        {
            // if we got here, then the record doesn't exist, so let's create it...
            
            // GET THE NEW KEY FIRST
            m_key = getNextKey( databaseConnection );
            
            LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Inserting RASpeedDialSet (id: %ld) to the database.",
                m_key);            
            
            // now we have the key, let's insert the record into the db

           /* s   << "insert into RA_SPEED_DIAL_SET (RASDSE_ID, IS_SYSTEM, OPERATOR_KEY, SEPROF_ID, SET_NAME) "
                << " values ( "
                << " '" << m_key << "', "
                << " '" << (m_isSystem ? 0 : 1) << "', "
                << " '" << m_operatorKey << "', "
                << " '" << m_profileId<< "', "
                << " '" << databaseConnection->escapeInsertString(m_setName) << "' "
                << " )" 
                << std::ends;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_SET_STD_INSERT_36801, m_key,
//			strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_SET_INSERT_36801, m_key,

		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_SET_INSERT_36801, m_key,
				(m_isSystem ? 0 : 1), m_operatorKey, m_profileId, databaseConnection->escapeInsertString(m_setName));
        }
        //std::string sql(s.str());            
        
        // This will throw an exception upon failure - ready to catch??
        databaseConnection->executeModification(strSql);
        
        // Set the data to be valid - no need to force a reload after a succesful write
        m_isValidData = true;
        m_isWritten = true;
    
    }


    unsigned long RASpeedDialSet::getNextKey( IDatabase* databaseConnection )
    {
        std::string colKey("RASDSE_ID");
        std::vector<std::string> columnNames;
        columnNames.push_back(colKey);
        
       /* std::stringstream s;
        s << " select RASDSE_SEQ.nextval as RASDSE_ID from dual " << std::ends;
        std::string sql(s.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_SET_Oracle_SELECT_36551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_SET_SELECT_36551);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_SET_SELECT_36551);
        
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
        unsigned long key = data->getUnsignedLongData(0,colKey);
        
        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data; data = 0;
        
        return key;
    }


    void RASpeedDialSet::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key or the name, depending what is set...
        std::stringstream s;
        //s << " select RASDSE_ID, IS_SYSTEM, OPERATOR_KEY, SEPROF_ID, SET_NAME from RA_SPEED_DIAL_SET ";
        if (m_key == ULONG_MAX) // no key, try by setName
        {
            s << " where SET_NAME = '" << databaseConnection->escapeQueryString(m_setName) << "' ";
        }
        else 
        {
            s << " where RASDSE_ID = " << m_key;
        }
        s << std::ends;
        //std::string sql(s.str());
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_SET_STD_SELECT_36501,s.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_SET_SELECT_36501,s.str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_SET_SELECT_36501,s.str());

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
        m_key            = data->getUnsignedLongData(0,colKey);
        m_isSystem       = data->getBooleanData(0,colIsSystem);
        m_operatorKey    = data->getUnsignedLongData(0,colOperatorKey);
        m_profileId      = data->getUnsignedLongData(0,colProfileId);
        m_setName        = data->getStringData(0,colSetName);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data; data = 0;
        
        // Need to record that we now have valid data
        m_isValidData = true;
		m_isWritten = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }

} // closes TA_Base_Core
