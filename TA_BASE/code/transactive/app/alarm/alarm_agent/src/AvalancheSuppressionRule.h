/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AvalancheSuppressionRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AVALANCHESUPPRESSIONRULE)
#define AVALANCHESUPPRESSIONRULE

#include <string>
#include <vector>
#include <map>

#include "app/alarm/alarm_agent/src/AbstractRule.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/IAlarmClockHelper.h"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef; // Forward declaration
    class AuditMessageSender;   // Forward declaration
    class AlarmClock;           // Forward declaration
}

namespace TA_Base_App
{
    class AvalancheSuppressionChild;  // Forward declaration
    class AvalancheSuppressionRule : public virtual AbstractRule, public virtual TA_Base_Core::IAlarmClockHelper
    {

    public:

        /**
          * Constructor
          *
          * @param The entity key associated with alarms that are subject to the rule
          * @param The entity type of associated with alarms that are subject to the rule
          * @param The alarm type of alarms that are subject to the rule
          * @param The timeout value for the rule
          * @param The rule key (as stored in the database) - used for online updates.
          * @param A vector of children to be suppressed
          */
        AvalancheSuppressionRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                                 TA_Base_App::AbstractRule::ETriggerType trigger,
                                 unsigned long key, unsigned long timeout, unsigned long timein, unsigned int minAlarms,
                                 const std::vector<AvalancheSuppressionChild*>& children );

        /** 
          * Destructor
          */
        virtual ~AvalancheSuppressionRule();

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
		
		bool SuppressArithmetic( AvalancheSuppressionRule * RuleForGoThrough, AvalancheSuppressionRule * RuleForStoreChild, const TA_Base_Core::AlarmDetailCorbaDef& message  );

        /** 
          * alarmIsAnAvalancheHead
          *
          * This method returns true if the supplied alarm is the head alarm for this rule
          *
          * @param The alarm
          *
          * @return TRUE if the supplied alarm is the head alarm for this rule.
          */
        bool alarmIsAnAvalancheHead( const TA_Base_Core::AlarmDetailCorbaDef& message );

		/**
		 * executeAlarm
		 *
		 * This function is called when the alarm clock goes off
		 */
        void executeAlarm( TA_Base_Core::AlarmClock* clock );

		/**
		 * getHeadAlarmId
		 *
		 * This method returns the Alarm ID of the head alarm for this rule
		 */
		std::string getHeadAlarmId();

		std::vector<AvalancheSuppressionChild*> getChildAvalancheRule()
		{
			return m_children;
		}
		void storeChildAlarmId(std::string ChildAlarmId)
		{
			m_suppressedAlarmIds.push_back( ChildAlarmId );
		}
		unsigned long getActivateTime()
	 	{
		 	return m_activateTime; 
	 	}
		void addAlarmsSuppressed()
	 	{
			 m_alarmsSuppressed++;
	 	}
		bool getActiveState()
		{
			return m_activeState;
		}
        
		void initChild(AvalancheSuppressionRule * SuppressRule);
		void removeChild(AvalancheSuppressionRule * SuppressRule);
		bool existCircle();
		bool isExistCircle(AvalancheSuppressionRule * SuppressRule);
		virtual const std::string& getRuleType() const
		{
			return AVALANCHE;
		}


		TA_Base_Core::ReEntrantThreadLockable m_listLock;


    private:

        /**
          * timeoutExpired
          *
          * Called by the alarmclock when the time runs out.
          * This will raise any child alarms that have not yet been closed.
          */
        void timeoutExpired();

        /**
          * timeinExpired
          *
          * Called by the alarmclock when the time runs out.
          * This will ensure that the minimum number of child alarms have been
          * received for this rule to be 'active'.
          */
        void timeinExpired();

        /** 
          * openAlarms
          *
          * tries to open all alarms suppressed by this rule
          */
        void AvalancheSuppressionRule::openAlarms();


        std::vector<AvalancheSuppressionChild*> m_children;
        unsigned long m_timeout;        // An avalanche lasts this long         
        unsigned long m_expiryTime;     // This avalanche expires at this time
        unsigned long m_timein;         // Requires m_minAlarmsToValidate in this time to make an avalanche
        unsigned int  m_minAlarmsToValidate;
        unsigned long m_activateTime;   // This avalanche becomes real at this time if m_alarmsSuppresssed > m_minAlarmsToActive 
		std::string   m_headAlarmId;	// The alarmID of the head alarm
		bool m_activeState;  //whether avalanche is active or not, true=active

        unsigned int m_alarmsSuppressed; // Number of alarms suppressed so far

		// TD 13530 Audit messages to be sent by AlarmQueue after updating alarm table
        // Audit message sender
        //TA_Base_Core::AuditMessageSender* m_auditSender;

        // list of currently suppened alarms
        std::vector<std::string> m_suppressedAlarmIds;

        // alarm clocks
        TA_Base_Core::AlarmClock* m_timeoutTimer;
        TA_Base_Core::AlarmClock* m_timeinTimer;
    };    
}

#endif // !defined(AVALANCHESUPPRESSIONRULE)
