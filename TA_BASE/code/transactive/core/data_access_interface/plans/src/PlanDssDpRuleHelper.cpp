/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanDssDpRuleHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PlanDssDpRuleHelper is an object that is held by PlanDssDpRule and ConfigPlanDssDpRule objects. 
  * It contains all data used by PlanDssDpRuleGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PlanDssDpRule and ConfigPlanDssDpRule.
  *
  */

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/plans/src/PlanDssDpRuleHelper.h"
#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/plans/src/PlanDssDpRuleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    const char* const PLAN_DSS_DP_RULE_TABLE_NAME = "PLAN_DSS_DP_RULE";

    const char* const KEY_COL = "PKEY";
    const char* const TRIGGER_ENTITY_KEY_COL = "TRIGGER_ENTITY_KEY";
    const char* const ACTIVE_PLAN_KEY_COL = "ACTIVE_PLAN_KEY";
    const char* const MINIMUM_INTERVAL_COL = "MINIMUM_INTERVAL";
    const char* const TOLERANT_TIME_FOR_ACTIVE_COL = "TOLERANT_TIME_FOR_ACTIVE";
    const char* const VALID_TIME_IN_DAY_COL = "VALID_TIME_IN_DAY";
    const char* const VALID_DAYS_IN_WEEK_COL = "VALID_DAYS_IN_WEEK";
    const char* const ACTIVATE_VALUE_COL = "ACTIVATE_VALUE";
    const char* const NEED_DEACTIVATE_COL = "NEED_DEACTIVATE";
    const char* const ALLOW_MULTIPLE_COL = "ALLOW_MULTIPLE";
    const char* const ENABLE_COL = "ENABLE";
    const char* const DENY_MULTIPLE_STRATEGY_COL = "DENY_MULTIPLE_STRATEGY";
    const char* const LOCATION_KEY_COL = "LOCATION_KEY";
    const char* const DATEMODIFIED_COL = "DATE_MODIFIED";
    const char* const DATECREATED_COL = "DATE_CREATED";

	PlanDssDpRuleHelper::PlanDssDpRuleHelper( const PlanDssDpRuleHelper& thePlanDssDpRuleHelper) : 
    m_ulKey(DatabaseKey::getInvalidKey()), 
    m_ulLocationKey(thePlanDssDpRuleHelper.m_ulLocationKey),
    m_ulTriggerEntityKey(thePlanDssDpRuleHelper.m_ulTriggerEntityKey),
    m_ulActivePlanKey(thePlanDssDpRuleHelper.m_ulActivePlanKey),
    m_ulMinimumInterval(thePlanDssDpRuleHelper.m_ulMinimumInterval),
    m_ulTolerantTimeForActive(thePlanDssDpRuleHelper.m_ulTolerantTimeForActive),
    m_ulValidStartTimeInDay(thePlanDssDpRuleHelper.m_ulValidStartTimeInDay),
    m_ulValidEndTimeInDay(thePlanDssDpRuleHelper.m_ulValidEndTimeInDay),
    m_strLocationName(thePlanDssDpRuleHelper.m_strLocationName),
    m_strTriggerEntityName(thePlanDssDpRuleHelper.m_strTriggerEntityName),
    m_strActivePlanName(thePlanDssDpRuleHelper.m_strActivePlanName),
    m_bValidSunday(thePlanDssDpRuleHelper.m_bValidSunday),
    m_bValidMonday(thePlanDssDpRuleHelper.m_bValidMonday),
    m_bValidTuesday(thePlanDssDpRuleHelper.m_bValidTuesday),
    m_bValidWednesday(thePlanDssDpRuleHelper.m_bValidWednesday),
    m_bValidThursday(thePlanDssDpRuleHelper.m_bValidThursday),
    m_bValidFriday(thePlanDssDpRuleHelper.m_bValidFriday),
    m_bValidSaturday(thePlanDssDpRuleHelper.m_bValidSaturday),
    m_bActivateValue(thePlanDssDpRuleHelper.m_bActivateValue),
    m_bNeedDeactivate(thePlanDssDpRuleHelper.m_bNeedDeactivate),
    m_bAllowMultiple(thePlanDssDpRuleHelper.m_bAllowMultiple),
    m_bEnable(thePlanDssDpRuleHelper.m_bEnable),
    m_nDenyMultipleStrategy(thePlanDssDpRuleHelper.m_nDenyMultipleStrategy),
    m_dateCreated(0),
    m_dateModified(0),
    m_isValidData(false),
    m_isNew(true)
    {
	}


    PlanDssDpRuleHelper::PlanDssDpRuleHelper(const unsigned long key) : 
    m_ulKey(key),	
    m_ulLocationKey(DatabaseKey::getInvalidKey()),
    m_ulTriggerEntityKey(DatabaseKey::getInvalidKey()),
    m_ulActivePlanKey(DatabaseKey::getInvalidKey()),
    m_ulMinimumInterval(0u),
    m_ulTolerantTimeForActive(0u),
    m_ulValidStartTimeInDay(0u),
    m_ulValidEndTimeInDay(86400u),
    m_strLocationName(""),
    m_strTriggerEntityName(""),
    m_strActivePlanName(""),
    m_bValidSunday(true),
    m_bValidMonday(true),
    m_bValidTuesday(true),
    m_bValidWednesday(true),
    m_bValidThursday(true),
    m_bValidFriday(true),
    m_bValidSaturday(true),
    m_bActivateValue(true),
    m_bNeedDeactivate(false),
    m_bAllowMultiple(true),
    m_bEnable(true),
    m_nDenyMultipleStrategy(StrategyNoDeny),
    m_dateCreated(0),
    m_dateModified(0),
    m_isValidData(false),
    m_isNew(false)
    {
    }


    PlanDssDpRuleHelper::PlanDssDpRuleHelper(unsigned long row, TA_Base_Core::IData& data) : 
    m_ulKey(data.getUnsignedLongData(row, KEY_COL)),	
    m_ulLocationKey(DatabaseKey::getInvalidKey()),
    m_ulTriggerEntityKey(DatabaseKey::getInvalidKey()),
    m_ulActivePlanKey(DatabaseKey::getInvalidKey()),
    m_ulMinimumInterval(0u),
    m_ulTolerantTimeForActive(0u),
    m_ulValidStartTimeInDay(0u),
    m_ulValidEndTimeInDay(86400u),
    m_strLocationName(""),
    m_strTriggerEntityName(""),
    m_strActivePlanName(""),
    m_bValidSunday(true),
    m_bValidMonday(true),
    m_bValidTuesday(true),
    m_bValidWednesday(true),
    m_bValidThursday(true),
    m_bValidFriday(true),
    m_bValidSaturday(true),
    m_bActivateValue(true),
    m_bNeedDeactivate(false),
    m_bAllowMultiple(true),
    m_bEnable(true),
    m_nDenyMultipleStrategy(StrategyNoDeny),
    m_dateCreated(0),
    m_dateModified(0),
    m_isValidData(false),
    m_isNew(false)
    {
        reloadUsing(row, data);
    }


    PlanDssDpRuleHelper::PlanDssDpRuleHelper() : 
    m_ulKey(DatabaseKey::getInvalidKey()), 
    m_ulLocationKey(DatabaseKey::getInvalidKey()),
    m_ulTriggerEntityKey(DatabaseKey::getInvalidKey()),
    m_ulActivePlanKey(DatabaseKey::getInvalidKey()),
    m_ulMinimumInterval(0u),
    m_ulTolerantTimeForActive(0u),
    m_ulValidStartTimeInDay(0u),
    m_ulValidEndTimeInDay(86400u),
    m_strLocationName(""),
    m_strTriggerEntityName(""),
    m_strActivePlanName(""),
    m_bValidSunday(true),
    m_bValidMonday(true),
    m_bValidTuesday(true),
    m_bValidWednesday(true),
    m_bValidThursday(true),
    m_bValidFriday(true),
    m_bValidSaturday(true),
    m_bActivateValue(true),
    m_bNeedDeactivate(false),
    m_bAllowMultiple(true),
    m_nDenyMultipleStrategy(StrategyNoDeny),
    m_dateCreated(0),
    m_dateModified(0),
    m_isValidData(false),
    m_isNew(true)
    {
    }


    PlanDssDpRuleHelper::~PlanDssDpRuleHelper()
    {
    }
  

    unsigned long PlanDssDpRuleHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ulKey;
    }


    unsigned long PlanDssDpRuleHelper::getLocationKey()
    {
        FUNCTION_ENTRY("getLocationKey" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ulLocationKey;
    }


    void PlanDssDpRuleHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_ulLocationKey = locationKey;
        m_strLocationName = "";

        FUNCTION_EXIT;
    }


    unsigned long PlanDssDpRuleHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );

        if ( m_isNew )
        {
            TA_THROW( DataException("The data must be written to the database before the date created can be retrieved", DataException::NO_VALUE,"" ) );
        }

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    unsigned long PlanDssDpRuleHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );

        if ( m_isNew )
        {
            TA_THROW( DataException("The data must be written to the database before the date modified can be retrieved", DataException::NO_VALUE,"" ) );
        }

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }


    void PlanDssDpRuleHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        if ( m_isNew )
        {
            TA_THROW( DataException("Attempted to call invalidate() on a new PlanDssDpRule", DataException::NO_VALUE,"" ) );
        }

        m_isValidData = false;
        m_strLocationName = "";
        m_strActivePlanName = "";
        m_strTriggerEntityName = "";

        FUNCTION_EXIT;
    }


    void PlanDssDpRuleHelper::reloadNames()
    {
        if ( m_strLocationName.empty() && DatabaseKey::isValidKey( m_ulLocationKey ))
        {
            ILocation* pLocation = NULL;

            try
            {
                pLocation = LocationAccessFactory::getInstance().getLocationByKey( m_ulLocationKey );
            }
            catch ( const std::exception& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception when getLocationByKey" );
            }

            if ( NULL != pLocation )
            {
                m_strLocationName = pLocation->getDisplayName();

                delete pLocation;
                pLocation = NULL;
            }
        }

        if ( m_strTriggerEntityName.empty() && DatabaseKey::isValidKey( m_ulTriggerEntityKey ))
        {
            IEntityData* pEntity = NULL;

            try
            {
                pEntity = EntityAccessFactory::getInstance().getEntity( m_ulTriggerEntityKey );
            }
            catch ( const std::exception& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception when getEntity" );
            }

            if ( NULL != pEntity )
            {
                m_strTriggerEntityName = pEntity->getName();

                delete pEntity;
                pEntity = NULL;
            }
        }

        if ( m_strActivePlanName.empty() && DatabaseKey::isValidKey( m_ulActivePlanKey ))
        {
            try
            {
                m_strActivePlanName = PlanAccessFactory::getInstance().getPlanPathByNodeId( m_ulActivePlanKey );
            }
            catch ( const std::exception& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception when getEntity" );
            }
        }
    }


    void PlanDssDpRuleHelper::reload()
    {
        if ( m_isNew )
        {
            TA_THROW( DataException("This DSS rule does not yet exist in the database.",DataException::CANNOT_DELETE,"" ) );
        }

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Plans_Cd, Read);

        /*std::string strSql  = databaseConnection->prepareSQLStatement(PLAN_DSS_DP_RULE_SELECT_79005, m_ulKey);*/
		SQLStatement  strSql;
		databaseConnection->prepareSQLStatement(strSql, PLAN_DSS_DP_RULE_SELECT_79005, m_ulKey);

        // Set up the columnNames vector to be passed to executeQuery()       
        std::vector<std::string> columnNames;
        columnNames.push_back(LOCATION_KEY_COL);
        columnNames.push_back(TRIGGER_ENTITY_KEY_COL);
        columnNames.push_back(ACTIVE_PLAN_KEY_COL);
        columnNames.push_back(MINIMUM_INTERVAL_COL);
        columnNames.push_back(TOLERANT_TIME_FOR_ACTIVE_COL);
        columnNames.push_back(VALID_TIME_IN_DAY_COL);
        columnNames.push_back(VALID_DAYS_IN_WEEK_COL);
        columnNames.push_back(ACTIVATE_VALUE_COL);
        columnNames.push_back(NEED_DEACTIVATE_COL);
        columnNames.push_back(ALLOW_MULTIPLE_COL);
        columnNames.push_back(DENY_MULTIPLE_STRATEGY_COL);
        columnNames.push_back(ENABLE_COL);
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (0 == data->getNumRows()) 
        {
            // TrackElement record not found
            std::ostringstream reason;
            reason  << "No record found for DSS rule with key = " << m_ulKey;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(), DataException::NO_VALUE, "DSS rule Key" ) );
        }
        else if ( 1 < data->getNumRows() )
        {
            // TrackElement record not found
            std::ostringstream reason;
            reason  << "More than one record found for DSS rule with key = " << m_ulKey;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(), DataException::NOT_UNIQUE, "DSS rule Key" ) );
        }

        reloadUsing(0, *data);
    }


    void PlanDssDpRuleHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing" );
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_ulLocationKey = data.getUnsignedLongData( row, LOCATION_KEY_COL );

        m_ulTriggerEntityKey = data.getUnsignedLongData( row, TRIGGER_ENTITY_KEY_COL );
        m_ulActivePlanKey = data.getUnsignedLongData( row, ACTIVE_PLAN_KEY_COL );
        m_ulMinimumInterval = data.getUnsignedLongData( row, MINIMUM_INTERVAL_COL );
        m_ulTolerantTimeForActive = data.getUnsignedLongData( row, TOLERANT_TIME_FOR_ACTIVE_COL );

        decodeTimeInDay( data.getUnsignedLongData( row, VALID_TIME_IN_DAY_COL ));
        decodeDaysInWeek( data.getUnsignedLongData( row, VALID_DAYS_IN_WEEK_COL ));

        m_bActivateValue = data.getBooleanData( row, ACTIVATE_VALUE_COL );
        m_bNeedDeactivate = data.getBooleanData( row, NEED_DEACTIVATE_COL );
        m_bAllowMultiple = data.getBooleanData( row, ALLOW_MULTIPLE_COL );
        m_bEnable = data.getBooleanData( row, ENABLE_COL );
        m_nDenyMultipleStrategy = static_cast<int>(data.getUnsignedLongData( row, DENY_MULTIPLE_STRATEGY_COL ));

        m_dateCreated = data.getDateData( row, DATECREATED_COL );

        try
        {
            m_dateModified = data.getDateData( row, DATEMODIFIED_COL );

            if (data.getNullTime() == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (DataException& ex)
        {
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (DataException::NO_VALUE != ex.getFailType())
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


    void PlanDssDpRuleHelper::writePlanDssDpRuleData() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY( "writePlanDssDpRuleData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if ( DatabaseKey::getInvalidKey() == m_ulLocationKey)
        {
            fieldNames.push_back( LOCATION_KEY_COL );
        }
        if ( DatabaseKey::getInvalidKey() == m_ulTriggerEntityKey)
        {
            fieldNames.push_back( TRIGGER_ENTITY_KEY_COL );
        }
        if ( DatabaseKey::getInvalidKey() == m_ulActivePlanKey)
        {
            fieldNames.push_back( ACTIVE_PLAN_KEY_COL );
        }
        if ( m_ulValidStartTimeInDay > m_ulValidEndTimeInDay )
        {
            fieldNames.push_back( VALID_TIME_IN_DAY_COL );
        }
        if ( !m_bAllowMultiple && ( StrategyNoDeny == m_nDenyMultipleStrategy ))
        {
            fieldNames.push_back( DENY_MULTIPLE_STRATEGY_COL );
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(DataConfigurationException("PlanDssDpRule data not fully specified. PlanDssDpRule cannot be written to database", fieldNames));
        }
        
        // Write the PlanDssDpRule to the database

        try 
        {
            // Need to check if this is a new PlanDssDpRule or an existing PlanDssDpRule - 
            // as a new PlanDssDpRule will be inserted, while an existing PlanDssDpRule will be updated
            if (m_isNew) // This is a new PlanDssDpRule
            {
                addNewPlanDssDpRule();
            }
            else // This is an existing PlanDssDpRule
            {
                modifyExistingPlanDssDpRule();
            }
        }
        catch (...)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PlanDssDpRule data not fully specified. PlanDssDpRule cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }
   

    void PlanDssDpRuleHelper::deletePlanDssDpRule()
    {
        FUNCTION_ENTRY("deletePlanDssDpRule");

        if ( m_isNew )
        {
            TA_THROW( DataException("This PlanDssDpRule does not yet exist in the database, and therefore cannot be deleted",
                DataException::CANNOT_DELETE,"Plan DSS rule Key" ) );
        }

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Plans_Cd, Write);
        //std::ostringstream sql;        

        // Good to go...
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PLAN_DSS_DP_RULE_DELETE_79008, m_ulKey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void PlanDssDpRuleHelper::modifyExistingPlanDssDpRule() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPlanDssDpRule");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "DSS rule %u already exists. It's data will be updated.", m_ulKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Plans_Cd, Write);

		SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, PLAN_DSS_DP_RULE_UPDATE_79007, m_ulLocationKey, 
            m_ulTriggerEntityKey, m_ulActivePlanKey, m_ulMinimumInterval, m_ulTolerantTimeForActive, encodeTimeInDay(), encodeDaysInWeek(), 
            ( m_bActivateValue ? 1 : 0 ), ( m_bNeedDeactivate ? 1 : 0 ), ( m_bAllowMultiple ? 1 : 0 ), m_nDenyMultipleStrategy, ( m_bEnable ? 1 : 0 ), m_ulKey);

        databaseConnection->executeModification(strSql);

        // now need to update the date fields
		databaseConnection->prepareSQLStatement(strSql, PLAN_DSS_DP_RULE_SELECT_79004, m_ulKey);

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 == data->getNumRows()) 
        {
            // DSS rule not found
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Plan DSS rule key = " << m_ulKey;			
            TA_THROW( DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Plan DSS rule Key" ) );
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


    void PlanDssDpRuleHelper::addNewPlanDssDpRule() 
                    //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPlanDssDpRule");

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Plans_Cd, Write);

        // We must retrieve the key first so we know where we are inserting the data. No other fields
        // are unique so this is our only identifier.		
        //std::string strSql  = databaseConnection->prepareSQLStatement(PLAN_DSS_DP_RULE_SELECT_79001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PLAN_DSS_DP_RULE_SELECT_79001);

        std::vector<std::string> columnNames;
        columnNames.push_back( KEY_COL );

        // Execute the query.
        {
            std::auto_ptr<IData> ptrSequence( databaseConnection->executeQuery( strSql, columnNames ));
            if ( 0 == ptrSequence->getNumRows() )
            {
                TA_THROW( DataException("Could not retrieve a new key for the DSS rule",DataException::NO_VALUE,"pkey") );
            }

            m_ulKey = ptrSequence->getUnsignedLongData( 0, KEY_COL );
        }

		databaseConnection->prepareSQLStatement(strSql, PLAN_DSS_DP_RULE_INSERT_79006, m_ulKey,  m_ulLocationKey, 
			m_ulTriggerEntityKey, m_ulActivePlanKey, m_ulMinimumInterval, m_ulTolerantTimeForActive, encodeTimeInDay(), encodeDaysInWeek(), 
			( m_bActivateValue ? 1 : 0 ), ( m_bNeedDeactivate ? 1 : 0 ), ( m_bAllowMultiple ? 1 : 0 ), m_nDenyMultipleStrategy, ( m_bEnable ? 1 : 0 ) );

        databaseConnection->executeModification(strSql);

		databaseConnection->prepareSQLStatement(strSql, PLAN_DSS_DP_RULE_SELECT_79003,  m_ulKey);

        columnNames.clear();
        columnNames.push_back(DATECREATED_COL);

        {
            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.
            // We are responsible for deleting the returned IData object when we're done with it
            std::auto_ptr<IData> ptrData( databaseConnection->executeQuery( strSql,columnNames ));

            if (0 == ptrData->getNumRows()) 
            {
                // PA DVA Message Version record not found
                std::ostringstream reasonMessage;
		        reasonMessage << "No data found for DSS DataPoint rule, key = " << m_ulKey;			
                TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA DVA Message Version key" ) );
            }

            // Retrieve the pkey as an unsigned long. There should only be a single returned item 
            // (as any other circumstance is specifically prevented by the above checks).
            // This method can throw a DataException
            m_dateCreated = ptrData->getDateData(0,DATECREATED_COL);
        }

        FUNCTION_EXIT;
    }

    unsigned long PlanDssDpRuleHelper::getKey() const
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_ulKey;
    }

    bool PlanDssDpRuleHelper::isNew() const
    {
        FUNCTION_ENTRY("isNew");

        FUNCTION_EXIT;
        return m_isNew;
    }

    void PlanDssDpRuleHelper::setTriggerEntityKey( const unsigned long ulTriggerEntityKey )
    {
        FUNCTION_ENTRY("setTriggerEntityKey");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_ulTriggerEntityKey = ulTriggerEntityKey;
        m_strTriggerEntityName = "";

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setActivePlanKey( const unsigned long ulActivePlanKey )
    {
        FUNCTION_ENTRY("setActivePlanKey");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_ulActivePlanKey = ulActivePlanKey;
        m_strActivePlanName = "";

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setMinimumInterval( const unsigned long ulMinimumInterval )
    {
        FUNCTION_ENTRY("setMinimumInterval");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_ulMinimumInterval = ulMinimumInterval;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setTolerantTimeForActive( const unsigned long ulTolerantTimeForActive )
    {
        FUNCTION_ENTRY("setTolerantTimeForActive");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_ulTolerantTimeForActive = ulTolerantTimeForActive;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidStartTimeInDay( const unsigned long ulValidStartTimeInDay )
    {
        FUNCTION_ENTRY("setValidStartTimeInDay");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_ulValidStartTimeInDay = ulValidStartTimeInDay;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidEndTimeInDay( const unsigned long ulValidEndTimeInDay )
    {
        FUNCTION_ENTRY("setValidEndTimeInDay");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_ulValidEndTimeInDay = ulValidEndTimeInDay;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidSunday( const bool bValidSunday )
    {
        FUNCTION_ENTRY("setValidSunday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bValidSunday = bValidSunday;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidMonday( const bool bValidMonday )
    {
        FUNCTION_ENTRY("setValidMonday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bValidMonday = bValidMonday;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidTuesday( const bool bValidTuesday )
    {
        FUNCTION_ENTRY("setValidTuesday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bValidTuesday = bValidTuesday;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidWednesday( const bool bValidWednesday )
    {
        FUNCTION_ENTRY("setValidWednesday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bValidWednesday = bValidWednesday;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidThursday( const bool bValidThursday )
    {
        FUNCTION_ENTRY("setValidThursday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bValidThursday = bValidThursday;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidFriday( const bool bValidFriday )
    {
        FUNCTION_ENTRY("setValidSaturday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bValidFriday = bValidFriday;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setValidSaturday( const bool bValidSaturday )
    {
        FUNCTION_ENTRY("setValidSaturday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bValidSaturday = bValidSaturday;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setActivateValue( const bool bActivateValue )
    {
        FUNCTION_ENTRY("setActivateValue");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bActivateValue = bActivateValue;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setNeedDeactivate( const bool bNeedDeactivate )
    {
        FUNCTION_ENTRY("setNeedDeactivate");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bNeedDeactivate = bNeedDeactivate;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setAllowMultiple( const bool bAllowMultiple )
    {
        FUNCTION_ENTRY("setAllowMultiple");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bAllowMultiple = bAllowMultiple;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setEnable( const bool bEnable )
    {
        FUNCTION_ENTRY("setEnable");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_bEnable = bEnable;

        FUNCTION_EXIT;
    }

    void PlanDssDpRuleHelper::setDenyMultipleStrategy( const int nDenyMultipleStrategy )
    {
        FUNCTION_ENTRY("setDenyMultipleStrategy");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_nDenyMultipleStrategy = nDenyMultipleStrategy;

        FUNCTION_EXIT;
    }

    unsigned long PlanDssDpRuleHelper::getTriggerEntityKey()
    {
        FUNCTION_ENTRY("getTrigerEntityKey");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ulTriggerEntityKey;
    }

    unsigned long PlanDssDpRuleHelper::getActivePlanKey()
    {
        FUNCTION_ENTRY("getActivePlanKey");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ulActivePlanKey;
    }

    unsigned long PlanDssDpRuleHelper::getMinimumInterval()
    {
        FUNCTION_ENTRY("getMinimumInterval");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ulMinimumInterval;
    }

	std::string PlanDssDpRuleHelper::getLocationName()
	{
		FUNCTION_ENTRY("getLocationName");

		if ( m_strLocationName.empty() )
		{
			reloadNames();
		}

		FUNCTION_EXIT;
		return m_strLocationName;
	}

	std::string PlanDssDpRuleHelper::getTriggerEntityName()
    {
        FUNCTION_ENTRY("getTriggerEntityName");

        if ( m_strTriggerEntityName.empty() )
        {
            reloadNames();
        }

        FUNCTION_EXIT;
        return m_strTriggerEntityName;
	}

	std::string PlanDssDpRuleHelper::getActivePlanName()
    {
        FUNCTION_ENTRY("getActivePlanName");

        if ( m_strActivePlanName.empty() )
        {
            reloadNames();
        }

        FUNCTION_EXIT;
        return m_strActivePlanName;
	}

    unsigned long PlanDssDpRuleHelper::getTolerantTimeForActive()
    {
        FUNCTION_ENTRY("getTolerantTimeForActive");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ulTolerantTimeForActive;
    }

    unsigned long PlanDssDpRuleHelper::getValidStartTimeInDay()
    {
        FUNCTION_ENTRY("getValidStartTimeInDay");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ulValidStartTimeInDay;
    }

    unsigned long PlanDssDpRuleHelper::getValidEndTimeInDay()
    {
        FUNCTION_ENTRY("getValidEndTimeInDay");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_ulValidEndTimeInDay;
    }

    bool PlanDssDpRuleHelper::getValidSunday()
    {
        FUNCTION_ENTRY("getValidSunday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bValidSunday;
    }

    bool PlanDssDpRuleHelper::getValidMonday()
    {
        FUNCTION_ENTRY("getValidMonday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bValidMonday;
    }

    bool PlanDssDpRuleHelper::getValidTuesday()
    {
        FUNCTION_ENTRY("getValidTuesday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bValidTuesday;
    }

    bool PlanDssDpRuleHelper::getValidWednesday()
    {
        FUNCTION_ENTRY("getValidWednesday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bValidWednesday;
    }

    bool PlanDssDpRuleHelper::getValidThursday()
    {
        FUNCTION_ENTRY("getValidThursday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bValidThursday;
    }

    bool PlanDssDpRuleHelper::getValidFriday()
    {
        FUNCTION_ENTRY("getValidFriday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bValidFriday;
    }

    bool PlanDssDpRuleHelper::getValidSaturday()
    {
        FUNCTION_ENTRY("getValidSaturday");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bValidSaturday;
    }

    bool PlanDssDpRuleHelper::getActivateValue()
    {
        FUNCTION_ENTRY("getActivateValue");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bActivateValue;
    }

    bool PlanDssDpRuleHelper::getNeedDeactivate()
    {
        FUNCTION_ENTRY("getNeedDeactivate");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bNeedDeactivate;
    }

    bool PlanDssDpRuleHelper::getAllowMultiple()
    {
        FUNCTION_ENTRY("getAllowMultiple");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bAllowMultiple;
    }

    bool PlanDssDpRuleHelper::getEnable()
    {
        FUNCTION_ENTRY("getEnable");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_bEnable;
    }

    int PlanDssDpRuleHelper::getDenyMultipleStrategy()
    {
        FUNCTION_ENTRY("getDenyMultipleStrategy");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_nDenyMultipleStrategy;
    }

    void PlanDssDpRuleHelper::decodeDaysInWeek( const unsigned long ulEncoded )
    {
        m_bValidSunday = ( 0u != ( ulEncoded & 0x0001u ));
        m_bValidMonday = ( 0u != ( ulEncoded & 0x0002u ));
        m_bValidTuesday = ( 0u != ( ulEncoded & 0x0004u ));
        m_bValidWednesday = ( 0u != ( ulEncoded & 0x0008u ));
        m_bValidThursday = ( 0u != ( ulEncoded & 0x0010u ));
        m_bValidFriday = ( 0u != ( ulEncoded & 0x0020u ));
        m_bValidSaturday = ( 0u != ( ulEncoded & 0x0040u ));
    }

    unsigned long PlanDssDpRuleHelper::encodeDaysInWeek()
    {
        unsigned long ulEncoded = 0u;

        ulEncoded |= m_bValidSunday ? 0x0001u : 0u;
        ulEncoded |= m_bValidMonday ? 0x0002u : 0u;
        ulEncoded |= m_bValidTuesday ? 0x0004u : 0u;
        ulEncoded |= m_bValidWednesday ? 0x0008u : 0u;
        ulEncoded |= m_bValidThursday ? 0x0010u : 0u;
        ulEncoded |= m_bValidFriday ? 0x0020u : 0u;
        ulEncoded |= m_bValidSaturday ? 0x0040u : 0u;

        return ulEncoded;
    }

    void PlanDssDpRuleHelper::decodeTimeInDay( const unsigned long ulEncoded )
    {
        m_ulValidStartTimeInDay = (( ulEncoded & 0xFFFF0000u ) >> 16u ) * 60u;
        m_ulValidEndTimeInDay = ( ulEncoded & 0x0000FFFFu ) * 60u;
    }

    unsigned long PlanDssDpRuleHelper::encodeTimeInDay()
    {
        unsigned long ulEncoded = 0u;

        ulEncoded |= ( m_ulValidEndTimeInDay / 60u );
        ulEncoded |= (( m_ulValidStartTimeInDay / 60u ) << 16u );

        return ulEncoded;
    }
} // closes TA_Base_Core

