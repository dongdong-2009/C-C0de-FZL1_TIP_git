/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AbstractRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This interface should be implemented by all configured rule types.
  */
#if !defined(ABSTRACTRULE)
#define ABSTRACTRULE

#include <string>

namespace TA_Base_Core
{
    struct AlarmDetailCorbaDef; // Forward declaration
}

namespace TA_Base_App
{
	const std::string SUPPRESS_BY_TYPE = "SuppressionByType";
	const std::string SUPPRESS_BY_DESCRIPTION = "SuppressionByDescription";
	const std::string EXECUTE_PLAN = "PlanExecution";
	const std::string AUTO_ACK = "AutomaticAcknowledgement";
	const std::string AVALANCHE = "AvalancheSuppression";
	const std::string MMS = "MMS";
	const std::string CCTV = "CCTV";

    class AbstractRule
    {

    public:

        enum ETriggerType
        {
            SUBMIT  = 0,
            ACK     = 1,
            CLOSE   = 2
        };

		enum ERuleState
		{
			RULE_CREATED = 0,
			RULE_DELETED = 1,
			RULE_UPDATED = 2
		};

        /**
          * Constructor
          *
          * @param The entity key associated with alarms that are subject to the rule
          * @param The entity type of associated with alarms that are subject to the rule
          * @param The alarm type of alarms that are subject to the rule
          * @param The trigger (was the alarm raised, closed, or acknowledged)
          * @param The rule key (as stored in the database) - used for online updates.
          */ 
        AbstractRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                     ETriggerType trigger, unsigned long key );

        /** 
          * Destructor
          */
        virtual ~AbstractRule();

        /** 
          * process
          *
          * The implementation of this method should check that the rule should
          * be applied to the supplied alarm. If so, the appropriate action should
          * be taken.
          *
          * @param The alarm
          * @param The trigger (was the alarm raised, closed, or acknowledged)
          */
        virtual void process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                              ETriggerType trigger) = 0;

		virtual const std::string& getRuleType() const =0;
        /** 
          * alarmShouldBeSuppressed
          *
          * This should be overidden by suppression rules only.
          *
          * @param The alarm
          *
          * @return TRUE if the supplied alarm should be suppressed.
          */
        virtual bool alarmShouldBeSuppressed( const TA_Base_Core::AlarmDetailCorbaDef& message );

        /** 
          * alarmIsAnAvalancheHead
          *
          * This should be overidden by avalanche suppression rules only.
          *
          * @param The alarm
          *
          * @return TRUE if the supplied alarm is the head of an avalanche.
          */
        virtual bool alarmIsAnAvalancheHead( const TA_Base_Core::AlarmDetailCorbaDef& message );

        /** 
          * Comparison operator
          *
          * @return Return TRUE if m_entityKey, m_entityType, and m_alarmType are all equal 
          *
          * @param The rule to compare
          */
        virtual bool operator==( const AbstractRule& rule );

        /** 
          * getEntityKey
          *
          * @return Returns the entity key 
          */
        const unsigned long getEntityKey();

        /** 
          * getEntityType
          *
          * @return Returns the entity type
          */
        const unsigned long getEntityType();

        /** 
          * getAlarmType
          *
          * @return Returns the alarm type 
          */
        const unsigned long getAlarmTypeKey();

        /** 
          * getTrigger
          *
          * @return Returns the trigger 
          */
        const ETriggerType getTrigger();

        /**
          * getKey
          *
          * @return Returns the alarm rule key
          */
        const unsigned long getKey();

    protected:

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
    };    
}

#endif // !defined(ABSTRACTRULE)
