/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaZoneGroupHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PaZoneGroupHelper is an object that is held by PaZoneGroup and ConfigPaZoneGroup objects. 
  * It contains all data used by PaZoneGroupGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaZoneGroup and ConfigPaZoneGroup.
  * Additionally, it can be used to query and update the PA_ZONE_GROUP_HELPER database table,
  * as there is no need to have a separate object based database interface for this table
  */

#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <list>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaZoneGroupHelper.h"
#include "core/data_access_interface/pa/src/PaZoneGroupAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"

//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    static const std::string PA_ZONE_GROUP_TABLE_NAME = "PA_ZONE_GROUP";
    static const std::string PA_ZONE_GROUP_TABLE_SHORT_NAME = "PAZGRO";
    static const std::string PA_ZONE_GROUP_SEQUENCE_COUNTER = "PAZGRO_SEQ";

    static const std::string PA_ZONE_GROUP_HELPER_TABLE_NAME = "PA_ZONE_GROUP_HELPER";
    static const std::string PA_ZONE_GROUP_HELPER_KEY_COL = "PAZGHE_ID";
    static const std::string PA_ZGH_GROUPKEY_COL = "PAZGRO_ID";
    static const std::string PA_ZGH_ZONEKEY_COL = "PAZONE_ID";
    static const std::string PA_ZONE_GROUP_HELPER_SEQUENCE_COUNTER = "PAZGHE_SEQ";

    const std::string PaZoneGroupHelper::KEY_COL = "PAZGRO_ID";
    const std::string PaZoneGroupHelper::LOCATIONKEY_COL = "LOCATIONKEY";
    const std::string PaZoneGroupHelper::LABEL_COL = "LABEL";
    const std::string PaZoneGroupHelper::ISEVENTGROUP_COL = "IS_EVENT_GROUP";
    const std::string PaZoneGroupHelper::DATEMODIFIED_COL = "DATE_MODIFIED";
    const std::string PaZoneGroupHelper::DATECREATED_COL = "DATE_CREATED";

    std::string PaZoneGroupHelper::m_localDatabase = "";

	PaZoneGroupHelper::PaZoneGroupHelper( const PaZoneGroupHelper& thePaZoneGroupHelper)
		: m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_locationKey(thePaZoneGroupHelper.m_locationKey),          
          m_label(thePaZoneGroupHelper.m_label),
          m_isEventGroup(thePaZoneGroupHelper.m_isEventGroup),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true),
		  // marvin++ TD8110
		  m_copiedKey(thePaZoneGroupHelper.getKeyDirect()),
		  m_isCopiedKeySet(true)
		  // ++marvin TD8110
    {	  
	}


    PaZoneGroupHelper::PaZoneGroupHelper(const unsigned long key)
        : m_key(key),		  
          m_label(""),
          m_isEventGroup(0),    // Default to 0 to mimic database behaviour
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false),
		  // marvin++ TD8110
		  m_copiedKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_isCopiedKeySet(false)
		  // ++marvin TD8110
    {
    }


    PaZoneGroupHelper::PaZoneGroupHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),		  
          m_label(""),
          m_isEventGroup(0),    // Default to 0 to mimic database behaviour
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false),
		  // marvin++ TD8110
		  m_copiedKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_isCopiedKeySet(false)
		  // ++marvin TD8110
    {
        reloadUsing(row, data);
    }


    PaZoneGroupHelper::PaZoneGroupHelper()
        : m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_label(""),
          m_isEventGroup(0),    // Default to 0 to mimic database behaviour
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true),
		  // marvin++ TD8110
		  m_copiedKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_isCopiedKeySet(false)
		  // ++marvin TD8110
    {
    }


    PaZoneGroupHelper::~PaZoneGroupHelper()
    {
    }
  

    unsigned long PaZoneGroupHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }

    unsigned long PaZoneGroupHelper::getLocationKey()
    {
        FUNCTION_ENTRY("getLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        // Following the reload of data, all values should have been set
        try
        {
            if (isNew() && !m_locationKey.hasBeenSet())
            {
                // 4669FAT TD #782 - if this is a new zone that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }
            return m_locationKey.getValue();
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_locationKey value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_locationKey value not set after reload");
            throw;
        }
    }

    void PaZoneGroupHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }


    std::string PaZoneGroupHelper::getLabel()
    {
        FUNCTION_ENTRY("getLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_label;
    }


    void PaZoneGroupHelper::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_label = label;
        FUNCTION_EXIT;
    }


    unsigned long PaZoneGroupHelper::getIsEventGroup()
    {
        FUNCTION_ENTRY("getIsEventGroup" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        try
        {
            return m_isEventGroup.getValue();
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            //TA_ASSERT(false, "m_isEventGroup value not set after reload");
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "m_isEventGroup value not set after reload");
            throw;
        }
    }


    void PaZoneGroupHelper::setIsEventGroup(const unsigned long isEventGroup)
    {
        FUNCTION_ENTRY("setIsEventGroup" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isEventGroup.setValue(isEventGroup);
        FUNCTION_EXIT;
    }

    time_t PaZoneGroupHelper::getDateCreated()
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


    time_t PaZoneGroupHelper::getDateModified()
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



    void PaZoneGroupHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaZoneGroup");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    DatabaseQueryHelper::QueryData 
        PaZoneGroupHelper::getBasicQueryData(const std::string& joinedTable)
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_ZONE_GROUP_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);
        result.columnNames.push_back(LABEL_COL);
		result.columnNames.push_back(ISEVENTGROUP_COL);
        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

        
        DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement,
														result.columnNames, 
                                                        result.tableName);

        return result;
    }


    void PaZoneGroupHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {               
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
        m_label   = data.getStringData( row, LABEL_COL );
		m_isEventGroup.setValue(data.getUnsignedLongData( row, ISEVENTGROUP_COL ));
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


    void PaZoneGroupHelper::writePaZoneGroupData(bool recreateModified) //throw(TA_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaZoneGroupData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        
        if (m_label.empty())
        {
            fieldNames.push_back(LABEL_COL);
        }
        
        if (!m_isEventGroup.hasBeenSet())
        {
            fieldNames.push_back(ISEVENTGROUP_COL);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("PaZoneGroup data not fully specified. PaZoneGroup cannot be written to database",
                                                         fieldNames));
        }

        validateLabelLocationPairUniqueness();

        // Write the PaZoneGroup to the database

        try 
        {
            // Need to check if this is a new PaZoneGroup or an existing PaZoneGroup - 
            // as a new PaZoneGroup will be inserted, while an existing PaZoneGroup will be updated
            if (m_isNew) // This is a new PaZoneGroup
            {
                addNewPaZoneGroup();
            }
            else // This is an existing PaZoneGroup
            {
                modifyExistingPaZoneGroup(recreateModified);
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaZoneGroup data not fully specified. PaZoneGroup cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PaZoneGroupHelper::validateLabelLocationPairUniqueness()
    {   
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read); 

      /*  std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_ZONE_GROUP_TABLE_NAME 
            << " where " << LOCATIONKEY_COL << " = " << m_locationKey.getValue()
            << " and " << LABEL_COL << " = '" 
            << (databaseConnection->escapeInsertString(m_label)).c_str()
            << "'";*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_SELECT_45502,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_SELECT_45502,
			m_locationKey.getValue(), databaseConnection->escapeInsertString(m_label));

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 != data->getNumRows()) 
        {        
            unsigned long duplicateItemKey = data->getUnsignedLongData(0, KEY_COL);

            // Ignore ourself..
            if (duplicateItemKey != m_key)
            {
                std::ostringstream str;
                str << "Attempting to write Pa Zone Group record that doesn't "
                    << "have unique label/location (existing duplicate has pKey="
                    << duplicateItemKey
                    << ")";

                TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                            TA_Base_Core::DataException::NOT_UNIQUE,
                            "LocationKey and Label") );
            }
        }
    }


    void PaZoneGroupHelper::deletePaZoneGroup()
    {
        FUNCTION_ENTRY("deletePaZoneGroup");

        TA_ASSERT(!m_isNew, "This PaZoneGroup does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);
        //std::ostringstream sql;
        
        // Good to go...
      /*  sql.str("");
        sql << "delete " << PA_ZONE_GROUP_TABLE_NAME << " where " << KEY_COL << " = " << m_key;*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_DELETE_45901, getLocalDatabaseName(), m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_DELETE_45901, getLocalDatabaseName(), m_key);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void PaZoneGroupHelper::modifyExistingPaZoneGroup(bool recreateModified) //throw(TA_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaZoneGroup");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA Zone Group %u already exists. It's data will be updated.", m_key);

        validateLocationKeyExistence();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write); 

		/*std::ostringstream sql;

		sql << "BEGIN "
			<< "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '"
			<< "update " << PA_ZONE_GROUP_TABLE_NAME << " set " 
            << LOCATIONKEY_COL << " = " << m_locationKey.getValue() << ", "
            << LABEL_COL << " = ''"
            << (databaseConnection->escapeAQSQLString(m_label)).c_str() << "'', " 
            << ISEVENTGROUP_COL << " = " << m_isEventGroup.getValue() << " "
            << "where " << KEY_COL << " = " << m_key
			<< " ');"
			<< "END;"; */  
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_UPDATE_45701, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_UPDATE_45701,
			getLocalDatabaseName(), m_locationKey.getValue(), databaseConnection->escapeAQSQLString(m_label), m_isEventGroup.getValue(), m_key);
		
//        std::ostringstream sql;
//
//        sql << "update " << PA_ZONE_GROUP_TABLE_NAME << " set " 
//            << LOCATIONKEY_COL << " = " << m_locationKey.getValue() << ", "
//            << LABEL_COL << " = '"
//            << (databaseConnection->escapeInsertString(m_label)).c_str() << "', " 
//            << ISEVENTGROUP_COL << " = " << m_isEventGroup.getValue() << " " 
//            << "where " << KEY_COL << " = " << m_key;        

        
        databaseConnection->executeModification(strSql);

        // now need to update the date fields
       /* sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_ZONE_GROUP_TABLE_NAME << " where " << KEY_COL << " = " << m_key;*/
//		strSql = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_SELECT_45551, m_key);

		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_SELECT_45551, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 == data->getNumRows()) 
        {                    
            delete data;
            data = NULL;

            // PA Zone Group record not found - this means someone has deleted
            // the record (the UPDATE statement won't fail, simply have no effect in
            // situations like these)
            // If the user specified a recreate, then attempt to recreate the record
            if (recreateModified)
            {
                addNewPaZoneGroupUsingKey(databaseConnection, m_key);
                // The add new function performs all required updates, so return early
                return;
            }
            else
            {
                std::ostringstream reasonMessage;
		        reasonMessage << "No data found for PA Zone Group key = " << m_key;			
                TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone Group key" ) );
            }
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


    void PaZoneGroupHelper::addNewPaZoneGroup() //throw(TA_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPaZoneGroup");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "PA Zone Group %u is a new PaZoneGroup. It will be added to the database.",m_locationKey.getValue());

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
                                                                PA_ZONE_GROUP_SEQUENCE_COUNTER);
        
        addNewPaZoneGroupUsingKey(databaseConnection, sequenceNumber);
    }


    void PaZoneGroupHelper::addNewPaZoneGroupUsingKey(TA_Base_Core::IDatabase* databaseConnection, unsigned long keyToUse)
    {
        FUNCTION_ENTRY("addNewPaZoneGroupUsingKey");

		/*std::ostringstream sql;
		sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'PUBLIC', '";
        sql << "INSERT INTO " << PA_ZONE_GROUP_TABLE_NAME << " (";
		sql << KEY_COL << ", ";
		sql << LOCATIONKEY_COL << ", ";
		sql << LABEL_COL << ", ";
		sql << ISEVENTGROUP_COL << ") ";
		sql << "values (" << keyToUse << ",";
		sql << m_locationKey.getValue() << ",''";
		sql << (databaseConnection->escapeAQSQLString(m_label)).c_str() << "'',";
		sql << m_isEventGroup.getValue() << ")');";
		sql << "END;";*/
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_INSERT_45851, getLocalDatabaseName(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_INSERT_45851, getLocalDatabaseName(),
			keyToUse, m_locationKey.getValue(), databaseConnection->escapeAQSQLString(m_label), m_isEventGroup.getValue());

//        std::ostringstream sql;
//        sql << "insert into " << PA_ZONE_GROUP_TABLE_NAME << " (" 
//            << KEY_COL << ", "
//            << LOCATIONKEY_COL << ", "
//            << LABEL_COL << ", "
//            << ISEVENTGROUP_COL << ") "
//            << "values (" << keyToUse << ","
//            << m_locationKey.getValue() << ",'" 
//            << (databaseConnection->escapeInsertString(m_label)).c_str() << "'," 
//            << m_isEventGroup.getValue() << ")";

        databaseConnection->executeModification(strSql);

       /* sql.str("");
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_ZONE_GROUP_TABLE_NAME << " where "
            << KEY_COL << " = " << keyToUse;*/
//		strSql = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_SELECT_45552, keyToUse);

		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_SELECT_45552, keyToUse);

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA Zone Group record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Zone Group key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone Group key" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,KEY_COL);
            m_dateCreated = data->getDateData(0,DATECREATED_COL);
            // Reset modified date (in case we're recreating this record), TES #782
            m_dateModified = 0;
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        TA_ASSERT( m_key == keyToUse, "Internal error");

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    void PaZoneGroupHelper::validateLocationKeyExistence() //throw(TA_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First check that the LOCATIONKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        
     /*   std::ostringstream sql;
        sql << "select PKEY from LOCATION where PKEY = " << m_locationKey.getValue();	*/	
        
        std::vector<std::string> columnNames;
        columnNames.clear();
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
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


    IPaZones PaZoneGroupHelper::getAssociatedPaZones()
	{   
        if (isNew())
        {
			// marvin++ TD8110
			if(m_isCopiedKeySet == true)
			{
				//Implementation for new zone groups copied
				return PaZoneAccessFactory::getInstance().getPaZonesByGroupId( m_copiedKey, false);
			}
			// ++marvin TD8110
			else
			{
				// New zone groups have no associations to begin with
				IPaZones empty;
				return empty;
			}
        }
        
        return PaZoneAccessFactory::getInstance().getPaZonesByGroupId( m_key, false);
	}


    void PaZoneGroupHelper::destroyGroupZoneAssociations(unsigned long groupKey)
    {
        // Calculate required sequence number (primary key) for insertion
        TA_Base_Core::IDatabase* databaseConnection = 
                    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        //std::ostringstream sql;

        // Remove all helpers where associated groupKey is a match
      /*  sql << "delete " << PA_ZONE_GROUP_HELPER_TABLE_NAME 
            << " where " << PA_ZGH_GROUPKEY_COL << " = " << groupKey;*/
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_DELETE_45902, getLocalDatabaseName(), groupKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_DELETE_45902, getLocalDatabaseName(), groupKey);

        databaseConnection->executeModification(strSql);
    }


    void PaZoneGroupHelper::associateGroupWithZones(unsigned long groupKey,
                                                    const std::list<unsigned long>& zoneKeys)
    {
        TA_Base_Core::IDatabase* databaseConnection = 
                    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        // Make associations in reverse, in this way
        // the zones are returned in same order when making fetches (probably by chance, so not important)
        for (std::list<unsigned long>::const_reverse_iterator it = zoneKeys.rbegin(); 
                            it != zoneKeys.rend(); 
                            it ++)
        {
            unsigned long zoneKey = (*it);

            // First off check this will be a unique entry
            validateNewGroupIdZoneKeyUniqueness(groupKey, zoneKey);
            
            // Calculate required sequence number (primary key) for insertion
            unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(
				databaseConnection,
				PA_ZONE_GROUP_HELPER_SEQUENCE_COUNTER);
            
            // Now insert entry into the database...  
			/*std::ostringstream sql;

			sql << "BEGIN ";
			sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'PUBLIC', '";
			sql << "INSERT INTO " << PA_ZONE_GROUP_HELPER_TABLE_NAME << " (";
			sql << PA_ZONE_GROUP_HELPER_KEY_COL << ", ";
			sql << PA_ZGH_GROUPKEY_COL << ", ";
			sql << PA_ZGH_ZONEKEY_COL << ") ";
			sql << "values (" << sequenceNumber << ",";
			sql << groupKey << ",";
			sql << zoneKey << ")');";
			sql << "END;";*/
//			std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_INSERT_45852,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_INSERT_45852,
				getLocalDatabaseName(), sequenceNumber, groupKey, zoneKey);
			
//			std::ostringstream sql;
//			sql << "insert into " << PA_ZONE_GROUP_HELPER_TABLE_NAME << " (" 
//				<< PA_ZONE_GROUP_HELPER_KEY_COL << ", "
//				<< PA_ZGH_GROUPKEY_COL << ", "
//                << PA_ZGH_ZONEKEY_COL << ") "
//                << "values (" << sequenceNumber << ","
//                << groupKey << "," 
//                << zoneKey  << ")";
			
            databaseConnection->executeModification(strSql);
        }        
    }


    void PaZoneGroupHelper::removeGroupZoneAssociations(unsigned long groupKey,
                                                        const std::list<unsigned long>& zoneKeys)
    {
        // Delete the record in the zone group helper with a matching (groupKey, zoneKey) pair        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        // Now insert entry into the database...        
        std::ostringstream sql;
      /*  sql << "delete " << PA_ZONE_GROUP_HELPER_TABLE_NAME << " where " 
            << PA_ZGH_GROUPKEY_COL << " = " << groupKey << " and "
            << PA_ZGH_ZONEKEY_COL << " in ( ";*/
         
        for (std::list<unsigned long>::const_iterator itr = zoneKeys.begin(); itr != zoneKeys.end(); itr ++)
        {             
            if (itr != zoneKeys.begin())
            {
                // Comma separate all elements
                sql << ", ";
            }
            sql << (*itr);
        }

        //sql << ")";
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_DELETE_45903, sql.str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_DELETE_45903, getLocalDatabaseName(), groupKey, sql.str());
            
        databaseConnection->executeModification(strSql);
    }


    void PaZoneGroupHelper::validateNewGroupIdZoneKeyUniqueness(unsigned long groupId,
                                                                unsigned long zoneKey)
    {   
       /* std::ostringstream sql;
        sql << "select " << PA_ZONE_GROUP_HELPER_KEY_COL
            << " from " << PA_ZONE_GROUP_HELPER_TABLE_NAME 
            << " where " << PA_ZGH_GROUPKEY_COL << " = " << groupId
            << " and " << PA_ZGH_ZONEKEY_COL << " = " << zoneKey;*/

        std::vector<std::string> columnNames;
        columnNames.push_back(PA_ZONE_GROUP_HELPER_KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ZONE_GROUP_SELECT_45503, groupId, zoneKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ZONE_GROUP_SELECT_45503, groupId, zoneKey);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 != data->getNumRows()) 
        {        
            unsigned long duplicateItemKey = data->getUnsignedLongData(0, PA_ZONE_GROUP_HELPER_KEY_COL);

            std::ostringstream str;
            str << "Attempting to write Pa Zone record that doesn't "
                << "have unique group/zone ID (existing duplicate has pKey="
                << duplicateItemKey
                << ")";

            TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                                TA_Base_Core::DataException::NOT_UNIQUE,
                                "GroupKey and ZoneKey") );            
        }
    }

    std::string PaZoneGroupHelper::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
				std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Pa_Cd, Write);

               /* DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Pa_Cd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }
            
        }
        return m_localDatabase;
    }
} // closes TA_Base_Core

