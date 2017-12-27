/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/PlanExecutionRuleHelper.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Provides an interface to Alarm Rules of type plan execution in the database.
  *
  */

#ifndef PLAN_EXECUTION_RULE_HELPER_H
#define PLAN_EXECUTION_RULE_HELPER_H


#include <string>
#include <vector>


#include "core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h"

namespace TA_Base_Core
{

    class PlanExecutionRuleHelper : public AlarmRuleHelper
    {

    public:
        
        // The string representing the type of the plan path parameter.
        static const std::string PLAN_PATH_PARAMETER;

        /** 
          * PlanExecutionRuleHelper
          *
          * Creates a new Plan Execution Alarm Rule.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleTrigger    The trigger for this alarm rule.
          * @param planPath       The full path of the plan.
          *
          */
        PlanExecutionRuleHelper(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
            const std::string& planPath);


        /** 
          * PlanExecutionRuleHelper
          *
          * Constructs a new plan execution rule helper with blank fields.
          *
          */
        PlanExecutionRuleHelper();


        /** 
          * PlanExecutionRuleHelper
          *
          * Creates a Plan Execution Alarm Rule from the database.
          *
          * @param pkey  the primary key of the plan execution alarm rule.
          */
        PlanExecutionRuleHelper(unsigned long pkey);



        /** 
          * PlanExecutionRuleHelper
          *
          * Creates a copy of the given object.
          *
          * @param original  the object that will be copied.
          */
        PlanExecutionRuleHelper(PlanExecutionRuleHelper& original);


        /** 
          * ~PlanExecutionRuleHelper
          *
          * Destructor
          *
          */
        virtual ~PlanExecutionRuleHelper();


        /** 
          * getPlanPath
          *
          * Gets the path of the plan in the format "[/<category1>/<categoryN>]/<planName>".
          *
          * @return the full path of the plan as a string.
          *
          */
        virtual std::string getPlanPath();


        /** 
          * setPlanPath
          *
          * Sets the path of the plan.
          *
          * @param planPath  the full path of the plan as a string.
          */
        virtual void setPlanPath(const std::string& planPath);


        /** 
          * applyChanges
          *
          * Overrides the parent class method so that a check can be made
          * to ensure all mandatory parameters are present.
          *
          */
        void applyChanges();


    protected:

        /** 
          * retrieveParameter
          *
          * Gets the plan execution type parameter for this alarm
          * rule from the database.
          *
          * @return the plan execution parameter
          *
          */
        virtual IAlarmRuleActionParameterData* PlanExecutionRuleHelper::retrieveParameter();

    private:

        const PlanExecutionRuleHelper& operator=(const PlanExecutionRuleHelper&);
    };


}// TA_Base_Core

#endif // PLAN_EXECUTION_RULE_HELPER_H
