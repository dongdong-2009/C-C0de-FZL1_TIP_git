/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaScheduleBroadcastHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by:  $Author: builder $
  *
  * PaScheduleBroadcastHelper is an object that is held by PaScheduleBroadcast and ConfigPaScheduleBroadcast objects. 
  * It contains all data used by PaScheduleBroadcast, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessage and ConfigPaScheduleBroadcast.
  */
#pragma warning(disable:4786 4290 4503)

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/PaScheduleBroadcastHelper.h"
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

PaScheduleBroadcastHelper::PaScheduleBroadcastHelper( const PaScheduleBroadcastHelper& thePaScheduleBroadcastHelper) : 
m_key( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_ulLocationKey( thePaScheduleBroadcastHelper.m_ulLocationKey ),
m_ulDvaMsgKey( thePaScheduleBroadcastHelper.m_ulDvaMsgKey ),
m_ulCyclicTimes( thePaScheduleBroadcastHelper.m_ulCyclicTimes ),
m_ulPriorityGroup( thePaScheduleBroadcastHelper.m_ulPriorityGroup ),
m_tmStartTime( thePaScheduleBroadcastHelper.m_tmStartTime ),
m_tmEndTime( thePaScheduleBroadcastHelper.m_tmEndTime ),
m_strScheduleName( thePaScheduleBroadcastHelper.m_strScheduleName ),
m_strIntervalType( thePaScheduleBroadcastHelper.m_strIntervalType ),
m_strSourceType( thePaScheduleBroadcastHelper.m_strSourceType ),
m_strStatus( thePaScheduleBroadcastHelper.m_strStatus ),
m_lstZoneKeys( thePaScheduleBroadcastHelper.m_lstZoneKeys ),
m_bEnable( thePaScheduleBroadcastHelper.m_bEnable ),
m_bAutoDel( thePaScheduleBroadcastHelper.m_bAutoDel ),
m_isValidData( false ),
m_isNew( true ),
m_bZonesLoaded( false )
{
}

PaScheduleBroadcastHelper::PaScheduleBroadcastHelper(const unsigned long key) : 
m_key( key ),
m_ulLocationKey(),
m_ulDvaMsgKey(),
m_ulCyclicTimes(),
m_ulPriorityGroup(),
m_tmStartTime( 0 ),
m_tmEndTime( 0 ),
m_strScheduleName( "" ),
m_strIntervalType( "" ),
m_strSourceType( "" ),
m_strStatus( "" ),
m_lstZoneKeys(),
m_bEnable( false ),
m_bAutoDel( false ),
m_isValidData( false ),
m_isNew( false ),
m_bZonesLoaded( false )
{
}

PaScheduleBroadcastHelper::PaScheduleBroadcastHelper(unsigned long row, TA_Base_Core::IData& data) : 
m_key( data.getUnsignedLongData(row, KEY_COL) ),
m_ulLocationKey(),
m_ulDvaMsgKey(),
m_ulCyclicTimes(),
m_ulPriorityGroup(),
m_tmStartTime( 0 ),
m_tmEndTime( 0 ),
m_strScheduleName( "" ),
m_strIntervalType( "" ),
m_strSourceType( "" ),
m_strStatus( "" ),
m_lstZoneKeys(),
m_bEnable( false ),
m_bAutoDel( false ),
m_isValidData( false ),
m_isNew( false ),
m_bZonesLoaded( false )
{
    reloadUsing( row, data );
}

PaScheduleBroadcastHelper::PaScheduleBroadcastHelper() : 
m_key( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_ulLocationKey(),
m_ulDvaMsgKey(),
m_ulCyclicTimes(),
m_ulPriorityGroup(),
m_tmStartTime( 0 ),
m_tmEndTime( 0 ),
m_strScheduleName( "" ),
m_strIntervalType( "" ),
m_strSourceType( "" ),
m_strStatus( "" ),
m_lstZoneKeys(),
m_bEnable( false ),
m_bAutoDel( false ),
m_isValidData( false ),
m_isNew( true ),
m_bZonesLoaded( false )
{
}


PaScheduleBroadcastHelper::~PaScheduleBroadcastHelper()
{
}


unsigned long PaScheduleBroadcastHelper::getKey()
{
    FUNCTION_ENTRY("getKey" );

    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_key;
}

unsigned long PaScheduleBroadcastHelper::getLocationKey()
{
    FUNCTION_ENTRY("getLocationKey" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }

    // Following the reload of data, all values should have been set
    try
    {
        if (isNew() && !m_ulLocationKey.hasBeenSet())
        {
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return m_ulLocationKey.getValue();
    }
    catch (TA_Base_Core::ValueNotSetException&)
    {
        // reload() should have either thrown exception or set this value
        TA_ASSERT(false, "m_ulLocationKey value not set after reload");
        throw;
    }

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setLocationKey(const unsigned long locationKey)
{
    FUNCTION_ENTRY("setLocationKey" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_ulLocationKey.setValue(locationKey);
    
    FUNCTION_EXIT;
}

unsigned long PaScheduleBroadcastHelper::getDvaMsgKey()
{
    FUNCTION_ENTRY("getDvaMsgKey" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    
    // Following the reload of data, all values should have been set
    try
    {
        if (isNew() && !m_ulDvaMsgKey.hasBeenSet())
        {
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }
        
        return m_ulDvaMsgKey.getValue();
    }
    catch (::TA_Base_Core::ValueNotSetException&)
    {
        // reload() should have either thrown exception or set this value
        TA_ASSERT(false, "m_ulLocationKey value not set after reload");
        throw;
    }
    
    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setDvaMsgKey( unsigned long ulDvaMsgKey )
{
    FUNCTION_ENTRY("setDvaMsgKey" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_ulDvaMsgKey.setValue(ulDvaMsgKey);
    
    FUNCTION_EXIT;
}

unsigned long PaScheduleBroadcastHelper::getCyclicTimes()
{
    FUNCTION_ENTRY("getCyclicTimes" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }

    // Following the reload of data, all values should have been set
    try
    {
        if (isNew() && !m_ulCyclicTimes.hasBeenSet())
        {
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return m_ulCyclicTimes.getValue();
    }
    catch (::TA_Base_Core::ValueNotSetException&)
    {
        // reload() should have either thrown exception or set this value
        TA_ASSERT(false, "m_ulLocationKey value not set after reload");
        throw;
    }

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setCyclicTimes( unsigned long ulCyclicTimes )
{
    FUNCTION_ENTRY("setCyclicTimes" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_ulCyclicTimes.setValue(ulCyclicTimes);

    FUNCTION_EXIT;
}

unsigned long PaScheduleBroadcastHelper::getPriorityGroup()
{
    FUNCTION_ENTRY("getPriorityGroup" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }

    // Following the reload of data, all values should have been set
    try
    {
        if (isNew() && !m_ulPriorityGroup.hasBeenSet())
        {
            return TA_Base_Core::DatabaseKey::getInvalidKey();                
        }

        return m_ulPriorityGroup.getValue();
    }
    catch (::TA_Base_Core::ValueNotSetException&)
    {
        // reload() should have either thrown exception or set this value
        TA_ASSERT(false, "m_ulLocationKey value not set after reload");
        throw;
    }

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setPriorityGroup( unsigned long ulPriorityGroup )
{
    FUNCTION_ENTRY("setPriorityGroup" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_ulPriorityGroup.setValue(ulPriorityGroup);

    FUNCTION_EXIT;
}

time_t PaScheduleBroadcastHelper::getStartTime()
{
    FUNCTION_ENTRY("getStartTime" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    return m_tmStartTime;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setStartTime( time_t tmStartTime )
{
    FUNCTION_ENTRY("setStartTime" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_tmStartTime = tmStartTime;

    FUNCTION_EXIT;
}

time_t PaScheduleBroadcastHelper::getEndTime()
{
    FUNCTION_ENTRY("getEndTime" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    return m_tmEndTime;

        FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setEndTime( time_t tmEndTime )
{
    FUNCTION_ENTRY("setEndTime" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_tmEndTime = tmEndTime;

    FUNCTION_EXIT;
}

std::string PaScheduleBroadcastHelper::getScheduleName()
{
    FUNCTION_ENTRY("getScheduleName" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    return m_strScheduleName;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setScheduleName( const std::string& strScheduleName )
{
    FUNCTION_ENTRY("setScheduleName" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_strScheduleName = strScheduleName;

    FUNCTION_EXIT;
}

std::string PaScheduleBroadcastHelper::getIntervalType()
{
    FUNCTION_ENTRY("getIntervalType" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    return m_strIntervalType;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setIntervalType( const std::string& strIntervalType )
{
    FUNCTION_ENTRY("setIntervalType" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_strIntervalType = strIntervalType;

    FUNCTION_EXIT;
}

std::string PaScheduleBroadcastHelper::getSourceType()
{
    FUNCTION_ENTRY("getSourceType" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    return m_strSourceType;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setSourceType( const std::string& strSourceType )
{
    FUNCTION_ENTRY("setSourceType" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_strSourceType = strSourceType;

    FUNCTION_EXIT;
}

std::string PaScheduleBroadcastHelper::getStatus()
{
    FUNCTION_ENTRY("getStatus" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    return m_strStatus;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setStatus( const std::string& strStatus )
{
    FUNCTION_ENTRY("setStatus" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_strStatus = strStatus;

    FUNCTION_EXIT;
}

std::vector<unsigned long> PaScheduleBroadcastHelper::getZoneKeys( )
{
    FUNCTION_ENTRY("getSourceType" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    else if ( !m_bZonesLoaded )
    {
        reloadZones();
    }

    return m_lstZoneKeys;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setZoneKeys( const std::vector<unsigned long>& lstZoneKeys )
{
    FUNCTION_ENTRY("setScheduleName" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_lstZoneKeys = lstZoneKeys;

    FUNCTION_EXIT;
}

bool PaScheduleBroadcastHelper::getIsEnable()
{
    FUNCTION_ENTRY("getIsEnable" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    return m_bEnable;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setIsEnable( bool bEnable )
{
    FUNCTION_ENTRY("setIsEnable" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_bEnable = bEnable;

    FUNCTION_EXIT;
}

bool PaScheduleBroadcastHelper::getAutoDel()
{
    FUNCTION_ENTRY("getAutoDel" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    return m_bAutoDel;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::setAutoDel( bool bAutoDel )
{
    FUNCTION_ENTRY("setAutoDel" );

    if (!m_isValidData && !m_isNew)
    {
        reloadAll();
    }
    m_bAutoDel = bAutoDel;

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::invalidate()
{
    FUNCTION_ENTRY("invalidate" );

    // This method cannot be called until the key has been set
    TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaDvaMessage");

    m_isValidData = false;

    FUNCTION_EXIT;
}

const DatabaseQueryHelper::QueryData PaScheduleBroadcastHelper::getBasicQueryData()
{
    DatabaseQueryHelper::QueryData result;

    result.primaryKeyCol = KEY_COL;
	result.tableName = PA_SCHEDULE_BROADCAST_TABLE;

    result.columnNames.push_back(KEY_COL);
    result.columnNames.push_back(LOCATION_KEY_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_START_TIME_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_END_TIME_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_IS_ENABLE_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_AUTO_DEL_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_DVA_MSG_KEY_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL);
    result.columnNames.push_back(PA_SCHEDULE_BROADCAST_STATUS_COL);

    DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement, result.columnNames, result.tableName, false );

    return result;
}


void PaScheduleBroadcastHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
{
    FUNCTION_ENTRY("reloadUsing");

    // Assign the data as appropriate to the member variables.
    // These calls can throw DataExceptions, as documented in the comment of this method.
    m_ulLocationKey.setValue( data.getUnsignedLongData( row, LOCATION_KEY_COL ));
    m_strScheduleName = data.getStringData( row, PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL );
    m_tmStartTime = data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_START_TIME_COL );
    m_tmEndTime = data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_END_TIME_COL );
    m_strIntervalType = data.getStringData( row, PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL );
    m_bEnable = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_IS_ENABLE_COL );
    m_bAutoDel = data.getBooleanData( row, PA_SCHEDULE_BROADCAST_AUTO_DEL_COL );
    m_ulDvaMsgKey.setValue( data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_DVA_MSG_KEY_COL ));
    m_ulCyclicTimes.setValue( data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL ));
    m_ulPriorityGroup.setValue( data.getUnsignedLongData( row, PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL ));
    m_strSourceType = data.getStringData( row, PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL );
    m_strStatus = data.getStringData( row, PA_SCHEDULE_BROADCAST_STATUS_COL );

    // Need to record that we now have valid data
    m_isValidData = true;
    FUNCTION_EXIT;
}


void PaScheduleBroadcastHelper::writePaScheduleBroadcast() //throw(TA_Base_Core::TransactiveException)
{       
    FUNCTION_ENTRY( "writePaScheduleBroadcast" );     

    // Check ALL values exist. If not add them to the dataConfiguratonException
    std::vector<std::string> fieldNames;
    if (!m_ulLocationKey.hasBeenSet())
    {
        fieldNames.push_back(LOCATION_KEY_COL);
    }
    if (m_strScheduleName.empty())
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL);
    }
    if ( 0 == m_tmStartTime )
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_START_TIME_COL);
    }
    if (m_strIntervalType.empty())
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL);
    }
    if (!m_ulDvaMsgKey.hasBeenSet())
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_DVA_MSG_KEY_COL);
    }
    if (!m_ulCyclicTimes.hasBeenSet())
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL);
    }
    if (!m_ulPriorityGroup.hasBeenSet())
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL);
    }
    if (m_strSourceType.empty())
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL);
    }
    if (m_strStatus.empty())
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_STATUS_COL);
    }
    if (m_lstZoneKeys.empty())
    {
        fieldNames.push_back(PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL);
    }

    if ( !fieldNames.empty() ) // Need to throw an exception
    {
        TA_THROW(TA_Base_Core::DataConfigurationException(
            "PaScheduleBroadcast data not fully specified. PaScheduleBroadcast cannot be written to database",
            fieldNames));
    }

    try 
    {
        // Need to check if this is a new PaScheduleBroadcast or an existing PaScheduleBroadcast - 
        // as a new PaScheduleBroadcast will be inserted, while an existing PaScheduleBroadcast will be updated
        if (m_isNew) // This is a new PaScheduleBroadcast
        {
            addNewPaScheduleBroadcast();
        }
        else // This is an existing PaScheduleBroadcast
        {
            modifyExistingPaScheduleBroadcast();
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

void PaScheduleBroadcastHelper::deletePaScheduleBroadcast()
{
    FUNCTION_ENTRY("deletePaScheduleBroadcast");

    TA_ASSERT(!m_isNew, "This PaScheduleBroadcast does not yet exist in the database, and therefore cannot be deleted");
    
    TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
    /*std::ostringstream sql;

    sql.str("");
    sql << "delete " << PA_SCHEDULE_BROADCAST_ZONES_TABLE << " where " 
        << PA_SCHEDULE_BROADCAST_ZONES_SCHEDULE_BROADCAST_KEY_COL << " = " << m_key;*/

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, PA_SCHEDULE_BROADCAST_ZONES_DELETE_120001, m_key);

    databaseConnection->executeModification(sql);

    /*sql.str("");
    sql << "delete " << PA_SCHEDULE_BROADCAST_TABLE << " where " << KEY_COL << " = " << m_key;*/
	
	databaseConnection->prepareSQLStatement(sql,PA_SCHEDULE_BROADCAST_DELETE_110002, m_key);

    databaseConnection->executeModification(sql);

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::modifyExistingPaScheduleBroadcast() //throw(TA_Base_Core::TransactiveException)
{
    FUNCTION_ENTRY("modifyExistingPaScheduleBroadcast");

    LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "PaScheduleBroadcast %u already exists. It's data will be updated.", m_key);

    validateLocationKeyExistence();

    ////////////////////////////////////////////////////////////////////////////
    //
    // Good to go...
    //
    ////////////////////////////////////////////////////////////////////////////
    TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
    /*std::ostringstream sql;

    sql << "update " << PA_SCHEDULE_BROADCAST_TABLE << " set " 
        << LOCATION_KEY_COL << " = " << m_ulLocationKey.getValue() << ", "
        << PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL << " = '" 
        << (databaseConnection->escapeAQSQLString(m_strScheduleName)).c_str() << "', "
        << PA_SCHEDULE_BROADCAST_START_TIME_COL << " = "<< m_tmStartTime << ", "
        << PA_SCHEDULE_BROADCAST_END_TIME_COL << " = "<< m_tmEndTime << ", "
        << PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL << " = '" 
        << (databaseConnection->escapeAQSQLString(m_strIntervalType)).c_str() << "', "
        << PA_SCHEDULE_BROADCAST_IS_ENABLE_COL << " = "<< m_bEnable << ", "
        << PA_SCHEDULE_BROADCAST_AUTO_DEL_COL << " = "<< m_bAutoDel << ", "
        << PA_SCHEDULE_BROADCAST_DVA_MSG_KEY_COL << " = "<< m_ulDvaMsgKey.getValue() << ", "
        << PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL << " = "<< m_ulCyclicTimes.getValue() << ", "
        << PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL << " = "<< m_ulPriorityGroup.getValue() << ", "
        << PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL << " = '" 
        << (databaseConnection->escapeAQSQLString(m_strSourceType)).c_str() << "', "
        << PA_SCHEDULE_BROADCAST_STATUS_COL << " = '" 
        << (databaseConnection->escapeAQSQLString(m_strStatus)).c_str() << "' "
        << "where " << KEY_COL << " = " << m_key;*/

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, PA_SCHEDULE_BROADCAST_UPDATE_110003,m_ulLocationKey.getValue(), (databaseConnection->escapeAQSQLString(m_strScheduleName)).c_str(),
		m_tmStartTime,m_tmEndTime,(databaseConnection->escapeAQSQLString(m_strIntervalType)).c_str(), m_bEnable, m_bAutoDel, m_ulDvaMsgKey.getValue(),m_ulCyclicTimes.getValue(),
		m_ulPriorityGroup.getValue(), (databaseConnection->escapeAQSQLString(m_strSourceType)).c_str(),(databaseConnection->escapeAQSQLString(m_strStatus)).c_str(),m_key);

    databaseConnection->executeModification(sql);

    /*sql.str("");
    sql << "delete " << PA_SCHEDULE_BROADCAST_ZONES_TABLE << " where " 
        << PA_SCHEDULE_BROADCAST_ZONES_SCHEDULE_BROADCAST_KEY_COL << " = " << m_key;*/

	SQLStatement sql2;
	databaseConnection->prepareSQLStatement(sql2,PA_SCHEDULE_BROADCAST_ZONES_DELETE_120001, m_key);

    databaseConnection->executeModification(sql2);

    for ( std::vector<unsigned long>::iterator itLoop = m_lstZoneKeys.begin(); m_lstZoneKeys.end() != itLoop; ++itLoop )
    {
        /*sql.str("");
        sql << "insert into " << PA_SCHEDULE_BROADCAST_ZONES_TABLE << " ( " 
            << PA_SCHEDULE_BROADCAST_ZONES_SCHEDULE_BROADCAST_KEY_COL << ", "
            << PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL << " ) values ( " << m_key << ", " << (*itLoop) << " )";*/

		SQLStatement sql3;
		databaseConnection->prepareSQLStatement(sql3, PA_SCHEDULE_BROADCAST_ZONES_INSERT_120002, m_key, (*itLoop));

        databaseConnection->executeModification(sql3);
    }

    // nothing more to be done here
    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::addNewPaScheduleBroadcast() //throw(TA_Base_Core::TransactiveException)
{
    FUNCTION_ENTRY("addNewPaScheduleBroadcast");

    LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
        "PaScheduleBroadcast %u is a new one. It will be added to the database.",m_ulLocationKey.getValue());

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
    sql << "insert into " << PA_SCHEDULE_BROADCAST_TABLE << " (" 
        << KEY_COL << ", "
        << LOCATION_KEY_COL << ", "
        << PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL << ", "
        << PA_SCHEDULE_BROADCAST_START_TIME_COL << ", "
        << PA_SCHEDULE_BROADCAST_END_TIME_COL << ", "
        << PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL << ", "
        << PA_SCHEDULE_BROADCAST_IS_ENABLE_COL << ", "
        << PA_SCHEDULE_BROADCAST_AUTO_DEL_COL << ", "
        << PA_SCHEDULE_BROADCAST_DVA_MSG_KEY_COL << ", "
        << PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL << ", "
        << PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL << ", "
        << PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL << ", "
        << PA_SCHEDULE_BROADCAST_STATUS_COL << ") "
        << "values (" << sequenceNumber << ","
        << m_ulLocationKey.getValue() << ", '" 
        << (databaseConnection->escapeAQSQLString(m_strScheduleName)).c_str() << "', " 
        << m_tmStartTime << ", " 
        << m_tmEndTime << ", '" 
        << (databaseConnection->escapeAQSQLString(m_strIntervalType)).c_str() << "', " 
        << m_bEnable << ", " 
        << m_bAutoDel << ", " 
        << m_ulDvaMsgKey.getValue() << ", " 
        << m_ulCyclicTimes.getValue() << ", " 
        << m_ulPriorityGroup.getValue() << ", '" 
        << (databaseConnection->escapeAQSQLString(m_strSourceType)).c_str() << "', '" 
        << (databaseConnection->escapeAQSQLString(m_strStatus)).c_str() << "')";*/

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, PA_SCHEDULE_BROADCAST_INSERT_110004, sequenceNumber, m_ulLocationKey.getValue(), (databaseConnection->escapeAQSQLString(m_strScheduleName)).c_str(),
		m_tmStartTime, m_tmEndTime, (databaseConnection->escapeAQSQLString(m_strIntervalType)).c_str(),m_bEnable, m_bAutoDel, m_ulDvaMsgKey.getValue(), m_ulCyclicTimes.getValue(),
		m_ulPriorityGroup.getValue(), (databaseConnection->escapeAQSQLString(m_strSourceType)).c_str(), (databaseConnection->escapeAQSQLString(m_strStatus)).c_str());

    databaseConnection->executeModification(sql);

    /*sql.str("");*/

    // Obtain the newly created record
   /* sql << "select " 
        << KEY_COL << " "
        << "from " << PA_SCHEDULE_BROADCAST_TABLE << " where "
        << KEY_COL << " = " << sequenceNumber;*/

	databaseConnection->prepareSQLStatement(sql, PA_SCHEDULE_BROADCAST_SELECT_110001, sequenceNumber);

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

    for ( std::vector<unsigned long>::iterator itLoop = m_lstZoneKeys.begin(); m_lstZoneKeys.end() != itLoop; ++itLoop )
    {
        /*sql.str("");
        sql << "insert into " << PA_SCHEDULE_BROADCAST_ZONES_TABLE << " ( " 
            << PA_SCHEDULE_BROADCAST_ZONES_SCHEDULE_BROADCAST_KEY_COL << ", "
            << PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL << " ) values ( " << m_key << ", " << (*itLoop) << " )";*/

		databaseConnection->prepareSQLStatement(sql, PA_SCHEDULE_BROADCAST_ZONES_INSERT_120002, m_key, (*itLoop));

        databaseConnection->executeModification(sql);
    }

    FUNCTION_EXIT;
}

void PaScheduleBroadcastHelper::validateLocationKeyExistence() //throw(TA_Base_Core::TransactiveException)
{
	/*
    std::ostringstream sql;
    sql << "select PKEY from LOCATION where PKEY = " << m_ulLocationKey.getValue();
	*/
    
    std::vector<std::string> columnNames;
    columnNames.clear();
    columnNames.push_back("PKEY");

    TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

	TA_Base_Core::SQLStatement sql;
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

void PaScheduleBroadcastHelper::reloadAll()
{
    reload();
    reloadZones();
}

void PaScheduleBroadcastHelper::reloadZones()
{
    /*std::ostringstream sql;

    sql << "select " << PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL
        << " from " << PA_SCHEDULE_BROADCAST_ZONES_TABLE
        << " where " << PA_SCHEDULE_BROADCAST_ZONES_SCHEDULE_BROADCAST_KEY_COL 
        << " = "<< m_key;*/     // Filter out non matching group IDs

    // get a connection to the database
    TA_Base_Core::IDatabase* databaseConnection = 
        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

	SQLStatement sql;
	databaseConnection->prepareSQLStatement(sql, PA_SCHEDULE_BROADCAST_ZONES_SELECT_120003, m_key);

    // Set up the columnNames vector to be passed to executeQuery()
    std::vector<std::string> columnNames;
    columnNames.push_back(PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL);

    // Execute the query. The method can throw a DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

    m_lstZoneKeys.clear();
    do
    {
        if ( 0 == data->getNumRows() )
        {
            delete data;
            data = NULL;
            break;
        }

        for (unsigned long i = 0; i < data->getNumRows() ; ++i )
        {   
            unsigned long key = data->getUnsignedLongData(i, PA_SCHEDULE_BROADCAST_ZONES_ZONE_KEY_COL);
            m_lstZoneKeys.push_back(key);
        }

        delete data;
        data = NULL;
    }
    while( databaseConnection->moreData(data) );

    m_bZonesLoaded = true;
}

} // closes TA_IRS_Core