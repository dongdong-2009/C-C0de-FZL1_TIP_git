/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/database_status_monitor/src/IDatabaseStatusObserver.h $
 * @author:  J.Chevalley
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * This interface is implemented by clients that want to observe the status of a
 * database and be notified when it becomes un/available.
 */

#if !defined(IDatabaseStatusObserver_C775327C_CA9B_442a_9960_32D8AB8E2838__INCLUDED_)
#define IDatabaseStatusObserver_C775327C_CA9B_442a_9960_32D8AB8E2838__INCLUDED_

#include <string>
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"

namespace TA_Base_Bus
{
    class IDatabaseStatusObserver
    {
    public:
	    /**
	     * @param DbConnectionInfo
	     * @param status
	     *
	     */
        virtual void notifyStatusChange(const std::string& DbConnectionInfo, DatabaseStatusCorbaDef::EState status) =0;
    };
}//namesapce
#endif // !defined(IDatabaseStatusObserver_C775327C_CA9B_442a_9960_32D8AB8E2838__INCLUDED_)
