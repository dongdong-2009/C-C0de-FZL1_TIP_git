/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaBroadcastShortcutHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by:  $Author: builder $
  *
  * PaBroadcastShortcutHelper is an object that is held by PaDvaMessage and ConfigPaDvaMessage objects. 
  * It contains all data used by PaDvaMessages, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessage and ConfigPaDvaMessage.
  */


#pragma warning(disable:4786 4290 4503)

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/PaBroadcastShortcutHelper.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/ValueNotSetException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{

PaBroadcastShortcutHelper::PaBroadcastShortcutHelper( const PaBroadcastShortcutHelper& thePaBroadcastShortcutHelper) : 
m_key( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_ulLocationKey( thePaBroadcastShortcutHelper.m_ulLocationKey ),
m_ulShortcutNo( thePaBroadcastShortcutHelper.m_ulShortcutNo ),
m_ulDVAMsgKey( thePaBroadcastShortcutHelper.m_ulDVAMsgKey ),
m_isValidData( false ),
m_isNew( true )
{
}

PaBroadcastShortcutHelper::PaBroadcastShortcutHelper(const unsigned long key) : 
m_key( key ),
m_ulLocationKey(),
m_ulShortcutNo(),
m_ulDVAMsgKey(),
m_isValidData( false ),
m_isNew( false )
{
}

PaBroadcastShortcutHelper::PaBroadcastShortcutHelper(unsigned long row, TA_Base_Core::IData& data) : 
m_key( data.getUnsignedLongData(row, KEY_COL) ),
m_ulLocationKey(),
m_ulShortcutNo(),
m_ulDVAMsgKey(),
m_isValidData( false ),
m_isNew( false )
{
    reloadUsing( row, data );
}

PaBroadcastShortcutHelper::PaBroadcastShortcutHelper() : 
m_key( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_ulLocationKey(),
m_ulShortcutNo(),
m_ulDVAMsgKey(),
m_isValidData( false ),
m_isNew( true )
{
}


PaBroadcastShortcutHelper::~PaBroadcastShortcutHelper()
{
}


unsigned long PaBroadcastShortcutHelper::getKey()
{
    FUNCTION_ENTRY("getKey" );
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;
    return m_key;
}

unsigned long PaBroadcastShortcutHelper::getLocationKey()
{
    FUNCTION_ENTRY("getLocationKey" );
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    // Following the reload of data, all values should have been set
    try
    {
        if (isNew() && !m_ulLocationKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return m_ulLocationKey.getValue();
    }
    catch (::TA_Base_Core::ValueNotSetException&)
    {
        // reload() should have either thrown exception or set this value
        TA_ASSERT(false, "m_ulLocationKey value not set after reload");
        throw;
    }

    FUNCTION_EXIT;
}

void PaBroadcastShortcutHelper::setLocationKey(const unsigned long locationKey)
{
    FUNCTION_ENTRY("setLocationKey" );
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    m_ulLocationKey.setValue(locationKey);
    
    FUNCTION_EXIT;
}

unsigned long PaBroadcastShortcutHelper::getShortcutNo()
{
    FUNCTION_ENTRY("getShortcutNo" );
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    // Following the reload of data, all values should have been set
    try
    {
        if (isNew() && !m_ulShortcutNo.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }
        
        return m_ulShortcutNo.getValue();
    }
    catch (::TA_Base_Core::ValueNotSetException&)
    {
        // reload() should have either thrown exception or set this value
        TA_ASSERT(false, "m_ulLocationKey value not set after reload");
        throw;
    }

    FUNCTION_EXIT;
}

void PaBroadcastShortcutHelper::setShortcutNo( unsigned long ulShortcutNo )
{
    FUNCTION_ENTRY("setShortcutNo" );
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    m_ulShortcutNo.setValue(ulShortcutNo);
    
    FUNCTION_EXIT;
}

unsigned long PaBroadcastShortcutHelper::getDVAMsgKey()
{
    FUNCTION_ENTRY("getDVAMsgKey" );
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    
    // Following the reload of data, all values should have been set
    try
    {
        if (isNew() && !m_ulDVAMsgKey.hasBeenSet())
        {
            // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
            // value for an unset key (otherwise, if loaded from DB, it must be set)
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }
        
        return m_ulDVAMsgKey.getValue();
    }
    catch (::TA_Base_Core::ValueNotSetException&)
    {
        // reload() should have either thrown exception or set this value
        TA_ASSERT(false, "m_ulLocationKey value not set after reload");
        throw;
    }
    
    FUNCTION_EXIT;
}

void PaBroadcastShortcutHelper::setDVAMsgKey( unsigned long ulDVAMsgKey )
{
    FUNCTION_ENTRY("setDVAMsgKey" );
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    m_ulDVAMsgKey.setValue(ulDVAMsgKey);
    
    FUNCTION_EXIT;
}

void PaBroadcastShortcutHelper::invalidate()
{
    FUNCTION_ENTRY("invalidate" );

    // This method cannot be called until the key has been set
    TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaDvaMessage");

    m_isValidData = false;

    FUNCTION_EXIT;
}

const DatabaseQueryHelper::QueryData PaBroadcastShortcutHelper::getBasicQueryData()
{
    DatabaseQueryHelper::QueryData result;

    result.primaryKeyCol = KEY_COL;
	result.tableName = PA_BROADCAST_SHORTCUT_TABLE;

    result.columnNames.push_back(KEY_COL);
    result.columnNames.push_back(LOCATION_KEY_COL);
    result.columnNames.push_back(SHORTCUT_NO_COL);
    result.columnNames.push_back(DVA_MSG_KEY_COL);
    result.columnNames.push_back(DATE_CREATED_COL);
    result.columnNames.push_back(DATE_MODIFIED_COL);

    DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement, result.columnNames, result.tableName);

    return result;
}


void PaBroadcastShortcutHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
{
    FUNCTION_ENTRY("reloadUsing");

    // Assign the data as appropriate to the member variables.
    // These calls can throw DataExceptions, as documented in the comment of this method.
    m_ulLocationKey.setValue( data.getUnsignedLongData( row, LOCATION_KEY_COL ));
    m_ulShortcutNo.setValue( data.getUnsignedLongData( row, SHORTCUT_NO_COL ));
	m_ulDVAMsgKey.setValue( data.getUnsignedLongData( row, DVA_MSG_KEY_COL ));

    // Need to record that we now have valid data
    m_isValidData = true;
    FUNCTION_EXIT;
}


void PaBroadcastShortcutHelper::writePaBroadcastShortcut() //throw(TA_Base_Core::TransactiveException)
{       
    FUNCTION_ENTRY( "writePaDvaMessageData" );     

    // Check ALL values exist. If not add them to the dataConfiguratonException
    std::vector<std::string> fieldNames;
    if (!m_ulLocationKey.hasBeenSet())
    {
        fieldNames.push_back(LOCATION_KEY_COL);
    }
    if (!m_ulShortcutNo.hasBeenSet())
    {
        fieldNames.push_back(SHORTCUT_NO_COL);
    }
    if (!m_ulDVAMsgKey.hasBeenSet())
    {
        fieldNames.push_back(DVA_MSG_KEY_COL);
    }
   
    if ( !fieldNames.empty() ) // Need to throw an exception
    {
        TA_THROW(TA_Base_Core::DataConfigurationException(
            "PaBroadcastShortcut data not fully specified. PaBroadcastShortcut cannot be written to database",
            fieldNames));
    }

    validateIdLocationPairUniqueness();

    // Write the PaBroadcastShortcut to the database

    try 
    {
        // Need to check if this is a new PaBroadcastShortcut or an existing PaBroadcastShortcut - 
        // as a new PaBroadcastShortcut will be inserted, while an existing PaBroadcastShortcut will be updated
        if (m_isNew) // This is a new PaBroadcastShortcut
        {
            addNewPaBroadcastShortcut();
        }
        else // This is an existing PaBroadcastShortcut
        {
            modifyExistingPaBroadcastShortcut();
        }
    }
    catch (TA_Base_Core::ValueNotSetException&)
    {
        // Should not get here if properly validated set state of fields before calling
        // statements in the try block
        std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

        TA_THROW(TA_Base_Core::DataConfigurationException(
                    "PaDvaMessage data not fully specified. PaDvaMessage cannot be written to database",
                    dummyNames));
    }

    // Now that the data has been written, what we hold is valid data.
    m_isValidData = true;
    m_isNew = false;

    FUNCTION_EXIT;
}


void PaBroadcastShortcutHelper::validateIdLocationPairUniqueness() 
{   
   /* std::ostringstream sql;
    sql << "select " << KEY_COL
        << " from " << PA_BROADCAST_SHORTCUT_TABLE 
        << " where " << LOCATION_KEY_COL << " = " << m_ulLocationKey.getValue()
        << " and " << SHORTCUT_NO_COL << " = " << m_ulShortcutNo.getValue();*/

    std::vector<std::string> columnNames;
    columnNames.push_back(KEY_COL);
              
    TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);        

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, PA_DVA_MSG_SHORTCUT_CONFIG_SELECT_107003, m_ulLocationKey.getValue(), m_ulShortcutNo.getValue());

    TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

    if (0 != data->getNumRows()) 
    {        
        unsigned long duplicateItemKey = data->getUnsignedLongData(0, KEY_COL);

        // Ignore ourself..
        if (duplicateItemKey != m_key)
        {
            std::ostringstream str;
            str << "Attempting to write Pa Dva Message record that doesn't "
                << "have unique id/location (existing duplicate has pKey="
                << duplicateItemKey
                << ")";


            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                        TA_Base_Core::DataException::NOT_UNIQUE,
                        "LocationKey and Id") );
        }
    }

    delete data;
    data = NULL;

}


