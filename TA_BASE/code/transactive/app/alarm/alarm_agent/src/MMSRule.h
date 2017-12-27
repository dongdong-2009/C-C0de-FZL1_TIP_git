/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/MMSRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * MMS Rules can be configured such that specific alarms can
  * trigger a job request
  */
#if !defined(MMSALARMRULE)
#define MMSALARMRULE

#include <string>

#include "app/alarm/alarm_agent/src/AbstractRule.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsAlarmSubmitterCorbaDef.h"
#include "core/naming/src/NamedObject.h"

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef; // Forward declaration
}

namespace TA_Base_App
{

    class MMSRule : public virtual AbstractRule
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
        MMSRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
                TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key );

        /** 
          * Destructor
          */
        virtual ~MMSRule();

        /** 
          * process
          *
          * This method will send the job request if the supplied alarm is subject to the
          * rule.
          *
          * @param The alarm
          * @param The trigger (was the alarm raised, closed, or acknowledged)
          */
        void process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                      TA_Base_App::AbstractRule::ETriggerType trigger);
		
		virtual const std::string& getRuleType() const
		{
			return MMS;
		}

    private:

// 		typedef TA_Base_Core::NamedObject< TA_Base_Bus::IMmsAlarmSubmitterCorbaDef, 
// 			TA_Base_Bus::IMmsAlarmSubmitterCorbaDef_ptr, 
// 			TA_Base_Bus::IMmsAlarmSubmitterCorbaDef_var > MMSNamedObject;

       // MMSNamedObject* m_mmsAgent;
        //Mintao++ TD15936
       // std::string m_mmsAgentSubmitterType;
    };    
}

#endif // !defined(MMSALARMRULE)
