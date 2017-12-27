#ifndef MONITOREXECUTIVE_H_INCLUDED
#define MONITOREXECUTIVE_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/MonitorExecutive.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Used to allow different behaviour for the execution of Agent methods between
 * Control and Monitor modes.
 */

#include "app/security/duty_agent/src/IDutyAgentExecutive.h"

namespace TA_Base_App
{
    class MonitorExecutive : public IDutyAgentExecutive
    {
    public:
		
		virtual void execute(IDutyAgentCommand& command, TA_Base_Bus::RightsLibrary& rightsLib, ta_uint32 resourceKey, bool checkRights = true ) const;
    };
}

#endif // MONITOREXECUTIVE_H_INCLUDED
