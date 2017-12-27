/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanDssDpRuleAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by: $Author: Ouyang $
  * 
  * PlanDssDpRuleAccessFactory is a singleton that is used to retrieve PlanDssDpRule objects either from the
  * database or newly created. All PlanDssDpRule objects returned will adhere to the IPlanDssDpRule interface.
  */


#include "core/data_access_interface/plans/src/PlanDssDpRule.h"
#include "core/data_access_interface/plans/src/ConfigPlanDssDpRule.h"
#include "core/data_access_interface/plans/src/PlanDssDpRuleAccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    PlanDssDpRuleAccessFactory* PlanDssDpRuleAccessFactory::m_instance = 0;

    const char* const KEY_COL = "PKEY";
    const char* const LOCATION_KEY_COL = "LOCATION_KEY";
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
    const char* const DATECREATED_COL = "DATE_CREATED";
    const char* const DATEMODIFIED_COL = "DATE_MODIFIED";

    PlanDssDpRuleAccessFactory& PlanDssDpRuleAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PlanDssDpRuleAccessFactory();
        }
        return *m_instance;
    }

    void PlanDssDpRuleAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }

    PlanDssDpRuleAccessFactory::PlanDssDpRuleAccessFactory()
    {

    }

    IPlanDssDpRules PlanDssDpRuleAccessFactory::getPlanDssDpRuleByLocationKey(const unsigned long locationKey, const bool readWrite)
    {
        std::stringstream stmWhere;

        stmWhere << " where " << LOCATION_KEY_COL << " = " << locationKey << " and " << ENABLE_COL << " <> 0";

        return getPlanDssDpRulesReturnedFromStatement(stmWhere.str(), readWrite);
    }

    IPlanDssDpRule* PlanDssDpRuleAccessFactory::getPlanDssDpRule(const unsigned long key,const bool readWrite)
    {
        std::stringstream stmWhere;

        stmWhere << " where " << KEY_COL << " = " << key;

        IPlanDssDpRules vecRules = getPlanDssDpRulesReturnedFromStatement(stmWhere.str(), readWrite);

        if ( 0u == vecRules.size() ) // No entry found with the specified pkey
        {
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "No data found for item where PKEY = %d", key );
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if ( 1u < vecRules.size() ) // More than one entry found for the PKEY
        {
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "No data found for item where PKEY = %d", key);			
            TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }


        return vecRules.front();
    }

    IPlanDssDpRules PlanDssDpRuleAccessFactory::getAllPlanDssDpRules( const bool readWrite)
    {
        return getPlanDssDpRulesReturnedFromStatement("", readWrite);
    }

    IPlanDssDpRules PlanDssDpRuleAccessFactory::getPlanDssDpRulesReturnedFromStatement(const std::string& whereClause, const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Plans_Cd, Read);

        // create the SQL string to retrieve the name of the PlanDssDpRule
        // this is a check to ensure an PlanDssDpRule with the specified
        // PATDME_ID actually exists.
		SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, PLAN_DSS_DP_RULE_SELECT_79002, whereClause);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
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
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        IPlanDssDpRules vecRules;

        do
        {
            unsigned long rowCount = data->getNumRows();

            for (unsigned long i = 0; i < rowCount; ++i)
            {
                if (readWrite)
                {
                    vecRules.push_back(new ConfigPlanDssDpRule(i, *data));
                }
                else
                {
                    vecRules.push_back(new PlanDssDpRule(i, *data));
                }
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);

        // Return the PlanDssDpRule pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return vecRules;
    }

    IConfigPlanDssDpRule* PlanDssDpRuleAccessFactory::copyPlanDssDpRule(const IConfigPlanDssDpRule* PlanDssDpRuleToCopy)
    {
        FUNCTION_ENTRY("copyPlanDssDpRule");

        const ConfigPlanDssDpRule* cast = dynamic_cast<const ConfigPlanDssDpRule*>(PlanDssDpRuleToCopy);

        if ( cast ==  NULL)
        {
            TA_THROW(DataException("PlanDssDpRuleToCopy is NULL",DataException::NO_VALUE,""));
        }

        FUNCTION_EXIT;
        return new ConfigPlanDssDpRule(*cast);

    }

    IConfigPlanDssDpRule* PlanDssDpRuleAccessFactory::createPlanDssDpRule()
    {            
        return new ConfigPlanDssDpRule();    
    }

} // closes TA_Base_Core


