/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmSeverityHelper.cpp $
 * @author Dirk McCormick
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by: $Author: Ouyang $
 * 
 * Provides access to the Alarm Severity data in the database.
 */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif // Win32


#include <string>
#include <time.h>

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/AlarmSeverityHelper.h"

#include "core/data_access_interface/src/ConfigEventColourData.h"
#include "core/data_access_interface/src/EventColourAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_Base_Core
{
    AlarmSeverityHelper::AlarmSeverityHelper(const unsigned long key)
		:
        m_name(""),
        m_key(key),
        m_dateCreated(0),
        m_dateModified(0), 
        m_isValidData(false)
	{
    }

    AlarmSeverityHelper::AlarmSeverityHelper(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_isValidData(false)
    {
        FUNCTION_ENTRY("AlarmSeverityHelper(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }


    AlarmSeverityHelper::~AlarmSeverityHelper()
    {
    }


    void AlarmSeverityHelper::setName(const std::string name)
    {
        if ( ! m_isValidData && m_name.empty())
        {
            reload();
        }
        m_name = name;
    }


    void AlarmSeverityHelper::setSeverityColourKey(unsigned long colour, IAlarmSeverityData::EColourType type)
    {
        if ( ! m_isValidData )
        {
            reload();
        }

        std::map<IAlarmSeverityData::EColourType, unsigned long>::iterator matching;
        matching = m_severityColourKey.find(type);
        if (matching != m_severityColourKey.end())
        {
            matching->second = colour;
        }
        else
        {
            m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(type, colour) );
        }
    }


    unsigned long AlarmSeverityHelper::getKey()
    {
        return m_key;
    }


    std::string AlarmSeverityHelper::getName()
    {
        if ( ! m_isValidData && m_name.empty())
        {
            reload();
        }
        return m_name;
    }


    unsigned long AlarmSeverityHelper::getSeverityColourKey(IAlarmSeverityData::EColourType type)
    {
        if ( ! m_isValidData )
        {
            reload();
        }

        std::map<IAlarmSeverityData::EColourType, unsigned long>::iterator matching;
        matching = m_severityColourKey.find(type);
        if (matching != m_severityColourKey.end())
        {
            return matching->second;
        }

        TA_THROW( DataException( "Alarm Severity Colour type could not be found",DataException::NO_VALUE,"" ) );
    }


    time_t AlarmSeverityHelper::getDateCreated()
    {
        if ( ! m_isValidData && ! m_dateCreated )
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t AlarmSeverityHelper::getDateModified()
    {
        if( ! m_isValidData && ! m_dateModified )
        {
            reload();
        }
        
        return m_dateModified;
    }


    void AlarmSeverityHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmSeverityHelper::invalidate" );
		//TD17850 - Additional fix, so that name would be reloaded
		m_name = "";
		m_isValidData = false;
    }


	void AlarmSeverityHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

		columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("OPEN_ACKED_FG_COLOUR1");
        columnNames.push_back("OPEN_ACKED_FG_COLOUR2");
        columnNames.push_back("OPEN_ACKED_BG_COLOUR1");
        columnNames.push_back("OPEN_ACKED_BG_COLOUR2");
        columnNames.push_back("OPEN_UNACKED_FG_COLOUR1");
        columnNames.push_back("OPEN_UNACKED_FG_COLOUR2");
        columnNames.push_back("OPEN_UNACKED_BG_COLOUR1");
        columnNames.push_back("OPEN_UNACKED_BG_COLOUR2");
        columnNames.push_back("CLOSED_UNACKED_FG_COLOUR1");
        columnNames.push_back("CLOSED_UNACKED_FG_COLOUR2");
        columnNames.push_back("CLOSED_UNACKED_BG_COLOUR1");
        columnNames.push_back("CLOSED_UNACKED_BG_COLOUR2");
        columnNames.push_back("SEL_OPEN_ACKED_FG_COLOUR1");
        columnNames.push_back("SEL_OPEN_ACKED_FG_COLOUR2");
        columnNames.push_back("SEL_OPEN_ACKED_BG_COLOUR1");
        columnNames.push_back("SEL_OPEN_ACKED_BG_COLOUR2");
        columnNames.push_back("SEL_OPEN_UNACKED_FG_COLOUR1");
        columnNames.push_back("SEL_OPEN_UNACKED_FG_COLOUR2");
        columnNames.push_back("SEL_OPEN_UNACKED_BG_COLOUR1");
        columnNames.push_back("SEL_OPEN_UNACKED_BG_COLOUR2");
        columnNames.push_back("SEL_CLOSED_UNACKED_FG_COLOUR1");
        columnNames.push_back("SEL_CLOSED_UNACKED_FG_COLOUR2");
        columnNames.push_back("SEL_CLOSED_UNACKED_BG_COLOUR1");
        columnNames.push_back("SEL_CLOSED_UNACKED_BG_COLOUR2");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
	}

	void AlarmSeverityHelper::reload()
	{
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmSeverityHelper::reload" );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);

		// create the SQL string to retrieve the data of the Severity
       /* std::ostringstream sql;
        sql << " select PKEY, NAME, "
			<< " OPEN_ACKED_FG_COLOUR1, OPEN_ACKED_FG_COLOUR2, OPEN_ACKED_BG_COLOUR1, OPEN_ACKED_BG_COLOUR2,"
			<< " OPEN_UNACKED_FG_COLOUR1, OPEN_UNACKED_FG_COLOUR2, OPEN_UNACKED_BG_COLOUR1, OPEN_UNACKED_BG_COLOUR2,"
			<< " CLOSED_UNACKED_FG_COLOUR1, CLOSED_UNACKED_FG_COLOUR2, CLOSED_UNACKED_BG_COLOUR1,"
			<< " CLOSED_UNACKED_BG_COLOUR2, SEL_OPEN_ACKED_FG_COLOUR1, SEL_OPEN_ACKED_FG_COLOUR2,"
			<< " SEL_OPEN_ACKED_BG_COLOUR1, SEL_OPEN_ACKED_BG_COLOUR2, SEL_OPEN_UNACKED_FG_COLOUR1,"
			<< " SEL_OPEN_UNACKED_FG_COLOUR2, SEL_OPEN_UNACKED_BG_COLOUR1, SEL_OPEN_UNACKED_BG_COLOUR2,"
			<< " SEL_CLOSED_UNACKED_FG_COLOUR1, SEL_CLOSED_UNACKED_FG_COLOUR2, SEL_CLOSED_UNACKED_BG_COLOUR1,"
			<< " SEL_CLOSED_UNACKED_BG_COLOUR2,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from ALARMSEVERITY where PKEY = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMSEVERITY_Oracle_SELECT_10553, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMSEVERITY_SELECT_10553, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMSEVERITY_SELECT_10553, m_key);

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
			sprintf( reasonMessage, "No severity found for pkey = %lu", m_key );
			TA_THROW( DataException( reasonMessage,DataException::NO_VALUE,"" ) );
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of severity records."); 
        reloadUsing(0, *data);
		
        LOG ( SourceInfo, DebugUtil::FunctionExit, "severity::reload" );
	}
	
    void AlarmSeverityHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

       // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		//TD17849 marc_bugfix
		/*
        m_name = data.getStringData( row, columnNames[0] );
		m_key  = data.getUnsignedLongData( row, columnNames[1] );
		*/
		m_key  = data.getUnsignedLongData( row, columnNames[0] );
        m_name = data.getStringData( row, columnNames[1] );
		m_severityColourKey.clear();
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::OPEN_ACKED_FG1, data.getUnsignedLongData(row, columnNames[2]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::OPEN_ACKED_FG2, data.getUnsignedLongData(row, columnNames[3]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::OPEN_ACKED_BG1, data.getUnsignedLongData(row, columnNames[4]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::OPEN_ACKED_BG2, data.getUnsignedLongData(row, columnNames[5]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::OPEN_UNACKED_FG1, data.getUnsignedLongData(row, columnNames[6]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::OPEN_UNACKED_FG2, data.getUnsignedLongData(row, columnNames[7]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::OPEN_UNACKED_BG1, data.getUnsignedLongData(row, columnNames[8]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::OPEN_UNACKED_BG2, data.getUnsignedLongData(row, columnNames[9]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::CLOSED_UNACKED_FG1, data.getUnsignedLongData(row, columnNames[10]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::CLOSED_UNACKED_FG2, data.getUnsignedLongData(row, columnNames[11]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::CLOSED_UNACKED_BG1, data.getUnsignedLongData(row, columnNames[12]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::CLOSED_UNACKED_BG2, data.getUnsignedLongData(row, columnNames[13]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_OPEN_ACKED_FG1, data.getUnsignedLongData(row, columnNames[14]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_OPEN_ACKED_FG2, data.getUnsignedLongData(row, columnNames[15]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_OPEN_ACKED_BG1, data.getUnsignedLongData(row, columnNames[16]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_OPEN_ACKED_BG2, data.getUnsignedLongData(row, columnNames[17]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_OPEN_UNACKED_FG1, data.getUnsignedLongData(row, columnNames[18]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_OPEN_UNACKED_FG2, data.getUnsignedLongData(row, columnNames[19]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_OPEN_UNACKED_BG1, data.getUnsignedLongData(row, columnNames[20]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_OPEN_UNACKED_BG2, data.getUnsignedLongData(row, columnNames[21]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_CLOSED_UNACKED_FG1, data.getUnsignedLongData(row, columnNames[22]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_CLOSED_UNACKED_FG2, data.getUnsignedLongData(row, columnNames[23]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_CLOSED_UNACKED_BG1, data.getUnsignedLongData(row, columnNames[24]) ) );
        m_severityColourKey.insert(std::map<IAlarmSeverityData::EColourType, unsigned long>::value_type(
                                   IAlarmSeverityData::SELECTED_CLOSED_UNACKED_BG2, data.getUnsignedLongData(row, columnNames[25]) ) );
        m_dateModified = data.getDateData(row,columnNames[26], 0);
        m_dateCreated = data.getDateData( row, columnNames[27]);        

        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }


    void AlarmSeverityHelper::applyChanges()
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
            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "Alarm severity data not fully specified. "
                        "Alarm severity cannot be written to database",
                        fieldNames));
        }
        
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);


        // Create a select statement that will ensure the uniqueness of the entity name.
        //std::string formatSQL = "select PKEY from ALARMSEVERITY where NAME = '"
        //    + databaseConnection->escapeQueryString(m_name) + "'";
        /*std::ostringstream formatSQL;
        formatSQL << "select PKEY from ALARMSEVERITY where NAME = '"
				  << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMSEVERITY_STD_SELECT_10501,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ALARMSEVERITY_SELECT_10501,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ALARMSEVERITY_SELECT_10501,
			databaseConnection->escapeQueryString(m_name));

        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("KEY");

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0, columnNames[0]))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;
				databaseConnection->disconnect();

                std::ostringstream reasonMessage;
			    reasonMessage << "An alarm severity already exists with name = "
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
			reasonMessage << "More than one alarm severity already exists with name = "
                          << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

		// delete the data
        delete data;
        data =  NULL;

		// get the old name from the pKey for updating the event colour table
        /*std::ostringstream nameSQL;
        nameSQL <<"select NAME from ALARMSEVERITY where PKEY = '" << m_key << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMSEVERITY_STD_SELECT_10502, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(ALARMSEVERITY_SELECT_10502, m_key);

		databaseConnection->prepareSQLStatement(strSql, ALARMSEVERITY_SELECT_10502, m_key);

        // Set up the vector to pass to executeQuery
        columnNames.clear();
        std::string nameColumn = "NAME";
        columnNames.push_back(nameColumn);

        // execute the query
        IData* namedata = databaseConnection->executeQuery(strSql,columnNames);
		std::string oldName = namedata->getStringData(0,nameColumn);

		// delete the data and close the connection
        delete namedata;
        namedata =  NULL;
		databaseConnection->disconnect();

		// create the SQL string to write the data of the Severity
        /*std::ostringstream sql;
        sql <<
            "update ALARMSEVERITY " <<
            "set NAME = " << "'" << databaseConnection->escapeInsertString(m_name) << "', " <<
            "OPEN_ACKED_FG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::OPEN_ACKED_FG1] << ", " <<
            "OPEN_ACKED_FG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::OPEN_ACKED_FG2] << ", " <<
            "OPEN_ACKED_BG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::OPEN_ACKED_BG1] << ", " <<
            "OPEN_ACKED_BG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::OPEN_ACKED_BG2] << ", " <<
            "OPEN_UNACKED_FG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::OPEN_UNACKED_FG1] << ", " <<
            "OPEN_UNACKED_FG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::OPEN_UNACKED_FG2] << ", " <<
            "OPEN_UNACKED_BG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::OPEN_UNACKED_BG1] << ", " <<
            "OPEN_UNACKED_BG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::OPEN_UNACKED_BG2] << ", " <<
            "CLOSED_UNACKED_FG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::CLOSED_UNACKED_FG1] << ", " <<
            "CLOSED_UNACKED_FG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::CLOSED_UNACKED_FG2] << ", " <<
            "CLOSED_UNACKED_BG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::CLOSED_UNACKED_BG1] << ", " <<
            "CLOSED_UNACKED_BG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::CLOSED_UNACKED_BG2] << ", " <<
            "SEL_OPEN_ACKED_FG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_ACKED_FG1] << ", " <<
            "SEL_OPEN_ACKED_FG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_ACKED_FG2] << ", " <<
            "SEL_OPEN_ACKED_BG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_ACKED_BG1] << ", " <<
            "SEL_OPEN_ACKED_BG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_ACKED_BG2] << ", " <<
            "SEL_OPEN_UNACKED_FG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_UNACKED_FG1] << ", " <<
            "SEL_OPEN_UNACKED_FG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_UNACKED_FG2] << ", " <<
            "SEL_OPEN_UNACKED_BG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_UNACKED_BG1] << ", " <<
            "SEL_OPEN_UNACKED_BG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_UNACKED_BG2] << ", " <<
            "SEL_CLOSED_UNACKED_FG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_CLOSED_UNACKED_FG1] << ", " <<
            "SEL_CLOSED_UNACKED_FG_COLOUR2 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_CLOSED_UNACKED_FG2] << ", " <<
            "SEL_CLOSED_UNACKED_BG_COLOUR1 = " << m_severityColourKey[IAlarmSeverityData::SELECTED_CLOSED_UNACKED_BG1] << ", " <<
            "SEL_CLOSED_UNACKED_BG_COLOUR2  = " << m_severityColourKey[IAlarmSeverityData::SELECTED_CLOSED_UNACKED_BG2] << " " <<
            "where PKEY = " << m_key;*/

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // get a connection to the database with read-write rights
        IDatabase* databaseConnectionWrite = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMSEVERITY_STD_UPDATE_10651, databaseConnection->escapeInsertString(m_name),
//		strSql  = databaseConnection->prepareSQLStatement(ALARMSEVERITY_UPDATE_10651, databaseConnection->escapeInsertString(m_name),

		databaseConnection->prepareSQLStatement(strSql, ALARMSEVERITY_UPDATE_10651, databaseConnection->escapeInsertString(m_name),
			m_severityColourKey[IAlarmSeverityData::OPEN_ACKED_FG1], m_severityColourKey[IAlarmSeverityData::OPEN_ACKED_FG2], 
			m_severityColourKey[IAlarmSeverityData::OPEN_ACKED_BG1], m_severityColourKey[IAlarmSeverityData::OPEN_ACKED_BG2],
			m_severityColourKey[IAlarmSeverityData::OPEN_UNACKED_FG1], m_severityColourKey[IAlarmSeverityData::OPEN_UNACKED_FG2],
			m_severityColourKey[IAlarmSeverityData::OPEN_UNACKED_BG1], m_severityColourKey[IAlarmSeverityData::OPEN_UNACKED_BG2],
			m_severityColourKey[IAlarmSeverityData::CLOSED_UNACKED_FG1], m_severityColourKey[IAlarmSeverityData::CLOSED_UNACKED_FG2],
			m_severityColourKey[IAlarmSeverityData::CLOSED_UNACKED_BG1], m_severityColourKey[IAlarmSeverityData::CLOSED_UNACKED_BG2],
			m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_ACKED_FG1], m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_ACKED_FG2],
			m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_ACKED_BG1], m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_ACKED_BG2],
			m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_UNACKED_FG1], m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_UNACKED_FG2],
			m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_UNACKED_BG1], m_severityColourKey[IAlarmSeverityData::SELECTED_OPEN_UNACKED_BG2],
			m_severityColourKey[IAlarmSeverityData::SELECTED_CLOSED_UNACKED_FG1], m_severityColourKey[IAlarmSeverityData::SELECTED_CLOSED_UNACKED_FG2],
			m_severityColourKey[IAlarmSeverityData::SELECTED_CLOSED_UNACKED_BG1], m_severityColourKey[IAlarmSeverityData::SELECTED_CLOSED_UNACKED_BG2],
			m_key );
        databaseConnectionWrite->executeModification(strSql);
        
        //now need to update the date fields
        /*sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from ALARMSEVERITY where PKEY = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMSEVERITY_Oracle_SELECT_10554, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(ALARMSEVERITY_SELECT_10554, m_key);

		databaseConnection->prepareSQLStatement(strSql, ALARMSEVERITY_SELECT_10554, m_key);

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
			
			if (oldName.empty() == false)
			{
				//Change the corresponding severity name in event colour table
				std::vector<IEventColourData*> configEventColourData = EventColourAccessFactory::getInstance().getAllEventColours(true);
				std::vector<IEventColourData*>::iterator configECDItr = configEventColourData.begin();
				for ( ; configECDItr != configEventColourData.end(); ++configECDItr)
				{
					IEventColourData* ECData = *configECDItr;
					if (ECData->getName().compare(oldName) == 0)
					{
						ConfigEventColourData* configECData = dynamic_cast<ConfigEventColourData*>(ECData);
						configECData->setName(m_name);
						configECData->applyChanges();
						// remind the EventColourPlugin to reload the changes
						// current implemention will always reload the event colours in the config editor
						// in future, may be improve the config editor framework to allow diff plug-ins to
						// inform each other of data changes
						break;
					}
				}
			}
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

} //close namespace TA_Base_Core
