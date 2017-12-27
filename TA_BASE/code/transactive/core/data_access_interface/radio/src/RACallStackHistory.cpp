 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RACallStackHistory.cpp $
  * @author Glen Kidd
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * RACallStackHistory is an implementation of IRACallStackHistory. It holds the data specific to an RACallStackHistory entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/radio/src/RACallStackHistory.h"
#include "core/data_access_interface/radio/src/RACallStackHistoryAccessFactory.h"
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

	const unsigned long RACallStackHistory::rowLimit = 50;

    RACallStackHistory::RACallStackHistory(const unsigned long& key)
        : m_key(key),
        m_profile(""),
        m_locationKey(0),
		m_timestamp(0),
        m_origin(IRACallStackHistory::ORIGIN_INVALID),
        m_callType(IRACallStackHistory::CALL_INVALID),
		m_identifier(""),
        m_location(""),
        m_isValidData( false ), // forces reload
        m_isWritten( true ),
		m_isEmergency(false) // td17294
    {
    }


    RACallStackHistory::RACallStackHistory(
            const unsigned long& key,
            const std::string& profile,
            const unsigned long& locationKey,
			const time_t& timestamp,
            OriginType originType,
            CallType callType,
            const std::string& identifier,
            const std::string& location,
            bool isNew,
			bool isEmergency // td17294
        )
        : m_key(key),
        m_profile(profile),
        m_locationKey(locationKey),
		m_timestamp(timestamp),
        m_origin(originType),
        m_callType(callType),
		m_identifier(identifier),
        m_location(location),
        m_isValidData( true && !isNew ), // assuming that if !new (ie. loaded from db, it is valid)
        m_isWritten( !isNew ),
		m_isEmergency(isEmergency)
    {
    }


    RACallStackHistory::~RACallStackHistory() 
	{
    }

    // td17294
	bool RACallStackHistory::isCallEmergency()
	{
		return m_isEmergency;		
	}

	IRACallStackHistory& RACallStackHistory::setEmergencyInfo(const bool isEmergency)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_isEmergency = isEmergency;
        return *this;
    }

    unsigned long RACallStackHistory::getKey(bool reloadFlag) // key
    {
        TA_ASSERT(m_key != 0, "The Key is not set");

        // If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == 0 )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( reloadFlag && m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_key;
    }


	std::string RACallStackHistory::getProfileName()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == 0 )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_profile;
    }


    unsigned long RACallStackHistory::getLocationKey()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == 0 )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_locationKey;
    }


	time_t RACallStackHistory::getTimestamp()
    {
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == 0 )
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_timestamp;
    }


    IRACallStackHistory::OriginType RACallStackHistory::getOrigin()
	{
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == 0)
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_origin;
    }


    IRACallStackHistory::CallType RACallStackHistory::getCallType()
	{
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == 0)
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_callType;
    }

		
	std::string RACallStackHistory::getIdentifier()
	{
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == 0)
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_identifier;
    }


	std::string RACallStackHistory::getLocation()
	{
		// If the data isn't written & the key isn't set then we must reload
		if ( ! m_isWritten && m_key == 0)
		{
			TA_ASSERT( true, "The [key] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }

        return m_location;
    }
	
#if 0
    IRACallStackHistory& RACallStackHistory::setKey(const unsigned long& key)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_key = key;
        return *this;
    }
#endif

    IRACallStackHistory& RACallStackHistory::setProfileName(const std::string& profile)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_profile = profile;
        return *this;
    }

    IRACallStackHistory& RACallStackHistory::setLocationKey(const unsigned long& locationKey)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_locationKey = locationKey;
        return *this;
    }

    IRACallStackHistory& RACallStackHistory::setTimestamp(const time_t& timestamp)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_timestamp = timestamp;
        return *this;
    }

    IRACallStackHistory& RACallStackHistory::setOrigin(const IRACallStackHistory::OriginType& origin)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_origin = origin;
        return *this;
    }
    
	IRACallStackHistory& RACallStackHistory::setCallType(const IRACallStackHistory::CallType& callType)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_callType = callType;
        return *this;
    }

	IRACallStackHistory& RACallStackHistory::setIdentifier(const std::string& identifier)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_identifier = identifier;
        return *this;
    }
    
    IRACallStackHistory& RACallStackHistory::setLocation(const std::string& location)
    {
        if (!m_isValidData && 0 != m_key)
        {
            reload();
        }
        m_location = location;
        return *this;
    }

    
    void RACallStackHistory::invalidate()
    {
        m_isValidData = false;  
    }

#if 0 // only changes through access factory... move this code to af::add...()
    void RACallStackHistory::applyChanges()
    {
    }
#endif

    void RACallStackHistory::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Cd, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
      /*  std::stringstream s;
        s   << "select RACSHI_ID, PROFILE_NAME, LOCATION_KEY, "
			<< "       to_char(TIMESTAMP,'YYYYMMDDHH24MMSS') TIMESTAMP, "
			<< "       ORIGIN, CALL_TYPE, IDENTIFIER, LOCATION, ISEMERGENCY " // td17294
            << " from RA_CALL_STACK_HISTORY "
            << " where RACSHI_ID = " << m_key 
            << std::ends;
        std::string sql(s.str());  */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_CALL_STACK_HISTORY_Oracle_SELECT_34052, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_CALL_STACK_HISTORY_SELECT_34052, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_CALL_STACK_HISTORY_SELECT_34052, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string colRACSHI_ID("RACSHI_ID");
        std::string colPROFILE("PROFILE");
        std::string colLOCATION_KEY("LOCATION_KEY");
        std::string colTIMESTAMP("TIMESTAMP");
        std::string colORIGIN("ORIGIN");
        std::string colCALL_TYPE("CALL_TYPE");
        std::string colIDENTIFIER("IDENTIFIER");
        std::string colLOCATION("LOCATION");
		std::string colISEMERGENCY("ISEMERGENCY"); // td17294

        std::vector<std::string> columnNames;
		columnNames.push_back(colRACSHI_ID);
		columnNames.push_back(colPROFILE);
		columnNames.push_back(colLOCATION_KEY);
        columnNames.push_back(colTIMESTAMP);
        columnNames.push_back(colORIGIN);
        columnNames.push_back(colCALL_TYPE);
        columnNames.push_back(colIDENTIFIER);
        columnNames.push_back(colLOCATION);
		columnNames.push_back(colISEMERGENCY); // td17294

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
			sprintf(reasonMessage, "No record found for key = %ld", m_key );			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey - SHOULD NEVER HAPPEN (unique key constraint)
        {
            delete data; data = 0;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one record found with key = %ld", m_key );
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key = data->getUnsignedLongData(0,colRACSHI_ID);
        m_profile = data->getStringData(0,colPROFILE);
        m_locationKey  = data->getUnsignedLongData(0,colLOCATION_KEY);
        m_timestamp = data->getDateData(0,colTIMESTAMP);
        m_origin = static_cast<IRACallStackHistory::OriginType>(data->getStringData(0,colORIGIN)[0]);
        m_callType = static_cast<IRACallStackHistory::CallType>(data->getStringData(0,colCALL_TYPE)[0]);
        m_identifier = data->getStringData(0,colIDENTIFIER);
        m_location = data->getStringData(0,colLOCATION);
		m_isEmergency = data->getBooleanData(0,colISEMERGENCY);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data; data = 0;
        
        // Need to record that we now have valid data
        m_isValidData = true;
		m_isWritten = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }

} // closes TA_Base_Core
