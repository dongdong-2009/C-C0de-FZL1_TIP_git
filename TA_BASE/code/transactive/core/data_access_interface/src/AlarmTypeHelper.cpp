 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmTypeHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * AlarmTypeHelper is an object that is held by AlarmTypeData and ConfigAlarmType objects. 
  * It contains all data used by Alarm types, and manipulation
  * methods for the data. It helps avoid re-writing code for both AlarmTypeData and ConfigAlarmType.
  */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/data_access_interface/src/AlarmTypeHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
//Mintao++ TD15302
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const unsigned long AlarmTypeHelper::NEW_ALARM_TYPES_STARTING_PKEY = 55000;

    AlarmTypeHelper::~AlarmTypeHelper() 
    {
        if (m_MmsRule != NULL)
        {
            delete m_MmsRule;
            m_MmsRule = NULL;
        }
    }
	
    AlarmTypeHelper::AlarmTypeHelper(unsigned long key)
        :
        m_isValidData(false),
        m_key(key),
        m_name(""),
		m_displayname(""),
        m_description(""),
        m_severityKey(0),
        m_toBePrinted(false),
		m_isSystemAlarm(true),
        m_toBePersisted(false),
        m_isDeleted(false),
        m_contextName(""),
        m_contextKey(0),
        m_dateCreated(0),
        m_dateModified(0),
		m_alarmValue(""),
		m_normalValue(""),
		m_isVisible(true),
		m_MmsStateType(IAlarmData::TYPE_NONE) //TD17628
    {
        m_MmsRule = NULL;
        m_isMmsRuleDeleted =false;
		m_isPhysicalAlarm = true; /*20449++*/
    }

    AlarmTypeHelper::AlarmTypeHelper(
        unsigned long key,
        const std::string& name,
        const std::string& description,
        unsigned long severityKey,
	    bool toBePrinted,
		bool toBePersisted,
		bool isSystemAlarm,
		bool isVisible)
        :
        m_isValidData(true),
        m_key(key),
        m_name(name),
		m_displayname(""),
        m_description(description),
        m_severityKey(severityKey),
        m_toBePrinted(toBePrinted),
		m_isSystemAlarm(isSystemAlarm),
        m_toBePersisted(toBePersisted),
        m_isDeleted(false),
		m_contextName(""),
        m_contextKey(0),
        m_dateCreated(0),
        m_dateModified(0),
		m_alarmValue(""),
		m_normalValue(""),
		m_isVisible(isVisible),
		m_MmsStateType(IAlarmData::TYPE_NONE) //TD17628
    {
        m_MmsRule = NULL;
        m_isMmsRuleDeleted =false;
		m_isPhysicalAlarm = true; /*20449++*/
    }

    AlarmTypeHelper::AlarmTypeHelper(
        const std::string& name,
        const std::string& description,
        unsigned long severityKey,
	    bool toBePrinted,
		bool toBePersisted,
		bool isSystemAlarm,
		bool isVisible)
        :
        m_isValidData(false),
        m_key(0),
        m_name(name),
		m_displayname(""),
        m_description(description),
        m_severityKey(severityKey),
        m_toBePrinted(toBePrinted),
		m_isSystemAlarm(isSystemAlarm),
        m_toBePersisted(toBePersisted),
        m_isDeleted(false),
		m_contextName(""),
        m_contextKey(0),
        m_dateCreated(0),
        m_dateModified(0),
		m_alarmValue(""),
		m_normalValue(""),
		m_isVisible(isVisible),
		m_MmsStateType(IAlarmData::TYPE_NONE) //TD17628
    {
        // TODO: Remove this line if AlarmTypes may be created
        TA_ASSERT(true, "Currently, new alarm types may not be created");
        m_MmsRule = NULL;
        m_isMmsRuleDeleted =false;
		m_isPhysicalAlarm = true; /*20449++*/
    }


	AlarmTypeHelper::AlarmTypeHelper(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_isDeleted(false), //TD17081 - must be initialized to false or assertions occur
		m_isValidData(false),
		m_MmsStateType(IAlarmData::TYPE_NONE) //TD17628
	{
		FUNCTION_ENTRY("AlarmTypeHelper(const unsigned long, TA_Base_Core::IData&)");        

		//TD17081 - must be initialized to false or assertions occur
		m_MmsRule = NULL; 
		m_isMmsRuleDeleted =false;
		//++TD17081
		m_isPhysicalAlarm = true;

		reloadUsing(row, data);
		FUNCTION_EXIT;
	}


	AlarmTypeHelper::AlarmTypeHelper( const AlarmTypeHelper& theAlarmTypeHelperToCopy)
    {
        m_key           = 0;
        m_name          = theAlarmTypeHelperToCopy.m_name;
		m_displayname   = theAlarmTypeHelperToCopy.m_displayname;
        m_description   = theAlarmTypeHelperToCopy.m_description;
        m_severityKey   = theAlarmTypeHelperToCopy.m_severityKey;
		m_toBePrinted   = theAlarmTypeHelperToCopy.m_toBePrinted;
		m_isSystemAlarm = theAlarmTypeHelperToCopy.m_isSystemAlarm;
		m_toBePersisted = theAlarmTypeHelperToCopy.m_toBePersisted;
        m_isDeleted     = theAlarmTypeHelperToCopy.m_isDeleted;
        m_isValidData   = theAlarmTypeHelperToCopy.m_isValidData;
		m_contextName	= theAlarmTypeHelperToCopy.m_contextName;
        m_contextKey	= theAlarmTypeHelperToCopy.m_contextKey;
        m_dateCreated   = theAlarmTypeHelperToCopy.m_dateCreated;
        m_dateModified  = theAlarmTypeHelperToCopy.m_dateModified;
		m_alarmValue	= theAlarmTypeHelperToCopy.m_alarmValue;
		m_normalValue	= theAlarmTypeHelperToCopy.m_normalValue;
		m_isVisible		= theAlarmTypeHelperToCopy.m_isVisible;
        m_MmsRule       = theAlarmTypeHelperToCopy.m_MmsRule;
        m_isMmsRuleDeleted = theAlarmTypeHelperToCopy.m_isMmsRuleDeleted;
		m_MmsStateType  =  theAlarmTypeHelperToCopy.m_MmsStateType; //TD17628
		m_isPhysicalAlarm = theAlarmTypeHelperToCopy.m_isPhysicalAlarm; /*20449++*/
    }


	unsigned long AlarmTypeHelper::getKey()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        return m_key;
    }


    bool AlarmTypeHelper::isNew()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        // If the key is 0, an entry has not yet been made in the database.
        // This functionality is not currently required.
        //return (m_key == 0);
        return false;
    }


    bool AlarmTypeHelper::canDelete()
    {
        // This functionality is not currently required
        //return (isNew() || m_key >= NEW_ALARM_TYPES_STARTING_PKEY);
        return false;
    }
    
    void AlarmTypeHelper::setName(const std::string& name)
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }

        m_name = name;
    }
    
	void AlarmTypeHelper::setDisplayName(const std::string& displayname)
	{
		TA_ASSERT( !m_isDeleted,
			"Operation attempted after object deleteAlarmType called.");

		if( !m_isValidData && !isNew() )
		{
			reload();
		}
		m_displayname = displayname;
	}
    
    std::string AlarmTypeHelper::getName()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() ) // reload the data if it is flaged as invalid
        {
            reload();
        }
        return m_name;
		
    }

	std::string AlarmTypeHelper::getDisplayName()
	{
		TA_ASSERT( !m_isDeleted,
			"Operation attempted after object deleteAlarmType called.");

		if( !m_isValidData && !isNew() ) // reload the data if it is flaged as invalid
		{
			reload();
		}
		return m_displayname;
	}

    std::string AlarmTypeHelper::getDescription()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_description;
    }

    void AlarmTypeHelper::setDescription(const std::string& description)
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmTypeHelper::setDescription" );
        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        m_description = description;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmTypeHelper::setDescription" );
    }

    unsigned long AlarmTypeHelper::getSeverityKey()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_severityKey;
    }

    std::string AlarmTypeHelper::getSeverityName()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        IAlarmSeverityData* severityData = NULL;

        try
        {
		    // Get the IAlarmSeverityData object
            AlarmSeverityAccessFactory& factory =
                AlarmSeverityAccessFactory::getInstance();
		    severityData = factory.getSeverityByKey( getSeverityKey(), false );
		    std::string name( severityData->getName() );

		    // Clean up
		    delete severityData;
            severityData = NULL;

		    // Return the name
		    return name;
        }
        catch ( ... )
        {
            if (severityData != NULL)
            {
                // Clean up if an exception is thrown
                delete severityData;
            }
            throw;
        }

		return "";
    }


    void AlarmTypeHelper::setSeverityKey(const unsigned long severityKey)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmTypeHelper::setSeverityKey" );
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        
        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        
        m_severityKey = severityKey;
        
        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmTypeHelper::setSeverityKey" );
    }

    unsigned long AlarmTypeHelper::getContextKey()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_contextKey;
    }

    std::string AlarmTypeHelper::getContextName()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_contextName;
    }


    bool AlarmTypeHelper::toBePrinted()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_toBePrinted;
    }

    bool AlarmTypeHelper::isSystemAlarm()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_isSystemAlarm;
    }

	//20449++
    bool AlarmTypeHelper::isPhysicalAlarm()
    {
        TA_ASSERT( !m_isDeleted,
			"Operation attempted after object deleteAlarmType called.");
		
        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_isPhysicalAlarm;
    }
	//++20449
    bool AlarmTypeHelper::isVisible()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_isVisible;
    }

    void AlarmTypeHelper::setPrintAlarmStatus(const bool printAlarmStatus)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmTypeHelper::setPrintAlarmStatus" );
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        m_toBePrinted = printAlarmStatus;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmTypeHelper::setPrintAlarmStatus" );
    }

	void AlarmTypeHelper::setSystemAlarmStatus(const bool systemAlarmStatus)
    {
        FUNCTION_ENTRY("setSystemAlarmStatus" );
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        m_isSystemAlarm = systemAlarmStatus;
        FUNCTION_EXIT;
    }

    bool AlarmTypeHelper::toBePersisted()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_toBePersisted;
    }

	std::string AlarmTypeHelper::getAlarmValue()
	{
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_alarmValue;
	}

	std::string AlarmTypeHelper::getNormalValue()
	{
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_normalValue;
	}

	void AlarmTypeHelper::setAlarmValue(const std::string& alarmValue)
	{
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        m_alarmValue = alarmValue;
	}

	void AlarmTypeHelper::setNormalValue(const std::string& normalValue)
	{
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        m_normalValue = normalValue;
	}

    time_t AlarmTypeHelper::getDateCreated()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_dateCreated;
    }

    time_t AlarmTypeHelper::getDateModified()
    {
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        return m_dateModified;
    }

    void AlarmTypeHelper::setPersistAlarmStatus(const bool persistAlarmStatus)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmTypeHelper::setPersistAlarmStatus" );
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( !m_isValidData && !isNew() )
        {
            reload();
        }
        m_toBePersisted = persistAlarmStatus;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmTypeHelper::setPersistAlarmStatus" );
    }

    void AlarmTypeHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmTypeHelper::invalidate" );

        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        // set the flag to false
        m_isValidData = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmTypeHelper::invalidate" );
    }


    void AlarmTypeHelper::createNewRowInDatabase()
    {
	/*
	This functionality is not currently required.
        FUNCTION_ENTRY("createNewRowInDatabase");
        TA_ASSERT(m_key == 0, "ID must be 0 before createNewRowInDatabase called");

        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");


        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);


        // TODO: Get MEMCON_ID from somewhere
        // Create the SQL to insert the new alarm type.
        std::string escapedName =
            databaseConnection->escapeInsertString(m_name);

        std::string escapedDescription =
            databaseConnection->escapeInsertString(m_description);

        std::ostringstream sql;
        sql <<
            "insert into me_alarm_type " <<
            "(" <<
                "meatyp_id, "        <<
                "type_name, "        <<
                "type_description, " <<
                "measev_id, "        <<
                "print_alarm, "      <<
                "persist_alarm, "    <<
                "memcon_id"          <<
            ") " <<
            "values " <<
            "(" <<
                       "ALARMTYPE_SEQ.NEXTVAL"   << ", "  <<
                "'" << escapedName               << "', " <<
                "'" << escapedDescription        << "', " <<
                       m_severityKey             << ", "  <<
                       m_toBePrinted             << ", "  <<
                       m_toBePersisted           << ", "  <<
                       "15" << // TODO: fix this
            ")";


        // execute the insert
        databaseConnection->executeModification(sql.str());



        // Get the pkey that was allocated.
        std::ostringstream getIdSql;
        getIdSql <<
            "select meatyp_id from me_alarm_type "
            "where name = '" << escapedName << "'";


        // execute the query to get the pkey
        std::string keyColumn = "meatyp_id";
        std::vector< std::string > columnNames;
        columnNames.push_back(keyColumn);
        IData* data = databaseConnection->executeQuery(sql.str(), columnNames);


        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,keyColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;


        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;

        FUNCTION_EXIT;
	*/
    }

	void AlarmTypeHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		columnNames.clear();

		columnNames.push_back("MEATYP_ID");
        columnNames.push_back("NAME");
		columnNames.push_back("DISPLAY_NAME");
        columnNames.push_back("MEMCONID");
        columnNames.push_back("CONTEXTNAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("SEVERITY");
        columnNames.push_back("PRINTALARM");
        columnNames.push_back("SYSTEM_ALARM");
		columnNames.push_back("ISVISIBLE");
        columnNames.push_back("PERSISTALARM");
        columnNames.push_back("ALARM_VALUE");
        columnNames.push_back("NORMAL_VALUE");
		//TD17628 marc_bugfix begin support mms state
		columnNames.push_back("MMS_STATE");
        columnNames.push_back("DATEMODIFIED");
        columnNames.push_back("DATECREATED");
		columnNames.push_back("PHYSICAL_ALARM"); //20449++
	}


    void AlarmTypeHelper::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmTypeHelper::reload" );
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);

        // create the SQL string to retrieve the data of the alarm type
        // based upon the key
        //
        // Note the use of the nvl command on the description column - this is used because
        // the description column can be null, but that will cause SimpleDb to have a heart-
        // attack. It can be removed when SimpleDb is no longer being used.
        //std::ostringstream sql;
		//TD17628 marc_bugfix support mms state
       /* sql << "select a.meatyp_id, a.type_name, a.memcon_id, mc.context_name, nvl(a.type_description,'NULL') as type_description, a.measev_id, a.print_alarm, a.system_alarm, a.isvisible, "
            << "a.persist_alarm, a.alarm_value, a.normal_value, a.mms_state, TO_CHAR(a.date_modified,'YYYYMMDDHH24MISS'), TO_CHAR(a.date_created,'YYYYMMDDHH24MISS'), a.physical_alarm from "
            << "me_alarm_type a, me_message_context mc where a.memcon_id = mc.memcon_id and a.meatyp_id = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_alarm_type_Oracle_SELECT_1555, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ME_ALARM_TYPE_SELECT_1555, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ME_ALARM_TYPE_SELECT_1555, m_key);
        
        std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            char reasonAlarm[256] = {0};
			sprintf(reasonAlarm, "No alarm type found for meatyp_id = %u",m_key);
            TA_THROW(DataException(reasonAlarm,DataException::NO_VALUE,""));
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of AlarmType records."); 
        reloadUsing(0, *data);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmTypeHelper::reload" );
    }

    void AlarmTypeHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

		std::vector<std::string> columnNames;
		getColumnNames(columnNames);

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_key  = data.getUnsignedLongData(row, columnNames[0]);
        m_name = data.getStringData(row, columnNames[1]);
		m_displayname = data.getStringData(row, columnNames[2]);
        m_contextKey = data.getUnsignedLongData(row, columnNames[3]);
        m_contextName = data.getStringData(row, columnNames[4]);
        
		m_description = data.getStringData(row, columnNames[5]);
		m_description = ("NULL" == m_description) ? "" : m_description;

        m_severityKey = data.getUnsignedLongData(row, columnNames[6]);
        m_toBePrinted = (1 == data.getIntegerData(row, columnNames[7])) ? true : false;
		m_isSystemAlarm = data.getBooleanData(row, columnNames[8]);
		
		// Set the isVisible indicator
		m_isVisible = (data.getIntegerData(row, columnNames[9]) == 0)? false : true;
        m_toBePersisted = (1 == data.getIntegerData(row, columnNames[10])) ? true : false;
		m_alarmValue  = data.getStringData(row, columnNames[11]);
		m_normalValue = data.getStringData(row, columnNames[12]);
		//TD17628 marc_bugfix support mms_state
		m_MmsStateType = (IAlarmData::EMmsState)data.getIntegerData(row, columnNames[13]);
        m_dateModified = data.getDateData(row, columnNames[14], 0);
        m_dateCreated = data.getDateData(row, columnNames[15]);
		m_isPhysicalAlarm = data.getBooleanData(row, columnNames[16]); //20449++
        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }	

    void AlarmTypeHelper::writeAlarmTypeData()
    {       
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmTypeHelper::writeAlarmTypeData" );     
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if ( m_name.empty() )
        {
            std::vector<std::string> fieldNames;
            fieldNames.push_back("type_name");
            TA_THROW(TA_Base_Core::DataConfigurationException(
				"Alarm type data not fully specified. Alarm type cannot be written to database", fieldNames));
        }

        // Write the Alarm Type to the database

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        
        // Always perform an update, as the Alarm Type will always exist.
        
        // Create a select statement that will ensure the uniqueness of the Alarm type name.
        /*std::string formatSQL = "select meatyp_id from me_alarm_type where type_name = '"
            + databaseConnection->escapeQueryString(m_name) + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_alarm_type_STD_SELECT_1501,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ME_ALARM_TYPE_SELECT_1501,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ME_ALARM_TYPE_SELECT_1501,
			databaseConnection->escapeQueryString(m_name) );

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
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {   // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                char reasonAlarm[256] = {0};
			    sprintf(reasonAlarm, "An alarm type already exists with name = %s or description = %s",
                    m_name.c_str(),m_description.c_str());			
                throw DataException(reasonAlarm,DataException::NOT_UNIQUE,"");
            }
            // else - it matches, so that's okay - it just means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonAlarm[256] = {0};
			sprintf(reasonAlarm, "More than one alarm type exists with name = %s or description = %s",
                m_name.c_str(),m_description.c_str());			
            throw DataException(reasonAlarm,DataException::NOT_UNIQUE,"");
        }
   //     char sql[1024];
   //     sprintf(sql,"update me_alarm_type set type_name = '%s', type_description = '%s', measev_id = %lu, print_alarm = %d, system_alarm = %d, persist_alarm = %d, alarm_value = '%s', normal_value = '%s', mms_state = %i where meatyp_id = %u",
   //         databaseConnection->escapeInsertString(m_name).c_str(),
   //         databaseConnection->escapeInsertString(m_description).c_str(),
   //         m_severityKey,m_toBePrinted,m_isSystemAlarm,m_toBePersisted,
			//databaseConnection->escapeInsertString(m_alarmValue).c_str(),
			//databaseConnection->escapeInsertString(m_normalValue).c_str(),
			////TD17628 marc_bugfix support mms state 
			//(int)m_MmsStateType,
			//m_key);
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_alarm_type_STD_UPDATE_1651, databaseConnection->escapeInsertString(m_name),
//		strSql  = databaseConnection->prepareSQLStatement(ME_ALARM_TYPE_UPDATE_1651, databaseConnection->escapeInsertString(m_name),

		databaseConnection->prepareSQLStatement(strSql, ME_ALARM_TYPE_UPDATE_1651, databaseConnection->escapeInsertString(m_name),
			databaseConnection->escapeInsertString(m_description), m_severityKey,m_toBePrinted,m_isSystemAlarm,m_toBePersisted, 
			databaseConnection->escapeInsertString(m_alarmValue), databaseConnection->escapeInsertString(m_normalValue), (int)m_MmsStateType, m_key);
        databaseConnection->executeModification(strSql);
        // nothing more to be done here
        
        // Invalidate the data so that all date information is refreshed.
        m_isValidData = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmTypeHelper::writeAlarmTypeData" );
    }

    void AlarmTypeHelper::deleteAlarmType()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmTypeHelper::deleteAlarmType" );
        TA_ASSERT( !m_isDeleted,
                  "Operation attempted after object deleteAlarmType called.");

        if( ! canDelete() )
        {
            const char* reason = "Deleting system alarms is not permitted.";
            TA_THROW( DataException(reason, DataException::CANNOT_DELETE, "") );
        }
                    
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Write);
        
        /*char sql[128];
        sprintf(sql,"delete from me_alarm_type where meatyp_id = %u", m_key);*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), me_alarm_type_STD_DELETE_1901,m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ME_ALARM_TYPE_DELETE_1901,m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ME_ALARM_TYPE_DELETE_1901,m_key);
        databaseConnection->executeModification(strSql);

        m_key = 0;
        m_name = "";
		m_displayname = "";
        m_description = "";
        m_severityKey = 10;
        m_toBePrinted = false;
		m_isSystemAlarm = true;
        m_toBePersisted = false;
		m_alarmValue = "";
		m_normalValue = "";
        m_dateCreated = 0;
        m_dateModified = 0;
        m_isValidData = false;
		//TD17628 marc_bugfix support for mms state
		m_MmsStateType = IAlarmData::TYPE_NONE;

        m_isDeleted = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmTypeHelper::deleteAlarmType" );
    }
	
    //Mintao++ TD15302
    void AlarmTypeHelper::createMMSRule(IAlarmData::EMmsState mmsStateType)
    {
        unsigned long entitykey = 0;
        unsigned long entitytypekey = 0;
        std::string ruleTrigger = "submit";
        m_MmsRule = AlarmRuleAccessFactory::getInstance().createMMSRule( entitykey, entitytypekey, m_key, ruleTrigger);  
        m_isMmsRuleDeleted = false;
		m_MmsStateType = mmsStateType; //TD17628
    }

    void AlarmTypeHelper::deleteMMSRule()
    {
        //set the flag to indicate the Mms Rule is deleted
        m_isMmsRuleDeleted = true;
		m_MmsStateType = IAlarmData::TYPE_NONE; //TD17628
    }

    bool AlarmTypeHelper::isMMSRuleDeleted() 
    {
        return m_isMmsRuleDeleted;
    }

    bool AlarmTypeHelper::isMMSEnabled()
    {
		//TD17628 marc_bugfix use a temp variable
		//The purpose of m_MmsRule is different (although unclear)
		MMSRuleData* tmpmmsRule = NULL;
		bool bRet;
        tmpmmsRule = AlarmRuleAccessFactory::getInstance().getMMSRuleForAlarmTypeKey(m_key);

        if (tmpmmsRule == NULL)
        {
            bRet =  false;
        }
        else
        {
            bRet =  true;
        }
		
		delete tmpmmsRule;

		return bRet;
	
    }

    MMSRuleData* AlarmTypeHelper::getRelatedMMSRule()  
    {
        return m_MmsRule;
    }
    //Mintao++ TD15302

	//TD17628 marc_bugfix support for mms state
	IAlarmData::EMmsState AlarmTypeHelper::getRelatedMMSType() 
	{
		return m_MmsStateType;
	}

	void AlarmTypeHelper::setRelatedMMSType(IAlarmData::EMmsState mmsState)
	{
		m_MmsStateType = mmsState;
	}

} // closes TA_Base_Core