void PaBroadcastShortcutHelper::deletePaBroadcastShortcut()
{
    FUNCTION_ENTRY("deletePaBroadcastShortcut");

    TA_ASSERT(!m_isNew, "This PaBroadcastShortcut does not yet exist in the database, and therefore cannot be deleted");
    
    TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
    /*std::ostringstream sql;

    sql.str("");
    sql << "delete " << PA_BROADCAST_SHORTCUT_TABLE << " where " << KEY_COL << " = " << m_key;*/

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, PA_DVA_MSG_SHORTCUT_CONFIG_DELETE_107004, m_key);

    databaseConnection->executeModification(sql);

    FUNCTION_EXIT;
}


void PaBroadcastShortcutHelper::modifyExistingPaBroadcastShortcut() //throw(TA_Base_Core::TransactiveException)
{
    FUNCTION_ENTRY("modifyExistingPaBroadcastShortcut");

    LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "PaBroadcastShortcut %u already exists. It's data will be updated.", m_key);

    validateLocationKeyExistence();

    ////////////////////////////////////////////////////////////////////////////
    //
    // Good to go...
    //
    ////////////////////////////////////////////////////////////////////////////
    TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
   /* std::ostringstream sql;

    sql << "update " << PA_BROADCAST_SHORTCUT_TABLE << " set " 
        << LOCATION_KEY_COL << " = " << m_ulLocationKey.getValue() << ", "
        << SHORTCUT_NO_COL << " = " << m_ulShortcutNo.getValue() << ", "
        << DVA_MSG_KEY_COL << " = "<< m_ulDVAMsgKey.getValue() << " "
        << "where " << KEY_COL << " = " << m_key; */
	

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, PA_DVA_MSG_SHORTCUT_CONFIG_UPDATE_107005, m_ulLocationKey.getValue(), m_ulShortcutNo.getValue(),
		m_ulDVAMsgKey.getValue(), m_key);

    databaseConnection->executeModification(sql);

    // nothing more to be done here
    FUNCTION_EXIT;
}


