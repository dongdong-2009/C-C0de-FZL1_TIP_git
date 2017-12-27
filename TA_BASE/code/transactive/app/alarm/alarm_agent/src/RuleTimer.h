/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/RuleTimer.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is a timer for rules created via the Alarm Manager. Once the rule has
  * expired it will be removed.
  */
#if !defined(RULETIMER)
#define RULETIMER

#include <map>

#include "app/alarm/alarm_agent/src/RuleRepository.h"

#include "core/threads/src/Thread.h"

namespace TA_Base_Core
{
    class ReEntrantThreadLockable; // Forward declaration
}


namespace TA_Base_App
{
    class RuleTimer : public TA_Base_Core::Thread
    {

    public:

        /**
          * Constructor
          *
          * Creates a RuleTimer object
          */
        RuleTimer(RuleRepository& ruleRepository);

        /**
          * Destructor
          */
        ~RuleTimer();

        /** 
          * addRule
          *
          * This method adds a rule that requires an expiry time
          *
          * @param The pkey of the new rule
          */
        void addRule( unsigned long ruleId );

        /** 
          * removeRule
          *
          * This method removes a rule that is monitored for expiration
          *
          * @param The pkey of the new rule
          */
        void removeRule( unsigned long ruleId );

         /**
         * run
         *
         * This method loops through the map of rules and checks whether the
         * time has expired for the rule. If so, the rule is removed.
         */
        virtual void run();

        /**
         * terminate
         *
         * Terminates the loop that checks the expiry times of the rules.
         */
        virtual void terminate();

    private:

        long m_ruleLifetime;                            // Lifetime of rules in seconds

        std::map<unsigned long, time_t> m_timedRules;   // Rule Id and expiry time

        RuleRepository& m_ruleRepository;

        static unsigned int SUPPRESSION_RULE_LIFETIME_DEFAULT;

        bool m_terminated;

        static TA_Base_Core::ReEntrantThreadLockable m_lock;
    };
}

#endif // !defined(RULETIMER)
