/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EventColourHelper.cpp $
 * @author Derrick Liew
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by: $Author: Ouyang $
 * 
 * Provides access to the Event Colour data in the database.
 */


// EventColourHelper.cpp: implementation of the EventColourHelper class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <time.h>

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/EventColourHelper.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_Base_Core
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    EventColourHelper::EventColourHelper(unsigned long key)
		:
        m_name(""),
        m_key(key),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isValidData(false)
    {
    }

    EventColourHelper::EventColourHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_name(""),
        m_isValidData(false)
    {
        FUNCTION_ENTRY("EventColourHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }

	EventColourHelper::~EventColourHelper()
	{
	}

	void EventColourHelper::setName(const std::string name)
    {
        if ( ! m_isValidData )
        {
            reload();
        }
        m_name = name;
    }


    void EventColourHelper::setEventColourKey(unsigned long colour, IEventColourData::EEventColourType type)
    {
        if ( ! m_isValidData )
        {
            reload();
        }

        std::map<IEventColourData::EEventColourType, unsigned long>::iterator matching;
        matching = m_eventColourKey.find(type);
        if (matching != m_eventColourKey.end())
        {
            matching->second = colour;
        }
        else
        {
            m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(type, colour) );
        }
    }


    unsigned long EventColourHelper::getKey()
    {
        return m_key;
    }


    std::string EventColourHelper::getName()
    {
        if ( ! m_isValidData )
        {
            reload();
        }
        return m_name;
    }


    unsigned long EventColourHelper::getEventColourKey(IEventColourData::EEventColourType type)
    {
        if ( ! m_isValidData )
        {
            reload();
        }

        std::map<IEventColourData::EEventColourType, unsigned long>::iterator matching;
        matching = m_eventColourKey.find(type);
        if (matching != m_eventColourKey.end())
        {
            return matching->second;
        }

        TA_THROW( DataException( "Event Colour type could not be found",DataException::NO_VALUE,"" ) );
    }


    time_t EventColourHelper::getDateCreated()
    {
        if ( ! m_isValidData )
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t EventColourHelper::getDateModified()
    {
        if( ! m_isValidData )
        {
            reload();
        }
        
        return m_dateModified;
    }


    void EventColourHelper::invalidate()
    {
        FUNCTION_ENTRY("EventColourHelper::invalidate");
		
		m_isValidData = false;
		
		FUNCTION_EXIT;
    }


	void EventColourHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

		columnNames.push_back("PKEY");
		columnNames.push_back("NAME");
        columnNames.push_back("OPEN_ACKED_FG_COLOUR1");
        columnNames.push_back("OPEN_ACKED_BG_COLOUR1");
        columnNames.push_back("OPEN_UNACKED_FG_COLOUR1");
        columnNames.push_back("OPEN_UNACKED_BG_COLOUR1");
        columnNames.push_back("CLOSED_UNACKED_FG_COLOUR1");
        columnNames.push_back("CLOSED_UNACKED_BG_COLOUR1");
        columnNames.push_back("CLOSED_ACKED_FG_COLOUR1");
        columnNames.push_back("CLOSED_ACKED_BG_COLOUR1");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}

	void EventColourHelper::reload()
	{
       FUNCTION_ENTRY("EventColourHelper::reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// create the SQL string to retrieve the data of the Severity
        /*std::ostringstream sql;
        sql << " select PKEY, NAME,"
			<< " OPEN_ACKED_FG_COLOUR1, OPEN_ACKED_BG_COLOUR1,"
			<< " OPEN_UNACKED_FG_COLOUR1, OPEN_UNACKED_BG_COLOUR1,"
			<< " CLOSED_UNACKED_FG_COLOUR1, CLOSED_UNACKED_BG_COLOUR1,"
			<< " CLOSED_ACKED_FG_COLOUR1, CLOSED_ACKED_BG_COLOUR1,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from EVENTCOLOUR where PKEY = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENTCOLOUR_Oracle_SELECT_15551, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENTCOLOUR_SELECT_15551, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENTCOLOUR_SELECT_15551, m_key);

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
		getColumnNames(columnNames);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery( strSql, columnNames ));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            char reasonMessage[256] = {0};
			sprintf( reasonMessage, "No event colour found for pkey = %lu", m_key );
			TA_THROW( DataException( reasonMessage,DataException::NO_VALUE,"" ) );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of event colour records."); 
        reloadUsing(0, *data);

        FUNCTION_EXIT;
	}
	
    void EventColourHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);
		
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key  = data.getUnsignedLongData( row, columnNames[0] );
        m_name = data.getStringData( row, columnNames[1] );

		m_eventColourKey.clear();
		m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(
                                IEventColourData::OPEN_ACKED_FG1, data.getUnsignedLongData(row,  columnNames[2]) ) );
        m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(
                                IEventColourData::OPEN_ACKED_BG1, data.getUnsignedLongData(row,  columnNames[3]) ) );

        m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(
                                IEventColourData::OPEN_UNACKED_FG1, data.getUnsignedLongData(row,  columnNames[4]) ) );
        m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(
                                IEventColourData::OPEN_UNACKED_BG1, data.getUnsignedLongData(row,  columnNames[5]) ) );
        
		m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(
                                IEventColourData::CLOSED_UNACKED_FG1, data.getUnsignedLongData(row,  columnNames[6]) ) );
        m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(
                                IEventColourData::CLOSED_UNACKED_BG1, data.getUnsignedLongData(row,  columnNames[7]) ) );
        
		m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(
                                IEventColourData::CLOSED_ACKED_FG1, data.getUnsignedLongData(row,  columnNames[8]) ) );
        m_eventColourKey.insert(std::map<IEventColourData::EEventColourType, unsigned long>::value_type(
                                IEventColourData::CLOSED_ACKED_BG1, data.getUnsignedLongData(row,  columnNames[9]) ) );
        
        m_dateModified = data.getDateData(row, columnNames[10], 0);
        m_dateCreated = data.getDateData( row,  columnNames[11] );

        m_isValidData = true;        
        FUNCTION_EXIT;
    }

    void EventColourHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        std::vector<std::string> fieldNames;

        // Check the name field isn't blank
        if(m_name == "")
        {
            fieldNames.push_back("name");
        }

        // If there are any fields missing
        if( !fieldNames.empty() )
        {
            // Throw an exception
            TA_THROW(TA_Base_Core::DataConfigurationException("Event colour data not fully specified. Event colour cannot be written to database",fieldNames));
        }
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::ostringstream sql;
        sql << "select PKEY from EVENTCOLOUR where NAME = '" << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENTCOLOUR_STD_SELECT_15501,
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENTCOLOUR_SELECT_15501,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENTCOLOUR_SELECT_15501,
			databaseConnection->escapeQueryString(m_name) );

        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0, keyColumn))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;
				databaseConnection->disconnect();

                std::ostringstream reasonMessage;
			    reasonMessage << "An event colour type already exists with name = "
                              << m_name.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
            }
            // else - it matches, so that's okay - it means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;
			databaseConnection->disconnect();

            std::ostringstream reasonMessage;
			reasonMessage << "More than one event colour type already exists with name = "
                          << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        delete data;
        data =  NULL;
		databaseConnection->disconnect();

		// create the SQL string to write the data of the Severity
		/*sql.str("");
        sql << " update EVENTCOLOUR "
			<< " set NAME = " << "'" << databaseConnection->escapeInsertString(m_name) << "', "
			<< " OPEN_ACKED_FG_COLOUR1 = " << m_eventColourKey[IEventColourData::OPEN_ACKED_FG1] << ", "
			<< " OPEN_ACKED_BG_COLOUR1 = " << m_eventColourKey[IEventColourData::OPEN_ACKED_BG1] << ", "
			<< " OPEN_UNACKED_FG_COLOUR1 = " << m_eventColourKey[IEventColourData::OPEN_UNACKED_FG1] << ", "
			<< " OPEN_UNACKED_BG_COLOUR1 = " << m_eventColourKey[IEventColourData::OPEN_UNACKED_BG1] << ", "
			<< " CLOSED_UNACKED_FG_COLOUR1 = " << m_eventColourKey[IEventColourData::CLOSED_UNACKED_FG1] << ", "
			<< " CLOSED_UNACKED_BG_COLOUR1 = " << m_eventColourKey[IEventColourData::CLOSED_UNACKED_BG1] << ", "
			<< " CLOSED_ACKED_FG_COLOUR1 = " << m_eventColourKey[IEventColourData::CLOSED_ACKED_FG1] << ", "
			<< " CLOSED_ACKED_BG_COLOUR1 = " << m_eventColourKey[IEventColourData::CLOSED_ACKED_BG1] << " "
			<< " where PKEY = " << m_key;*/

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        IDatabase* databaseConnectionWrite = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);
//		strSql = defPrepareSQLStatement(databaseConnectionWrite->GetDbServerType(), EVENTCOLOUR_STD_UPDATE_15651, 
//		strSql  = databaseConnectionWrite->prepareSQLStatement(EVENTCOLOUR_UPDATE_15651, 

		databaseConnectionWrite->prepareSQLStatement(strSql, EVENTCOLOUR_UPDATE_15651,
			databaseConnectionWrite->escapeInsertString(m_name), m_eventColourKey[IEventColourData::OPEN_ACKED_FG1],
			m_eventColourKey[IEventColourData::OPEN_ACKED_BG1], m_eventColourKey[IEventColourData::OPEN_UNACKED_FG1],
			m_eventColourKey[IEventColourData::OPEN_UNACKED_BG1], m_eventColourKey[IEventColourData::CLOSED_UNACKED_FG1],
			m_eventColourKey[IEventColourData::CLOSED_UNACKED_BG1], m_eventColourKey[IEventColourData::CLOSED_ACKED_FG1],
			 m_eventColourKey[IEventColourData::CLOSED_ACKED_BG1], m_key );
        databaseConnectionWrite->executeModification(strSql);
        
        //now need to update the date fields
        /*sql.str("");
        sql << " select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS')"
			<< " from EVENTCOLOUR where PKEY = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnectionWrite->GetDbServerType(), EVENTCOLOUR_Oracle_SELECT_15553, m_key);
//		strSql  = databaseConnectionWrite->prepareSQLStatement(EVENTCOLOUR_SELECT_15553, m_key);

		databaseConnectionWrite->prepareSQLStatement(strSql, EVENTCOLOUR_SELECT_15553, m_key);

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnectionWrite->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;
			databaseConnectionWrite->disconnect();

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;
			databaseConnectionWrite->disconnect();

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
            m_dateModified = data->getDateData(0,modifiedColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
			databaseConnectionWrite->disconnect();
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;
		databaseConnectionWrite->disconnect();

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

} //close namespace