void PaBroadcastShortcutHelper::addNewPaBroadcastShortcut() //throw(TA_Base_Core::TransactiveException)
{
    FUNCTION_ENTRY("addNewPaBroadcastShortcut");

    LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
        "PaBroadcastShortcut %u is a new one. It will be added to the database.",m_ulLocationKey.getValue());

    validateLocationKeyExistence();


    ////////////////////////////////////////////////////////////////////////////
    //
    // Good to go...
    //
    ////////////////////////////////////////////////////////////////////////////
    TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
    // Obtain the primary key that should be used for this next entry
    unsigned long sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(
                                                            databaseConnection, 
                                                            PA_DVA_BROADCAST_SHORTCUT_SEQUENCE_COUNTER);

    /*std::ostringstream sql;
    sql << "insert into " << PA_BROADCAST_SHORTCUT_TABLE << " (" 
        << KEY_COL << ", "
        << LOCATION_KEY_COL << ", "
        << SHORTCUT_NO_COL << ", "
        << DVA_MSG_KEY_COL << ") "
        << "values (" << sequenceNumber << ","
        << m_ulLocationKey.getValue() << "," 
        << m_ulShortcutNo.getValue() << "," 
        << m_ulDVAMsgKey.getValue() << ")";*/

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql,PA_DVA_MSG_SHORTCUT_CONFIG_INSERT_107006, sequenceNumber, m_ulLocationKey.getValue(),
		m_ulShortcutNo.getValue(), m_ulDVAMsgKey.getValue());

    databaseConnection->executeModification(sql);

   /* sql.str("");*/

    // Obtain the newly created record
    /*sql << "select " 
        << KEY_COL << " "
        << "from " << PA_BROADCAST_SHORTCUT_TABLE << " where "
        << KEY_COL << " = " << sequenceNumber;*/

	databaseConnection->prepareSQLStatement(sql, PA_DVA_MSG_SHORTCUT_CONFIG_SELECT_107002, sequenceNumber);

    std::vector<std::string> columnNames;
    columnNames.push_back(KEY_COL);

    // Execute the query. The method can throw a DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

    if (0 == data->getNumRows()) 
    {
        // PA Zone record not found            
        delete data;
        data = NULL;

        std::ostringstream reasonMessage;
		reasonMessage << "No data found for PA Zone key = " << m_key;			
        TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone key" ) );
    }

    // Retrieve the pkey as an unsigned long. There should only be a single returned item 
    // (as any other circumstance is specifically prevented by the above checks).
    // This method can throw a DataException
    try
    {
        m_key = data->getUnsignedLongData(0,KEY_COL);
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


void PaBroadcastShortcutHelper::validateLocationKeyExistence() //throw(TA_Base_Core::TransactiveException)
{
    /*std::ostringstream sql;
    sql << "select PKEY from LOCATION where PKEY = " << m_ulLocationKey.getValue();*/
    
    std::vector<std::string> columnNames;
    columnNames.clear();
    columnNames.push_back("PKEY");

    TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, LOCATION_SELECT_7003, m_ulLocationKey.getValue());

    TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

    if (0 == data->getNumRows()) 
    {
        // Location key not found
        
        delete data;
        data = NULL;

        std::ostringstream reasonMessage;
		reasonMessage << "No data found for Location key = " << m_ulLocationKey.getValue();			
        TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Location key" ) );
    }

    delete data;
    data = NULL;
}

} // closes TA_IRS_Core
