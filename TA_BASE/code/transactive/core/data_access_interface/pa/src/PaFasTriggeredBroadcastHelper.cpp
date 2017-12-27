/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/PaFasTriggeredBroadcastHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: grace.koh $
  *
  * PaFasTriggeredBroadcastHelper is an object that is held by PaFasTriggeredBroadcast and ConfigPaFasTriggeredBroadcast objects. 
  * It contains all data used by PaFasTriggeredBroadcastGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaFasTriggeredBroadcast and ConfigPaFasTriggeredBroadcast.
  * Additionally, it can be used to query and update the PA_ZONE_GROUP_HELPER database table,
  * as there is no need to have a separate object based database interface for this table
  *
  */

#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcastHelper.h"
#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcastAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    static const std::string PA_FAS_TRIGGERED_BROADCAST_TABLE_NAME = "PA_FAS_TRIGGERED_BROADCAST";
    static const std::string PA_FAS_TRIGGERED_BROADCAST_TABLE_SHORT_NAME = "PAFASBR";
    static const std::string KEY_COL = "PAFASBR_ID";
    static const std::string PA_FAS_TRIGGERED_BROADCAST_SEQUENCE_COUNTER = "PAFASBR_SEQ";
    
    static const std::string FASALARMENTITYKEY_COL   = "FAS_ALARM_ENTITY_KEY";
    static const std::string LOCATIONKEY_COL    = "LOCATIONKEY";
    static const std::string PACYCLICTIMES_COL     = "PA_CYCLIC_TIMES";
    static const std::string PAZONEGROUP_COL    = "PAZGRO_ID";
    static const std::string PADVAMESSAGE_COL   = "PADMES_ID";
    static const std::string ISENABLED_COL      = "IS_ENABLED";

    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";

	PaFasTriggeredBroadcastHelper::PaFasTriggeredBroadcastHelper( const PaFasTriggeredBroadcastHelper& thePaFasTriggeredBroadcastHelper)
		: m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
          m_fasAlarmEntityKey(thePaFasTriggeredBroadcastHelper.m_fasAlarmEntityKey),
		  m_locationKey(thePaFasTriggeredBroadcastHelper.m_locationKey), 
          m_paCyclicTimes(thePaFasTriggeredBroadcastHelper.m_paCyclicTimes),
		  m_paZoneGroupKey(thePaFasTriggeredBroadcastHelper.m_paZoneGroupKey),
		  m_paDvaMessageKey(thePaFasTriggeredBroadcastHelper.m_paDvaMessageKey),
		  m_isEnabledParameter(thePaFasTriggeredBroadcastHelper.m_isEnabledParameter),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    PaFasTriggeredBroadcastHelper::PaFasTriggeredBroadcastHelper(const unsigned long key)
        : m_key(key),		  
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    PaFasTriggeredBroadcastHelper::PaFasTriggeredBroadcastHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),		  
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        reloadUsing(row, data);
    }


    PaFasTriggeredBroadcastHelper::PaFasTriggeredBroadcastHelper()
        : m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
          m_fasAlarmEntityKey(),
          m_locationKey(), 
          m_paCyclicTimes( 0 ),
          m_paZoneGroupKey(),
          m_paDvaMessageKey(),
          m_isEnabledParameter( false ),
		  m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    PaFasTriggeredBroadcastHelper::~PaFasTriggeredBroadcastHelper()
    {
    }
  

    unsigned long PaFasTriggeredBroadcastHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }


    unsigned long PaFasTriggeredBroadcastHelper::getFasAlarmEntityKey()
    {
        if (isNew() && !m_fasAlarmEntityKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_fasAlarmEntityKey);
    }


    unsigned long PaFasTriggeredBroadcastHelper::getLocationKey()
    {
        if (isNew() && !m_locationKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_locationKey);
    }


    unsigned long PaFasTriggeredBroadcastHelper::getPaCyclicTimes()
    {
        if (isNew() && !m_paCyclicTimes.hasBeenSet())
        {
            // Return default value 0
            return 0;                
        }

        return fetchValueFrom(m_paCyclicTimes);
    }


    unsigned long PaFasTriggeredBroadcastHelper::getPaZoneGroupKey()
    {
        if (isNew() && !m_paZoneGroupKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_paZoneGroupKey);
    }


    unsigned long PaFasTriggeredBroadcastHelper::getPaDvaMessageKey()
    {
        if (isNew() && !m_paDvaMessageKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_paDvaMessageKey);
    }


    bool PaFasTriggeredBroadcastHelper::getIsEnabledParameter()
    {
        return fetchValueFrom(m_isEnabledParameter);
    }

    void PaFasTriggeredBroadcastHelper::setFasAlarmEntityKey(unsigned long eventType)
    {
        FUNCTION_ENTRY("setFasAlarmEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_fasAlarmEntityKey.setValue(eventType);
        
        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::setPaCyclicTimes(unsigned long platformId)
    {
        FUNCTION_ENTRY("setPaCyclicTimes" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_paCyclicTimes.setValue(platformId);
        
        FUNCTION_EXIT;
    }

    void PaFasTriggeredBroadcastHelper::setPaZoneGroupKey(unsigned long key)
    {
        FUNCTION_ENTRY("setPaZoneGroupKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_paZoneGroupKey.setValue(key);
        
        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::setPaDvaMessageKey(unsigned long key)
    {
        FUNCTION_ENTRY("setPaDvaMessageKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_paDvaMessageKey.setValue(key);
        
        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::setIsEnabledParameter(bool isEnabled)
    {
        FUNCTION_ENTRY("setIsEnabledParameter" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isEnabledParameter.setValue(isEnabled);
        
        FUNCTION_EXIT;
    }


    time_t PaFasTriggeredBroadcastHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t PaFasTriggeredBroadcastHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }



    void PaFasTriggeredBroadcastHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaFasTriggeredBroadcast");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    const DatabaseQueryHelper::QueryData 
        PaFasTriggeredBroadcastHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_FAS_TRIGGERED_BROADCAST_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(FASALARMENTITYKEY_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);
        result.columnNames.push_back(PACYCLICTIMES_COL);
        result.columnNames.push_back(PAZONEGROUP_COL);
        result.columnNames.push_back(PADVAMESSAGE_COL);
        result.columnNames.push_back(ISENABLED_COL);

        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

        DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement, result.columnNames, 
                                                        result.tableName);
        return result;
    }


    void PaFasTriggeredBroadcastHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing" );
       
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_fasAlarmEntityKey.setValue(data.getUnsignedLongData( row, FASALARMENTITYKEY_COL));
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
        m_paCyclicTimes.setValue(data.getUnsignedLongData( row, PACYCLICTIMES_COL ));
        m_paZoneGroupKey.setValue(data.getUnsignedLongData( row, PAZONEGROUP_COL ));
        m_paDvaMessageKey.setValue(data.getUnsignedLongData( row, PADVAMESSAGE_COL ));
        m_isEnabledParameter.setValue(data.getBooleanData( row, ISENABLED_COL ));
        
        m_dateCreated = data.getDateData( row, DATECREATED_COL );

        try
        {
            m_dateModified = data.getDateData( row, DATEMODIFIED_COL );
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::writePaFasTriggeredBroadcastData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaFasTriggeredBroadcastData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;

        if (!m_fasAlarmEntityKey.hasBeenSet())
        {
            fieldNames.push_back(FASALARMENTITYKEY_COL);
        }
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        if (!m_paCyclicTimes.hasBeenSet())
        {
            fieldNames.push_back(PACYCLICTIMES_COL);
        }
        if (!m_paDvaMessageKey.hasBeenSet())
        {
            fieldNames.push_back(PADVAMESSAGE_COL);
        }
        if (!m_isEnabledParameter.hasBeenSet())
        {
            fieldNames.push_back(ISENABLED_COL);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("PaFasTriggeredBroadcast data not fully specified. PaFasTriggeredBroadcast cannot be written to database",
                                                         fieldNames));
        }

        // Write the PaFasTriggeredBroadcast to the database

        try 
        {
            // Need to check if this is a new PaFasTriggeredBroadcast or an existing PaFasTriggeredBroadcast - 
            // as a new PaFasTriggeredBroadcast will be inserted, while an existing PaFasTriggeredBroadcast will be updated
            if (m_isNew) // This is a new PaFasTriggeredBroadcast
            {
                addNewPaFasTriggeredBroadcast();
            }
            else // This is an existing PaFasTriggeredBroadcast
            {
                modifyExistingPaFasTriggeredBroadcast();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaFasTriggeredBroadcast data not fully specified. PaFasTriggeredBroadcast cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::deletePaFasTriggeredBroadcast()
    {
        FUNCTION_ENTRY("deletePaFasTriggeredBroadcast");

        TA_ASSERT(!m_isNew, "This PaFasTriggeredBroadcast does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
       /* std::ostringstream sql;   */     

        // Good to go...
        /*sql.str("");
        sql << "delete " << PA_FAS_TRIGGERED_BROADCAST_TABLE_NAME 
            << " where " << KEY_COL << " = " << m_key;*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PA_FAS_TRIGGERED_BROADCAST_DELETE_108002, m_key);

        databaseConnection->executeModification(sql);

        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::modifyExistingPaFasTriggeredBroadcast() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaFasTriggeredBroadcast");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA ATS Triggered Broadcast %u already exists. It's data will be updated.", m_key);

        validateLocationKeyExistence();
        validateFasAlarmEntityLocationPairUniqueness();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        /*std::ostringstream sql;

        sql << "update " << PA_FAS_TRIGGERED_BROADCAST_TABLE_NAME 
            << " set " 
            << FASALARMENTITYKEY_COL     << " = " << m_fasAlarmEntityKey.getValue() << ", "
            << LOCATIONKEY_COL      << " = " << m_locationKey.getValue() << ", "
            << PACYCLICTIMES_COL       << " = " << m_paCyclicTimes.getValue() << ", "
            << PAZONEGROUP_COL      << " = " << m_paZoneGroupKey.getValue() << ", "
            << PADVAMESSAGE_COL     << " = " << m_paDvaMessageKey.getValue() << ", "
            << ISENABLED_COL        << " = " << (m_isEnabledParameter.getValue() ? 1 : 0) << " " 
            << "where " << KEY_COL << " = " << m_key;  */

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PA_FAS_TRIGGERED_BROADCAST_UPDATE_108003, m_fasAlarmEntityKey.getValue(),m_locationKey.getValue(),
			m_paCyclicTimes.getValue(), m_paZoneGroupKey.getValue(),m_paDvaMessageKey.getValue(),(m_isEnabledParameter.getValue() ? 1 : 0),m_key);


        databaseConnection->executeModification(sql);

        // now need to update the date fields
       /* sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_FAS_TRIGGERED_BROADCAST_TABLE_NAME 
            << " where " << KEY_COL << " = " << m_key;*/

		databaseConnection->prepareSQLStatement(sql, PA_FAS_TRIGGERED_BROADCAST_SELECT_108004, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA ATS Triggered Broadcast record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA ATS Triggered Broadcast key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA ATS Triggered Broadcast key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0, DATECREATED_COL);
            m_dateModified = data->getDateData(0, DATEMODIFIED_COL);
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

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::addNewPaFasTriggeredBroadcast() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPaFasTriggeredBroadcast");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "PA ATS Triggered Broadcast %u is a new PaFasTriggeredBroadcast. It will be added to the database.",m_locationKey.getValue());

        validateLocationKeyExistence();
        validateFasAlarmEntityLocationPairUniqueness();


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        // Get a suitable primary key generated for this new entry
        unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(
                                                                databaseConnection,
                                                                PA_FAS_TRIGGERED_BROADCAST_SEQUENCE_COUNTER);

        /*std::ostringstream sql;
        sql << "insert into " << PA_FAS_TRIGGERED_BROADCAST_TABLE_NAME << " (" 
            << KEY_COL << ", "
            << FASALARMENTITYKEY_COL     << ","
            << LOCATIONKEY_COL      << ","
            << PACYCLICTIMES_COL       << ","
            << PAZONEGROUP_COL      << ","
            << PADVAMESSAGE_COL     << ","
            << ISENABLED_COL        << ") "
            << "values (" << sequenceNumber << ","
            << m_fasAlarmEntityKey.getValue() << ","
            << m_locationKey.getValue() << "," 
            << m_paCyclicTimes.getValue() << "," 
            << m_paZoneGroupKey.getValue() << "," 
            << m_paDvaMessageKey.getValue() << "," 
            << (m_isEnabledParameter.getValue() ? 1 : 0) << ")";*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PA_FAS_TRIGGERED_BROADCAST_INSERT_108005, sequenceNumber, m_fasAlarmEntityKey.getValue(),
			m_locationKey.getValue(),m_paCyclicTimes.getValue(),m_paZoneGroupKey.getValue(),m_paDvaMessageKey.getValue(),(m_isEnabledParameter.getValue() ? 1 : 0));
            
        databaseConnection->executeModification(sql);

        /*sql.str("");
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_FAS_TRIGGERED_BROADCAST_TABLE_NAME << " where "
            << KEY_COL << " = " << sequenceNumber;*/

		databaseConnection->prepareSQLStatement(sql,PA_FAS_TRIGGERED_BROADCAST_SELECT_108006, sequenceNumber);

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA ATS Triggered Broadcast record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA ATS Triggered Broadcast key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA ATS Triggered Broadcast key" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,KEY_COL);
            m_dateCreated = data->getDateData(0,DATECREATED_COL);
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

        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastHelper::validateLocationKeyExistence() //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First check that the LOCATIONKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        
        /*std::ostringstream sql;
        sql << "select PKEY from LOCATION where PKEY = " << m_locationKey.getValue();*/
        
        std::vector<std::string> columnNames;
        columnNames.clear();
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7003, m_locationKey.getValue());

        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // Location key not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Location key = " << m_locationKey.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Location key" ) );
        }

        delete data;
        data = NULL;
    }

    template <class Type>
    Type PaFasTriggeredBroadcastHelper::fetchValueFrom(const TA_Base_Core::PrimitiveWrapper<Type>& inputRef)
    {    
        FUNCTION_ENTRY("fetchValueFrom");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT

        // Following the reload of data, the input reference data should be up to date
        try
        {
            return inputRef.getValue();
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "internal value not set after reload");
            throw;
        }
    }

    void PaFasTriggeredBroadcastHelper::validateFasAlarmEntityLocationPairUniqueness()
    {
       /* std::ostringstream sql;
        sql << "select " << FASALARMENTITYKEY_COL << " from "
            << PA_FAS_TRIGGERED_BROADCAST_TABLE_NAME << " where "
            << FASALARMENTITYKEY_COL << " = " << m_fasAlarmEntityKey.getValue() << " and "
            << LOCATIONKEY_COL << " = " << m_locationKey.getValue() << " and "
            << KEY_COL << " <> " << m_key;*/

        std::vector<std::string> columnNames;
        columnNames.push_back(FASALARMENTITYKEY_COL);

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql,PA_FAS_TRIGGERED_BROADCAST_SELECT_108007,m_fasAlarmEntityKey.getValue(),
			m_locationKey.getValue(), m_key);

        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

        unsigned long ulRowCount = data->getNumRows();
        delete data;
        data = NULL;

        if (0 != ulRowCount) 
        {
            // Duplicated Fas Alarm entity
            std::ostringstream reasonMessage;
            reasonMessage << "Duplicated Fas Alarm entity for entity key = " << m_fasAlarmEntityKey.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE, "»ðÔÖ¾¯±¨µã" ) );
        }
    }
} // closes TA_Base_Core

