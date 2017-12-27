 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAPredefinedSDSMessage.cpp $
  * @author Glen Kidd
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * RAPredefinedSDSMessage is an implementation of IRAPredefinedSDSMessage. It holds the data specific to an RAPredefinedSDSMessage entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/radio/src/RAPredefinedSDSMessage.h"
#include "core/data_access_interface/radio/src/RAPredefinedSDSMessageAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    RAPredefinedSDSMessage::RAPredefinedSDSMessage(const std::string& shortName)
        : m_shortName(shortName),
        m_message(""),
        m_isValidData( false ), // forces reload
        m_isWritten( true )
    {
    }


    RAPredefinedSDSMessage::RAPredefinedSDSMessage(
        const std::string& shortName,
        const std::string& message,
        bool isNew
        )
        : m_shortName(shortName),
        m_message(message),
        m_isValidData( true && !isNew ), // assuming that if !new (ie. loaded from db, it is valid)
        m_isWritten( !isNew )
    {
    }


    RAPredefinedSDSMessage::~RAPredefinedSDSMessage() 
	{
    }

    std::string RAPredefinedSDSMessage::getShortName(bool reloadFlag) // key
    {
        TA_ASSERT(m_shortName != "", "The TSI is not set");

        // If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_shortName == "" )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( reloadFlag && m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_shortName;
    }


    std::string RAPredefinedSDSMessage::getMessage()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_shortName == "" )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_message;
    }
	

    IRAPredefinedSDSMessage& RAPredefinedSDSMessage::setShortName(const std::string& shortName)
    {
        if (!m_isValidData && std::string("") != m_shortName)
        {
            reload();
        }
        m_shortName = shortName;
        return *this;
    }


    IRAPredefinedSDSMessage& RAPredefinedSDSMessage::setMessage(const std::string& message)
    {
        if (!m_isValidData && std::string("") != m_shortName)
        {
            reload();
        }
        m_message = message;
        return *this;
    }

    
    void RAPredefinedSDSMessage::invalidate()
    {
        m_isValidData = false;  
    }


    void RAPredefinedSDSMessage::applyChanges()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "RAPredefinedSDSMessage::applyChanges" );     
		TA_ASSERT( !m_shortName.empty(), "TSI not set, can not apply" );

        // Do existing record check the hard way...
        try 
        {
            {
                RAPredefinedSDSMessage checkExisting(m_shortName);
                checkExisting.getMessage(); // force load
            }
            // delete only called if record found (no data exception causes delete to be skipped)
            RAPredefinedSDSMessageAccessFactory::getInstance().deleteMessage(
                new RAPredefinedSDSMessage(m_shortName)); // use temp obj 'cause we need ourselves
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


        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Writing RAPredefinedSDSMessage (id: %s) to the database.",
            m_shortName.c_str());
        
	 
        /*std::stringstream s;
        s   << "insert into RA_PREDEFINED_SDS_MESSAGES (SHORT_NAME, MESSAGE) "
            << " values ( "
            << " '" << databaseConnection->escapeInsertString(m_shortName) << "', "
            << " '" << databaseConnection->escapeInsertString(m_message) << "' "
            << " )" 
            << std::ends;
        std::string sql(s.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_PREDEFINED_SDS_MESSAGES_STD_INSERT_35301,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_PREDEFINED_SDS_MESSAGES_INSERT_35301,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_SDS_MESSAGES_INSERT_35301,
			databaseConnection->escapeInsertString(m_shortName), databaseConnection->escapeInsertString(m_message));

        // This will throw an exception upon failure - ready to catch??
        databaseConnection->executeModification(strSql);

		// Set the data to be valid - no need to force a reload after a succesful write
		m_isValidData = true;
		m_isWritten = true;

    }


    void RAPredefinedSDSMessage::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        /*std::stringstream s;
        s   << "select SHORT_NAME, MESSAGE "
            << " from RA_PREDEFINED_SDS_MESSAGES "
            << " where SHORT_NAME = '" << databaseConnection->escapeQueryString(m_shortName) << "'"
            << std::ends;
        std::string sql(s.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_PREDEFINED_SDS_MESSAGES_STD_SELECT_35001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_PREDEFINED_SDS_MESSAGES_SELECT_35001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_PREDEFINED_SDS_MESSAGES_SELECT_35001,
			databaseConnection->escapeQueryString(m_shortName));

        // Set up the columnNames vector to be passed to executeQuery()
        std::string colSHORT_NAME("SHORT_NAME");
        std::string colMESSAGE("MESSAGE");

        std::vector<std::string> columnNames;
		columnNames.push_back(colSHORT_NAME);
		columnNames.push_back(colMESSAGE);

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
			sprintf(reasonMessage, "No Message found for shortName = %s", m_shortName.c_str() );			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey - SHOULD NEVER HAPPEN (unique key constraint)
        {
            delete data; data = 0;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one Message found with shortName = %s", m_shortName.c_str() );
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_shortName = data->getStringData(0,colSHORT_NAME);
        m_message   = data->getStringData(0,colMESSAGE);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data; data = 0;
        
        // Need to record that we now have valid data
        m_isValidData = true;
		m_isWritten = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }

} // closes TA_Base_Core
