/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/PlanExecutionRuleHelper.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Provides an interface to Alarm Rules of type suppression in the database.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/alarm_rule/src/PlanExecutionRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/PlanExecutionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleActionParameterData.h"

#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/plans/src/IPlan.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"

#include <string>
#include <sstream>

using TA_Base_Core::DebugUtil;
typedef TA_Base_Core::IAlarmRuleActionParameterData::ActionParameterVector ActionParameterVector;


namespace TA_Base_Core
{
    const std::string PlanExecutionRuleHelper::PLAN_PATH_PARAMETER  = "PLAN_PATH";

    PlanExecutionRuleHelper::PlanExecutionRuleHelper()
        :
        AlarmRuleHelper(
            PlanExecutionRuleData::RULE_TYPE,
            IAlarmRuleData::RULE_TRIGGER_SUBMIT)
    {
        addNewParameter(PLAN_PATH_PARAMETER, "");
    }

    
    PlanExecutionRuleHelper::PlanExecutionRuleHelper(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
        const std::string& planPath)
        :
        AlarmRuleHelper(
            entityTypeKey,
            entityKey,
            alarmTypeKey,
            PlanExecutionRuleData::RULE_TYPE,
            ruleTrigger)
    {
        addNewParameter(PLAN_PATH_PARAMETER, planPath);
    }



    PlanExecutionRuleHelper::PlanExecutionRuleHelper(unsigned long pkey)
        :
        AlarmRuleHelper(pkey)
    {
    }


    PlanExecutionRuleHelper::PlanExecutionRuleHelper(
        PlanExecutionRuleHelper& original)
        :
        AlarmRuleHelper(
            original.getEntityTypeKey(),
            original.getEntityKey(),
            original.getAlarmType(),
            original.getRuleType(),
            original.getRuleTrigger())
    {
        addNewParameter(PLAN_PATH_PARAMETER, original.getPlanPath());
    }


    PlanExecutionRuleHelper::~PlanExecutionRuleHelper()
    {
    }


    std::string PlanExecutionRuleHelper::getPlanPath()
    {
        return retrieveParameter()->getValue();
    }


    void PlanExecutionRuleHelper::setPlanPath(const std::string& planPath)
    {
        // Create a Plan object based on the specified path. This will ensure the path is valid.
        std::auto_ptr<IPlan> plan(PlanAccessFactory::getInstance().getPlanByPath(planPath));
        retrieveParameter()->setValue(plan->getPath());
    }


    IAlarmRuleActionParameterData* PlanExecutionRuleHelper::retrieveParameter()
    {
        ActionParameterVector parameters = getParameters();

        // There should only be one parameter: plan path
        if(parameters.size() != 1)
        {
            std::ostringstream errorMessage;
            errorMessage <<
                "Wrong number of parameters in database " <<
                "for PlanExecution rule with key " << getKey();
            const char* message = errorMessage.str().c_str();

            TA_THROW(
                DataException(
                    message,
                    DataException::MISSING_MANDATORY,
                    PLAN_PATH_PARAMETER) );
        }
        
        // The single parameter should have PLAN_PATH_PARAMETER as its type
        if(parameters[0]->getType() != PLAN_PATH_PARAMETER)
        {
            std::ostringstream errorMessage;
            errorMessage <<
                "Parameter has wrong name: " << parameters[0]->getType() <<
                "for PlanExecution rule with key " << getKey();
            const char* message = errorMessage.str().c_str();

            TA_THROW(
                DataException(
                    message,
                    DataException::MISSING_MANDATORY,
                    PLAN_PATH_PARAMETER) );
        }

        return parameters[0];
    }




    void PlanExecutionRuleHelper::applyChanges()
    {
        // If the plan key value has not been set
        if(retrieveParameter()->getValue() == "")
        {
            // Throw an exception
            const char* message = "The plan parameter has not been set.";
            TA_THROW(
                DataException(
                    message, DataException::MISSING_MANDATORY, "Plan"));
        }

        AlarmRuleHelper::applyChanges();
    }




}// TA_Base_Core
