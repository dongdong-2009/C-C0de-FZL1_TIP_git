#if !defined(NO_ALARM_AGENT_EXCEPTION)
#define NO_ALARM_AGENT_EXCEPTION
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/NoAlarmAgentException.h $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Used by TA_Alarm, thrown when the Alarm lib can't find the Alarm agent
  * at a specific location
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{

    class NoAlarmAgentException : public TransactiveException  
    {
    public:
        NoAlarmAgentException() throw();
        NoAlarmAgentException(const std::string& msg) throw();
        virtual ~NoAlarmAgentException() throw();

    protected:

        // This is where any exception messages are stored
        std::string m_message;
    };
};

#endif // !defined(NO_ALARM_AGENT_EXCEPTION)
