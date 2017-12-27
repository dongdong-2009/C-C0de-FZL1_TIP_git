/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datanode_library/src/SummaryTriggerTimer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  * The DataNodeActivationProcessor thread is responsible
  * for generating events and submitting
  * them.
  *
  */

#ifndef SUMMARYTRIGGERTIMER_H
#define SUMMARYTRIGGERTIMER_H

#include "core/timers/src/SingletonTimerUtil.h"
#include "core/timers/src/ITimeoutCallback.h"

#include <map>

namespace TA_Base_Bus
{
    class IEntity;
    class SummaryProcessor;

    class SummaryTriggerTimer : public TA_Base_Core::ITimeoutCallback
	{

	public:
        SummaryTriggerTimer(SummaryProcessor& summaryProcessor);
        ~SummaryTriggerTimer();
        
        typedef std::map<unsigned long, TA_Base_Bus::IEntity*> EntityMap;
        void setEntityMap(const EntityMap* entityMap);
        
        virtual void timerExpired(long timerId, void* userData);
        
	private:

        const EntityMap* m_entityMap;
        
        SummaryProcessor& m_summaryProcessor;
        TA_Base_Core::SingletonTimerUtil&		m_timerUtility;
        
	};
}

#endif
