 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASpeedDialButton.cpp $
  * @author Glen Kidd
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * RASpeedDialButton is an implementation of IRASpeedDialButton. It holds the data specific to an RASpeedDialButton entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/radio/src/RASpeedDialButton.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
//#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    RASpeedDialButton::RASpeedDialButton(const unsigned long key)
        : m_key(key),
        m_dialSetKey(0),
        m_buttonPosition(0),
        m_buttonLabel(""),
        m_buttonTSI(""),
        m_isValidData( false ), 
        m_isWritten( true )
    {
    }


    RASpeedDialButton::RASpeedDialButton(
        unsigned int key,
        unsigned int setKey,
        unsigned int buttonPosition,
        const std::string& buttonLabel,
        const std::string& buttonTSI,
        bool isNew
        ) 
        : m_key(key),
        m_dialSetKey(setKey),
        m_buttonPosition(buttonPosition),
        m_buttonLabel(buttonLabel),
        m_buttonTSI(buttonTSI),
        m_isValidData( true && !isNew ), // assuming that if !new (ie. loaded from db, it is valid)
        m_isWritten( !isNew )
    {
    }


    RASpeedDialButton::~RASpeedDialButton() 
	{
    }
    

    unsigned long RASpeedDialButton::getKey(bool reloadFlag)
    {
        TA_ASSERT(m_key  != ULONG_MAX, "The TSI is not set");
        
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
    
    unsigned long RASpeedDialButton::getSpeedDialSetKey()
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

        return m_dialSetKey;
    }
    
    unsigned long RASpeedDialButton::getButtonPosition()
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

        return m_buttonPosition;
    }
    
    std::string RASpeedDialButton::getButtonLabel()
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

        return m_buttonLabel;
    }
    
    std::string RASpeedDialButton::getButtonTSI()
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

        return m_buttonTSI;
    }
    
    IRASpeedDialButton& RASpeedDialButton::setSpeedDialSetKey(unsigned long key)
    {
        if (!m_isValidData && ULONG_MAX == m_key)
        {
            reload();
        }
        m_dialSetKey = key;
        return *this;
    }
    
    IRASpeedDialButton& RASpeedDialButton::setButtonPosition(unsigned long buttonPosition)
    {
        if (!m_isValidData && ULONG_MAX == m_key)
        {
            reload();
        }
        m_buttonPosition = buttonPosition;
        return *this;
    }
    
    IRASpeedDialButton& RASpeedDialButton::setButtonLabel(const std::string& buttonLabel)
    {
        if (!m_isValidData && ULONG_MAX == m_key)
        {
            reload();
        }
        m_buttonLabel = buttonLabel;
        return *this;
    }
    
    IRASpeedDialButton& RASpeedDialButton::setButtonTSI(const std::string& buttonTSI)
    {
        if (!m_isValidData && ULONG_MAX == m_key)
        {
            reload();
        }
        m_buttonTSI = buttonTSI;
        return *this;
    }


    void RASpeedDialButton::invalidate()
    {
        m_isValidData = false;  
    }
    

    /// @todo This method is a little on the large side - try to refactor our the common bits and generally clean it up. 
    void RASpeedDialButton::applyChanges()
    {   
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "RASpeedDialButton::applyChanges" );     
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
                RASpeedDialButton checkExisting(m_key);
                checkExisting.getButtonLabel(); // force load
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
                "Updating RASpeedDialButton (id: %ld) to the database.",
                m_key);            
            
           /* s   << "update RA_SPEED_DIAL_BUTTON set (RASDSE_ID, BUTTON_POSITION, BUTTON_LABEL, BUTTON_TSI) "
                << " = ( select  "
                << " '" << m_dialSetKey << "', "
                << " '" << m_buttonPosition << "', "
                << " '" << databaseConnection->escapeInsertString(m_buttonLabel) << "', "
                << " '" << databaseConnection->escapeInsertString(m_buttonTSI) << "' "
                << "  from dual) " 
                << " where RASDBU_ID = " << m_key 
                << std::ends;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_BUTTON_STD_UPDATE_36151,
//			strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_BUTTON_UPDATE_36151,

		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_BUTTON_UPDATE_36151,
				m_dialSetKey, m_buttonPosition, databaseConnection->escapeInsertString(m_buttonLabel), 
				databaseConnection->escapeInsertString(m_buttonTSI), m_key);
        }
        else 
        {
            // if we got here, then the record doesn't exist, so let's create it...
            
            // GET THE NEW KEY FIRST
            m_key = getNextKey(databaseConnection);

            LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Inserting RASpeedDialButton (id: %ld) to the database.",
                m_key);            
    
            // now we have the key, let's insert the record into the db

            /*s   << "insert into RA_SPEED_DIAL_BUTTON (RASDBU_ID, RASDSE_ID, BUTTON_POSITION, BUTTON_LABEL, BUTTON_TSI) "
                << " values ( "
                << " '" << m_key << "', "
                << " '" << m_dialSetKey << "', "
                << " '" << m_buttonPosition << "', "
                << " '" << databaseConnection->escapeInsertString(m_buttonLabel) << "', "
                << " '" << databaseConnection->escapeInsertString(m_buttonTSI) << "' "
                << " )" 
                << std::ends;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_BUTTON_STD_INSERT_36301,
//			strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_BUTTON_INSERT_36301,

		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_BUTTON_INSERT_36301,
				m_key, m_dialSetKey, m_buttonPosition, databaseConnection->escapeInsertString(m_buttonLabel),
				databaseConnection->escapeInsertString(m_buttonTSI));
        }
        //std::string sql(s.str());
        
        // This will throw an exception upon failure - ready to catch??
        databaseConnection->executeModification(strSql);
        
        // Set the data to be valid - no need to force a reload after a succesful write
        m_isValidData = true;
        m_isWritten = true;
        
    }


    unsigned long RASpeedDialButton::getNextKey( IDatabase* databaseConnection )
    {
        std::string colKey("RASDBU_ID");
        std::vector<std::string> columnNames;
        columnNames.push_back(colKey);
        
       /* std::stringstream s;
        s << " select RASDBU_SEQ.nextval as RASDBU_ID from dual " << std::ends;
        std::string sql(s.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_BUTTON_Oracle_SELECT_36051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_BUTTON_SELECT_36051);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_BUTTON_SELECT_36051);
        
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



    void RASpeedDialButton::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
      /*  std::stringstream s;
        s << " select RASDBU_ID, RASDSE_ID, BUTTON_POSITION, BUTTON_LABEL, BUTTON_TSI from RA_SPEED_DIAL_BUTTON ";
//        if (m_key == ULONG_MAX) // no key, try by setName
//        {
//            s << " where BUTTON_LABEL = '" << databaseConnection->escapeQueryString(m_setName) << "' ";
//        }
//        else 
//        {
            s << " where RASDBU_ID = " << m_key;
//        }
        s << std::ends;
        std::string sql(s.str());  */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_BUTTON_STD_SELECT_36001, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_BUTTON_SELECT_36001, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_BUTTON_SELECT_36001, m_key);
        // Set up the columnNames vector to be passed to executeQuery()
        std::string colKey("RASDBU_ID");
        std::string colSpeedDialSetKey("RASDSE_ID");
        std::string colButtonPosition("BUTTON_POSITION");
        std::string colButtonLabel("BUTTON_LABEL");
        std::string colButtonTSI("BUTTON_TSI");

        std::vector<std::string> columnNames;
		columnNames.push_back(colKey);
		columnNames.push_back(colSpeedDialSetKey);
		columnNames.push_back(colButtonPosition);
        columnNames.push_back(colButtonLabel);
        columnNames.push_back(colButtonTSI);

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
        m_dialSetKey     = data->getUnsignedLongData(0,colSpeedDialSetKey);
        m_buttonPosition = data->getUnsignedLongData(0,colButtonPosition);
        m_buttonLabel    = data->getStringData(0,colButtonLabel);
        m_buttonTSI      = data->getStringData(0,colButtonTSI);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data; data = 0;
        
        // Need to record that we now have valid data
        m_isValidData = true;
		m_isWritten = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }


} // closes TA_Base_Core
