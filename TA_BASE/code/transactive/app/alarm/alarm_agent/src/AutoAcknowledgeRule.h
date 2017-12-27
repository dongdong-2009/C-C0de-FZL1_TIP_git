/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AutoAcknowledgeRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Automatic Acknowledge rules can be configured for alarms that should be
  * automatically acknowledged when a new alarm of the same type is raised.
  * This prevents large numbers of these alarms flooding the Alarm Manager
  */
#if !defined(ALARMACKNOWLEDGERULE)
#define ALARMACKNOWLEDGERULE

#include <string>
#include "app/alarm/alarm_agent/src/AbstractRule.h"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef; // Forward declaration
}

namespace TA_Base_App
{

    class AutoAcknowledgeRule : public virtual AbstractRule
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
          */
        AutoAcknowledgeRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmType,
                            TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key );

        /** 
          * Destructor
          */
        virtual ~AutoAcknowledgeRule();

        /** 
          * process
          *
          * This method checks to see if a closed alarm of this type exists in the cache.
          * If so, it acknowledges the alarm
          *
          * @param The alarm
          * @param The trigger (was the alarm raised, closed, or acknowledged)
          */
        void process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                      TA_Base_App::AbstractRule::ETriggerType trigger);
		
		virtual const std::string& getRuleType() const
		{
			return AUTO_ACK;
		}

	protected:
		//TD17851 marc_bugfix begin
        /**
          * doesRuleApply
          *
          * This method returns true if the supplied alarm is subject to the rule
          *
          * @param The alarm
          * @param The trigger
          */
        virtual bool doesRuleApply( const TA_Base_Core::AlarmDetailCorbaDef& message,
                                    const ETriggerType& trigger );
    private:

        // The rule key (as stored in the database)
        unsigned long m_key;

        // Alarms associated with this entity are subject to this rule
        unsigned long m_entityKey;

        // Alarms associated with entities of this type are subject to this rule
        unsigned long m_entityType;

        // Alarms of this type are subject to this rule
        unsigned long m_alarmTypeKey;

        // The trigger for the rule (alarm raised, closed, acknowledged)
        ETriggerType m_trigger;
		//TD17851 marc_bugfix end 

    };    
}

#endif // !defined(ALARMACKNOWLEDGERULE)
