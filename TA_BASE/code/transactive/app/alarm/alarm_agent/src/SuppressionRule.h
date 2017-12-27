/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/SuppressionRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Suppression rules can be configured for alarms that should be suppressed
  * (ignored by the Alarm Agent)
  */
#if !defined(SUPPRESSIONRULE)
#define SUPPRESSIONRULE

#include <string>
#include "app/alarm/alarm_agent/src/AbstractRule.h"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef; // Forward declaration
    struct AlarmRuleCorbaDef; // Forward declaration
}

namespace TA_Base_App
{

    class SuppressionRule : public virtual AbstractRule
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
		  * @param The alarm description of the alarm that are subject to the rule. This is only
		  *        necessary for suppression by description and set to empty string if not.
          */
        SuppressionRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
			TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key, const std::string& alarmDescription="" );

        /**
          * Constructor
          *
          * @param An Alarm Rule Corba message
          */
        SuppressionRule( const TA_Base_Core::AlarmRuleCorbaDef& rule );

        /** 
          * Destructor
          */
        virtual ~SuppressionRule();

        /** 
          * process
          *
          * No processing is required for suppression rules. This method does nothing
          *
          * @param The alarm
          * @param The trigger (was the alarm raised, closed, or acknowledged)
          */
        void process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                      TA_Base_App::AbstractRule::ETriggerType trigger);

        /** 
          * alarmShouldBeSuppressed
          *
          * This method returns true if the supplied alarm is covered by this rule
          *
          * @param The alarm
          *
          * @return TRUE if the supplied alarm should be suppressed.
          */
        bool alarmShouldBeSuppressed( const TA_Base_Core::AlarmDetailCorbaDef& message );

		// obtain copy of the alarm description for inhibition purposes
		// It will return empty string if description is not the criteria for inhibition
		std::string getAlarmDescription() const { return m_description; };
		
		virtual const std::string& getRuleType() const
		{
			if(!m_description.empty())
			{
				return SUPPRESS_BY_DESCRIPTION;
			}
			else
			{
				return SUPPRESS_BY_TYPE;
			}
		}

	private:

		// The alarm description for this rule, if applicable
		// This is only for rule that needs inhibition/suppression by description
		// It will be empty if inhibition/suppression by description is not required
		std::string m_description;

    };    
}

#endif // !defined(SUPPRESSIONRULE)
