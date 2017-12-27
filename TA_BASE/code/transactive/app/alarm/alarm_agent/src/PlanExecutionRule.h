/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/PlanExecutionRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Plan Execution Rules can be configured such that specific alarms can
  * trigger the execution of a plan
  */
#if !defined(PLANEXECUTIONRULE)
#define PLANEXECUTIONRULE

#include <string>
#include "app/alarm/alarm_agent/src/AbstractRule.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "core/naming/src/NamedObject.h"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef; // Forward declaration
}

namespace TA_Base_App
{

    class PlanExecutionRule : public virtual AbstractRule
    {

    public:

        /**
          * Constructor
          *
          * @param The entity key associated with alarms that are subject to the rule
          * @param The entity type of associated with alarms that are subject to the rule
          * @param The alarm type of alarms that are subject to the rule
          * @param The trigger (was the alarm raised, closed, or acknowledged)
          * @param The rule key (as stored in the database) - used for online updates.
          * @param The full path of the plan to execute
          */
        PlanExecutionRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                          TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key, const std::string& planToRun );

        /** 
          * Destructor
          */
        virtual ~PlanExecutionRule();

        /** 
          * process
          *
          * This method will execute the plan if the supplied alarm is subject to the
          * rule.
          *
          * @param The alarm
          * @param The trigger (was the alarm raised, closed, or acknowledged)
          */
        void process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                      TA_Base_App::AbstractRule::ETriggerType trigger);
		virtual const std::string& getRuleType() const
		{
			return EXECUTE_PLAN;
		}

    private:

        std::string m_planToRun; // fully qualified plan path in the format "[/<category1>/<categoryN>]/<planName>"

        TA_Base_Bus::PlanAgentLibrary* m_planAgent;
    };    
}

#endif // !defined(PLANEXECUTIONRULE)
