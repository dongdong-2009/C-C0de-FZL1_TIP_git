/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaAtsTriggeredBroadcastHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaAtsTriggeredBroadcastHelper is an object that is held by PaAtsTriggeredBroadcast and ConfigPaAtsTriggeredBroadcast objects. 
  * It contains all data used by PaAtsTriggeredBroadcastGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaAtsTriggeredBroadcast and ConfigPaAtsTriggeredBroadcast.
  * Additionally, it can be used to query and update the PA_ZONE_GROUP_HELPER database table,
  * as there is no need to have a separate object based database interface for this table
  *
  */

#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcastHelper.h"
#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcastAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    static const std::string PA_ATS_TRIGGERED_BROADCAST_TABLE_NAME = "PA_ATS_TRIGGERED_BROADCAST";
    static const std::string PA_ATS_TRIGGERED_BROADCAST_TABLE_SHORT_NAME = "PAATBR";
    static const std::string KEY_COL = "PAATBR_ID";
    static const std::string PA_ATS_TRIGGERED_BROADCAST_SEQUENCE_COUNTER = "PAATBR_SEQ";
    
    static const std::string ATSEVENTTYPE_COL   = "ATS_EVENT_TYPE";
    static const std::string LOCATIONKEY_COL    = "LOCATIONKEY";
    static const std::string DESTLOCATIONKEY_COL= "DEST_LOCATIONKEY";
    static const std::string PLATFORMID_COL     = "PLATFORM_ID";
    static const std::string ISLASTTRAIN_COL    = "IS_LAST_TRAIN";
    static const std::string PAZONEGROUP_COL    = "PAZGRO_ID";
    static const std::string PADVAMESSAGE_COL   = "PADMES_ID";
    static const std::string ISENABLED_COL      = "IS_ENABLED";
    static const std::string DVAMESSAGEVERSION_COL = "DVA_MESSAGE_VERSION";
    

    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";

	PaAtsTriggeredBroadcastHelper::PaAtsTriggeredBroadcastHelper( const PaAtsTriggeredBroadcastHelper& thePaAtsTriggeredBroadcastHelper)
		: m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
          m_atsEventType(thePaAtsTriggeredBroadcastHelper.m_atsEventType),
		  m_locationKey(thePaAtsTriggeredBroadcastHelper.m_locationKey),                    
          m_destinationLocationKey(thePaAtsTriggeredBroadcastHelper.m_destinationLocationKey),
          m_platformId(thePaAtsTriggeredBroadcastHelper.m_platformId),
		  m_isLastTrain(thePaAtsTriggeredBroadcastHelper.m_isLastTrain),
		  m_paZoneGroupKey(thePaAtsTriggeredBroadcastHelper.m_paZoneGroupKey),
		  m_paDvaMessageKey(thePaAtsTriggeredBroadcastHelper.m_paDvaMessageKey),
		  m_isEnabledParameter(thePaAtsTriggeredBroadcastHelper.m_isEnabledParameter),		           
          m_dvaMessageVersion(thePaAtsTriggeredBroadcastHelper.m_dvaMessageVersion),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    PaAtsTriggeredBroadcastHelper::PaAtsTriggeredBroadcastHelper(const unsigned long key)
        : m_key(key),		  
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    PaAtsTriggeredBroadcastHelper::PaAtsTriggeredBroadcastHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),		  
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        reloadUsing(row, data);
    }


    PaAtsTriggeredBroadcastHelper::PaAtsTriggeredBroadcastHelper()
        : m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true),
          m_isLastTrain(false),
          m_isEnabledParameter(false),
          m_dvaMessageVersion("")
    {
    }


    PaAtsTriggeredBroadcastHelper::~PaAtsTriggeredBroadcastHelper()
    {
    }
  

    unsigned long PaAtsTriggeredBroadcastHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }


    unsigned long PaAtsTriggeredBroadcastHelper::getAtsEventType()
    {
        if (isNew() && !m_atsEventType.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_atsEventType);
    }


    unsigned long PaAtsTriggeredBroadcastHelper::getLocationKey()
    {
        if (isNew() && !m_locationKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_locationKey);
    }


    unsigned long PaAtsTriggeredBroadcastHelper::getDestinationLocationKey()
    {
        if (isNew() && !m_destinationLocationKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_destinationLocationKey);
    }


    unsigned long PaAtsTriggeredBroadcastHelper::getPlatformId()
    {
        if (isNew() && !m_platformId.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_platformId);
    }


    bool PaAtsTriggeredBroadcastHelper::getIsLastTrain()
    {
        return fetchValueFrom(m_isLastTrain);
    }


    unsigned long PaAtsTriggeredBroadcastHelper::getPaZoneGroupKey()
    {
        if (isNew() && !m_paZoneGroupKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_paZoneGroupKey);
    }


    unsigned long PaAtsTriggeredBroadcastHelper::getPaDvaMessageKey()
    {
        if (isNew() && !m_paDvaMessageKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return fetchValueFrom(m_paDvaMessageKey);
    }


    bool PaAtsTriggeredBroadcastHelper::getIsEnabledParameter()
    {
        return fetchValueFrom(m_isEnabledParameter);
    }

    
    std::string PaAtsTriggeredBroadcastHelper::getDvaMessageVersion()
    {
        return fetchValueFrom(m_dvaMessageVersion);
    }


    void PaAtsTriggeredBroadcastHelper::setAtsEventType(unsigned long eventType)
    {
        FUNCTION_ENTRY("setAtsEventType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_atsEventType.setValue(eventType);
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::setDestinationLocationKey(unsigned long key)
    {
        FUNCTION_ENTRY("setDestinationLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_destinationLocationKey.setValue(key);
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::setPlatformId(unsigned long platformId)
    {
        FUNCTION_ENTRY("setPlatformId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_platformId.setValue(platformId);
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::setIsLastTrain(bool isEnabled)
    {
        FUNCTION_ENTRY("setIsLastTrain" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isLastTrain.setValue(isEnabled);
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::setPaZoneGroupKey(unsigned long key)
    {
        FUNCTION_ENTRY("setPaZoneGroupKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_paZoneGroupKey.setValue(key);
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::setPaDvaMessageKey(unsigned long key)
    {
        FUNCTION_ENTRY("setPaDvaMessageKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_paDvaMessageKey.setValue(key);
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::setIsEnabledParameter(bool isEnabled)
    {
        FUNCTION_ENTRY("setIsEnabledParameter" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isEnabledParameter.setValue(isEnabled);
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::setDvaMessageVersion(const std::string& version)
    {
        FUNCTION_ENTRY("setDvaMessageVersion(" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_dvaMessageVersion.setValue(version);
        
        FUNCTION_EXIT;
    }


    time_t PaAtsTriggeredBroadcastHelper::getDateCreated()
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


    time_t PaAtsTriggeredBroadcastHelper::getDateModified()
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



    void PaAtsTriggeredBroadcastHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaAtsTriggeredBroadcast");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    DatabaseQueryHelper::QueryData 
        PaAtsTriggeredBroadcastHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_ATS_TRIGGERED_BROADCAST_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(ATSEVENTTYPE_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);
        result.columnNames.push_back(DESTLOCATIONKEY_COL);
        result.columnNames.push_back(PLATFORMID_COL);
        result.columnNames.push_back(ISLASTTRAIN_COL);
        result.columnNames.push_back(PAZONEGROUP_COL);
        result.columnNames.push_back(PADVAMESSAGE_COL);
        result.columnNames.push_back(ISENABLED_COL);
        result.columnNames.push_back(DVAMESSAGEVERSION_COL);

        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

        /*result.statement = 
            DatabaseQueryHelper::createBasicSelectStatement(result.columnNames, 
                                                            result.tableName);*/
		
		DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement, result.columnNames, 
			result.tableName);
        return result;
    }


    void PaAtsTriggeredBroadcastHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing" );
       
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_atsEventType.setValue(data.getUnsignedLongData( row, ATSEVENTTYPE_COL));
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
        m_destinationLocationKey.setValue(data.getUnsignedLongData( row,  DESTLOCATIONKEY_COL));
        m_platformId.setValue(data.getUnsignedLongData( row, PLATFORMID_COL ));
        m_isLastTrain.setValue(data.getBooleanData( row, ISLASTTRAIN_COL ));
        m_paZoneGroupKey.setValue(data.getUnsignedLongData( row, PAZONEGROUP_COL ));
        m_paDvaMessageKey.setValue(data.getUnsignedLongData( row, PADVAMESSAGE_COL ));
        m_isEnabledParameter.setValue(data.getBooleanData( row, ISENABLED_COL ));       
        m_dvaMessageVersion.setValue(data.getStringData( row, DVAMESSAGEVERSION_COL ));
        
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


    void PaAtsTriggeredBroadcastHelper::writePaAtsTriggeredBroadcastData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaAtsTriggeredBroadcastData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;

        if (!m_atsEventType.hasBeenSet())
        {
            fieldNames.push_back(ATSEVENTTYPE_COL);
        }
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        if (!m_destinationLocationKey.hasBeenSet())
        {
            fieldNames.push_back(DESTLOCATIONKEY_COL);
        }
        if (!m_platformId.hasBeenSet())
        {
            fieldNames.push_back(PLATFORMID_COL);
        }
        if (!m_isLastTrain.hasBeenSet())
        {
            fieldNames.push_back(ISLASTTRAIN_COL);
        }
        if (!m_paZoneGroupKey.hasBeenSet())
        {
            fieldNames.push_back(PAZONEGROUP_COL);
        }
        if (!m_paDvaMessageKey.hasBeenSet())
        {
            fieldNames.push_back(PADVAMESSAGE_COL);
        }
        if (!m_isEnabledParameter.hasBeenSet())
        {
            fieldNames.push_back(ISENABLED_COL);
        }
        if (!m_dvaMessageVersion.hasBeenSet() ||
                m_dvaMessageVersion.getValue().length() == 0)
        {
            fieldNames.push_back(DVAMESSAGEVERSION_COL);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("PaAtsTriggeredBroadcast data not fully specified. PaAtsTriggeredBroadcast cannot be written to database",
                                                         fieldNames));
        }

        // Write the PaAtsTriggeredBroadcast to the database

        try 
        {
            // Need to check if this is a new PaAtsTriggeredBroadcast or an existing PaAtsTriggeredBroadcast - 
            // as a new PaAtsTriggeredBroadcast will be inserted, while an existing PaAtsTriggeredBroadcast will be updated
            if (m_isNew) // This is a new PaAtsTriggeredBroadcast
            {
                addNewPaAtsTriggeredBroadcast();
            }
            else // This is an existing PaAtsTriggeredBroadcast
            {
                modifyExistingPaAtsTriggeredBroadcast();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaAtsTriggeredBroadcast data not fully specified. PaAtsTriggeredBroadcast cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::deletePaAtsTriggeredBroadcast()
    {
        FUNCTION_ENTRY("deletePaAtsTriggeredBroadcast");

        TA_ASSERT(!m_isNew, "This PaAtsTriggeredBroadcast does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        //std::ostringstream sql;        

        // Good to go...
       /* sql.str("");
        sql << "delete " << PA_ATS_TRIGGERED_BROADCAST_TABLE_NAME 
            << " where " << KEY_COL << " = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_ATS_TRIGGERED_BROADCAST_STD_DELETE_45401,
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ATS_TRIGGERED_BROADCAST_DELETE_45401,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ATS_TRIGGERED_BROADCAST_DELETE_45401,
			m_key);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastHelper::modifyExistingPaAtsTriggeredBroadcast() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaAtsTriggeredBroadcast");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA ATS Triggered Broadcast %u already exists. It's data will be updated.", m_key);

        validateLocationKeyExistence();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        //std::ostringstream sql;

        const std::string dvaMessageVersionStr = 
            (databaseConnection->escapeInsertString(m_dvaMessageVersion.getValue()));

      /*  sql << "update " << PA_ATS_TRIGGERED_BROADCAST_TABLE_NAME 
            << " set " 
            << ATSEVENTTYPE_COL     << " = " << m_atsEventType.getValue() << ", "
            << LOCATIONKEY_COL      << " = " << m_locationKey.getValue() << ", "
            << DESTLOCATIONKEY_COL  << " = " << m_destinationLocationKey.getValue() << ", "
            << PLATFORMID_COL       << " = " << m_platformId.getValue() << ", "
            << ISLASTTRAIN_COL      << " = " << (m_isLastTrain.getValue() ? 1 : 0) << ", "
            << PAZONEGROUP_COL      << " = " << m_paZoneGroupKey.getValue() << ", "
            << PADVAMESSAGE_COL     << " = " << m_paDvaMessageKey.getValue() << ", "
            << ISENABLED_COL        << " = " << (m_isEnabledParameter.getValue() ? 1 : 0) << ", "            
            << DVAMESSAGEVERSION_COL<< " = '" << dvaMessageVersionStr.c_str() << "' "
            << "where " << KEY_COL << " = " << m_key;  */ 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_ATS_TRIGGERED_BROADCAST_STD_UPDATE_45151,
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ATS_TRIGGERED_BROADCAST_SELECT_45151,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ATS_TRIGGERED_BROADCAST_SELECT_45151,
			m_atsEventType.getValue(), m_locationKey.getValue(), m_destinationLocationKey.getValue(), m_platformId.getValue(),
			(m_isLastTrain.getValue() ? 1 : 0), m_paZoneGroupKey.getValue(), m_paDvaMessageKey.getValue(), 
			(m_isEnabledParameter.getValue() ? 1 : 0), dvaMessageVersionStr, m_key);

        databaseConnection->executeModification(strSql);

        // now need to update the date fields
       /* sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_ATS_TRIGGERED_BROADCAST_TABLE_NAME 
            << " where " << KEY_COL << " = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_ATS_TRIGGERED_BROADCAST_Oracle_SELECT_45051, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(PA_ATS_TRIGGERED_BROADCAST_SELECT_45051, m_key);

		databaseConnection->prepareSQLStatement(strSql, PA_ATS_TRIGGERED_BROADCAST_SELECT_45051, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

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


    void PaAtsTriggeredBroadcastHelper::addNewPaAtsTriggeredBroadcast() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPaAtsTriggeredBroadcast");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "PA ATS Triggered Broadcast %u is a new PaAtsTriggeredBroadcast. It will be added to the database.",m_locationKey.getValue());

        validateLocationKeyExistence();


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        // Get a suitable primary key generated for this new entry
        unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(
                                                                databaseConnection,
                                                                PA_ATS_TRIGGERED_BROADCAST_SEQUENCE_COUNTER);

      /*  std::ostringstream sql;
        sql << "insert into " << PA_ATS_TRIGGERED_BROADCAST_TABLE_NAME << " (" 
            << KEY_COL << ", "
            << ATSEVENTTYPE_COL     << ","
            << LOCATIONKEY_COL      << ","
            << DESTLOCATIONKEY_COL  << ","
            << PLATFORMID_COL       << ","
            << ISLASTTRAIN_COL      << ","
            << PAZONEGROUP_COL      << ","
            << PADVAMESSAGE_COL     << ","
            << ISENABLED_COL        << ","
            << DVAMESSAGEVERSION_COL<< ") "
            << "values (" << sequenceNumber << ","
            << m_atsEventType.getValue() << ","
            << m_locationKey.getValue() << "," 
            << m_destinationLocationKey.getValue() << "," 
            << m_platformId.getValue() << "," 
            << (m_isLastTrain.getValue() ? 1 : 0) << "," 
            << m_paZoneGroupKey.getValue() << "," 
            << m_paDvaMessageKey.getValue() << "," 
            << (m_isEnabledParameter.getValue() ? 1 : 0) << ", '"
            << (databaseConnection->escapeInsertString(m_dvaMessageVersion.getValue())).c_str()
            << "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_ATS_TRIGGERED_BROADCAST_STD_INSERT_45301,
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ATS_TRIGGERED_BROADCAST_INSERT_45301,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ATS_TRIGGERED_BROADCAST_INSERT_45301,
			sequenceNumber, m_atsEventType.getValue(), m_locationKey.getValue(), m_destinationLocationKey.getValue(), m_platformId.getValue(),
			(m_isLastTrain.getValue() ? 1 : 0), m_paZoneGroupKey.getValue(),  m_paDvaMessageKey.getValue(), (m_isEnabledParameter.getValue() ? 1 : 0),
			databaseConnection->escapeInsertString(m_dvaMessageVersion.getValue()));
            
        databaseConnection->executeModification(strSql);

       /* sql.str("");
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_ATS_TRIGGERED_BROADCAST_TABLE_NAME << " where "
            << KEY_COL << " = " << sequenceNumber;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_ATS_TRIGGERED_BROADCAST_Oracle_SELECT_45052, sequenceNumber);
//		strSql  = databaseConnection->prepareSQLStatement(PA_ATS_TRIGGERED_BROADCAST_SELECT_45052, sequenceNumber);

		databaseConnection->prepareSQLStatement(strSql, PA_ATS_TRIGGERED_BROADCAST_SELECT_45052, sequenceNumber);

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

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


    void PaAtsTriggeredBroadcastHelper::validateLocationKeyExistence() //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First check that the LOCATIONKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        
      /*  std::ostringstream sql;
        sql << "select PKEY from LOCATION where PKEY = " << m_locationKey.getValue();*/		
        
        std::vector<std::string> columnNames;
        columnNames.clear();
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_STD_SELECT_7003, m_locationKey.getValue());
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7003, m_locationKey.getValue());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7003, m_locationKey.getValue());
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

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
    Type PaAtsTriggeredBroadcastHelper::fetchValueFrom(const TA_Base_Core::PrimitiveWrapper<Type>& inputRef)
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
            //TA_ASSERT(false, "internal value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "internal value not set after reload");
            throw;
        }
    }
} // closes TA_Base_Core

