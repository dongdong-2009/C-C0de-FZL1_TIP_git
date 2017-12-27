 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageTypeHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * MessageTypeHelper is an object that is held by MessageTypeData and ConfigMessageType objects. 
  * It contains all data used by message types, and manipulation
  * methods for the data. It helps avoid re-writing code for both MessageTypeData and ConfigMessageType.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/MessageTypeHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	
    MessageTypeHelper::MessageTypeHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isValidData(false)
    {
        FUNCTION_ENTRY("MessageTypeHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }

	
	unsigned long MessageTypeHelper::getKey()
    {
        return atol(m_key.c_str());
    }


    std::string MessageTypeHelper::getName()
    {
        if (!m_isValidData) // reload the data if it is flaged as invalid
        {
            reload();
        }
        return m_name;
    }

    std::string MessageTypeHelper::getDescription()
    {
        if (!m_isValidData)
        {
            reload();
        }
        return m_description;
    }

    void MessageTypeHelper::setDescription(const std::string& description)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::setDescription" );
        if (!m_isValidData)
        {
            reload();
        }
        m_description = description;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::setDescription" );
    }

    void MessageTypeHelper::setName(const std::string& name)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::setName" );
        if (!m_isValidData)
        {
            reload();
        }
        m_name = name;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::setName" );
    }

	void MessageTypeHelper::setDisplayName(const std::string& displayname)
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::setDisplayName" );
		if (!m_isValidData)
		{
			reload();
		}
		m_displayname = displayname;
		LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::setDisplayName" );
	}

    unsigned int MessageTypeHelper::getPriority()
    {
        if (!m_isValidData)
        {
            reload();
        }
        return m_priority;
    }

    void MessageTypeHelper::setPriority(const unsigned int priority)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::setPriority" );

        TA_ASSERT( 10 > priority,"The Priority must be between 0 and 9");

        if (!m_isValidData)
        {
            reload();
        }
        
        m_priority = priority;
        
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::setPriority" );
    }

    bool MessageTypeHelper::toBePrinted()
    {
        if (!m_isValidData)
        {
            reload();
        }
        return m_toBePrinted;
    }

	bool MessageTypeHelper::getIsEnableRealTimeEvent()
	{
		if(!m_isValidData)
		{
			reload();
		}
		return m_isRealTimeEvent;
	}

	unsigned long MessageTypeHelper::getActionId()
	{
		if(!m_isValidData)
		{
			reload();
		}
		return m_actionID;
	}
    void MessageTypeHelper::setPrintMessageStatus(const bool printMessageStatus)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::setPrintMessageStatus" );
        if (!m_isValidData)
        {
            reload();
        }
        m_toBePrinted = printMessageStatus;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::setPrintMessageStatus" );
    }

    bool MessageTypeHelper::toBeLogged()
    {
        if (!m_isValidData)
        {
            reload();
        }
        return m_toBeLogged;
    }

    void MessageTypeHelper::setLogMessageStatus(const bool logMessageStatus)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::setLogMessageStatus" );
        if (!m_isValidData)
        {
            reload();
        }
        m_toBeLogged = logMessageStatus;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::setLogMessageStatus" );
    }

    bool MessageTypeHelper::toBePersisted()
    {
        if (!m_isValidData)
        {
            reload();
        }
        return m_toBePersisted;
    }

    void MessageTypeHelper::setPersistMessageStatus(const bool persistMessageStatus)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::setPersistMessageStatus" );
        if (!m_isValidData)
        {
            reload();
        }
        m_toBePersisted = persistMessageStatus;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::setPersistMessageStatus" );
    }

    void MessageTypeHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::invalidate" );

        // set the flag to false
        m_isValidData = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::invalidate" );
    }

    bool MessageTypeHelper::isNew()
    {
        // New Message Types cannot be created.
        return false;
    }

    time_t MessageTypeHelper::getDateCreated()
    {
        if ( !m_isValidData )
        {
            reload();
        }
        return m_dateCreated;
    }

    time_t MessageTypeHelper::getDateModified()
    {
        if ( !m_isValidData )
        {
            reload();
        }
        return m_dateModified;
    }

    unsigned int MessageTypeHelper::getViewLevel()
    {
        if ( !m_isValidData )
        {
            reload();
        }
        return m_viewLevel;
    }

    void MessageTypeHelper::setViewLevel( const unsigned int viewLevel )
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::setViewLevel" );
        if ( !m_isValidData )
        {
            reload();
        }
        m_viewLevel = viewLevel;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::setViewLevel" );
    }

	void MessageTypeHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

        columnNames.push_back("MEMTYP_ID");
        columnNames.push_back("NAME");
		columnNames.push_back("DISPLAY_NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("PRIORITY");
        columnNames.push_back("PRINTMESSAGE");
        columnNames.push_back("LOGMESSATE");
        columnNames.push_back("PERSISTMESSAGE");
		columnNames.push_back("VIEWLEVEL");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		columnNames.push_back("ACTION_ID");
		columnNames.push_back("ISENABLEREALTIMEEVENT");
		// Column name is meaningless but column order is very important. We just modified the name for naming convention, it should be same with database column name.
	}
	std::string MessageTypeHelper::getDisplayName(){
		if (!m_isValidData) // reload the data if it is flaged as invalid
		{
			reload();
		}
		return m_displayname;
	}

    void MessageTypeHelper::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::reload" );
       
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the data of the message type
        // based upon the key
        std::ostringstream sql;

        // Note the use of the nvl command on the description column - this is used because
        // the description column can be null, but that will cause SimpleDb to have a heart-
        // attack. It can be removed when SimpleDb is no longer being used.
		/*sql << "SELECT memtyp_id, type_name, NVL(type_description,'NULL') as type_description, priority, print_message, log_message, persist_message,"
            << "view_level, TO_CHAR(date_modified, 'YYYYMMDDHH24MISS'), TO_CHAR(date_created,'YYYYMMDDHH24MISS') "
            << "FROM me_message_type WHERE memtyp_id = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_message_type_Oracle_SELECT_10055, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ME_MESSAGE_TYPE_SELECT_10055, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ME_MESSAGE_TYPE_SELECT_10055, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql,columnNames));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No message type found for memtyp_id = %s",m_key.c_str());			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of MessageType records."); 
        reloadUsing(0, *data);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::reload" );
    }


	void MessageTypeHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);
		
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key  = data.getStringData(row, columnNames[0]);      
		m_name = data.getStringData(row, columnNames[1]);
		m_displayname = data.getStringData(row, columnNames[2]);
        m_description = data.getStringData(row, columnNames[3]);
        m_priority = data.getIntegerData(row, columnNames[4]);
        unsigned int printFlag = data.getIntegerData(row, columnNames[5]);
        m_toBePrinted = (1 == printFlag); 
        unsigned int logFlag = data.getIntegerData(row, columnNames[6]);
        m_toBeLogged = (1 == logFlag);
        unsigned int persistFlag = data.getIntegerData(row, columnNames[7]);
        m_toBePersisted = (1 == persistFlag);
        m_viewLevel = data.getUnsignedLongData(row, columnNames[8]);
        
        m_dateModified = data.getDateData(row, columnNames[9], 0);
        m_dateCreated = data.getDateData(row, columnNames[10]);

		m_actionID = data.getUnsignedLongData(row, columnNames[11]);
		unsigned int isRealTimeEvent = data.getIntegerData(row, columnNames[12]);
		m_isRealTimeEvent = (1==isRealTimeEvent);

        // If description is NULL, that means it was actually an empty value, and we should
        // cast it as such
        if ("NULL" == m_description)
        {
            m_description = "";
        }
        // Check the validity of the data based on restrictions we know
        if (9 < m_priority) // Priority must be between 0..9. As m_priority is an 
                            // unsigned int, it can't be negative, so only need to
                            // test for greater than 9.
        {
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "Priority %d not valid for memtyp_id = %s",m_priority,m_key.c_str());
            TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,""));
        }
        if (1 < printFlag) //Not a boolean value
        {
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "Print Flag %d not valid for memtyp_id = %s",printFlag, m_key.c_str());
            TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,""));
        }
        if (1 < logFlag) //Not a boolean value
        {
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "Log Flag %d not valid for memtyp_id = %s",logFlag, m_key.c_str());
            TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,""));
        }
        if (1 < persistFlag) //Not a boolean value
        {
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "Persist Flag %d not valid for memtyp_id = %s", persistFlag, m_key.c_str());
            TA_THROW(DataException(reasonMessage,DataException::WRONG_TYPE,""));
        }

        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void MessageTypeHelper::writeMessageTypeData()
    {       
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::writeMessageTypeData" );     
        
        // Don't need to check the values exists, as Message Types cannot be created, only
        // loaded from the database. 
        // Only need to check the string fields are not null.
        bool nameNull = m_name == "";
        
        std::vector<std::string> fieldNames;
        if (nameNull)
        {
            fieldNames.push_back("type_name");
        }
        if (nameNull) // Need to throw an exception
        {
                TA_THROW(TA_Base_Core::DataConfigurationException("Message type data not fully specified. Message type cannot be written to database"
                                                                ,fieldNames));
        }

        // Write the Message Type to the database

        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // Always perform an update, as the Message Type will always exist.
        
        // Create a select statement that will ensure the uniqueness of the message type name.
        /*std::string formatSQL = "select memtyp_id from me_message_type where type_name = '"
            + databaseConnection->escapeQueryString(m_name) + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_message_type_STD_SELECT_10001,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ME_MESSAGE_TYPE_SELECT_10001,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ME_MESSAGE_TYPE_SELECT_10001,
			databaseConnection->escapeQueryString(m_name));

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 rows returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getStringData(0,keyColumn))
            {   // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                char reasonMessage[256] = {0};
			    sprintf(reasonMessage, "An message type already exists with name = %s or description = %s",
                    m_name.c_str(),m_description.c_str());			
                throw DataException(reasonMessage,DataException::NOT_UNIQUE,"");
            }
            // else - it matches, so that's okay - it just means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one message type exists with name = %s or description = %s",
                m_name.c_str(),m_description.c_str());			
            throw DataException(reasonMessage,DataException::NOT_UNIQUE,"");
        }
        /*char sql[1024];
        sprintf(sql,"update me_message_type set type_name = '%s', type_description = '%s', priority = %d, print_message = %d, log_message = %d, persist_message = %d, view_level = %d where memtyp_id = %s",
            databaseConnection->escapeInsertString(m_name).c_str(),
            databaseConnection->escapeInsertString(m_description).c_str()
            ,m_priority,m_toBePrinted,m_toBeLogged,
            m_toBePersisted, m_viewLevel, m_key.c_str());*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_message_type_STD_UPDATE_10151, databaseConnection->escapeInsertString(m_name),
//		strSql  = databaseConnection->prepareSQLStatement(ME_MESSAGE_TYPE_UPDATE_10151, databaseConnection->escapeInsertString(m_name),

		databaseConnection->prepareSQLStatement(strSql, ME_MESSAGE_TYPE_UPDATE_10151, databaseConnection->escapeInsertString(m_name),
			databaseConnection->escapeInsertString(m_description), m_priority,m_toBePrinted,m_toBeLogged, m_toBePersisted, m_viewLevel, m_key);
        databaseConnection->executeModification(strSql);
        // nothing more to be done here
        
        // Invalidate the data so date values are updated.
        m_isValidData = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::writeMessageTypeData" );
    }

    void MessageTypeHelper::deleteMessageType()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageTypeHelper::deleteMessageType" );
                    
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        /*char sql[128];
        sprintf(sql,"delete from me_message_type where memtyp_id = %s",
            m_key.c_str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_message_type_STD_DELETE_10401, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ME_MESSAGE_TYPE_DELETE_10401, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ME_MESSAGE_TYPE_DELETE_10401, m_key);
        databaseConnection->executeModification(strSql);

        m_key = "0";
        m_name = "";
		m_displayname = "";
        m_description = "";
        m_priority = 10;
        m_toBePrinted = false;
        m_toBeLogged = false;
        m_toBePersisted = false;
        m_viewLevel = 0;
        m_dateCreated = 0;
        m_dateModified = 0;
        m_isValidData = false;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageTypeHelper::deleteMessageType" );
    }

} // closes TA_Base_Core

