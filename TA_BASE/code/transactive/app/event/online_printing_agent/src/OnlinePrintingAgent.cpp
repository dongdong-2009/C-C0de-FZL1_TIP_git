/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/OnlinePrintingAgent.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2012/06/12 13:35:44 $
  *  Last modified by:  $Author: builder $
  *
  *  Mainline for the Online Printing Agent.
  *
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

namespace
{
    const int ABNORMAL_TERMINATION(128);
}

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;


int main (int argCount, char* args[])
{
    int exitCode(0);

    try
    {
        OnlinePrintingAgentUser onlinePrintingAgentUser(argCount,args);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Online Printing Agent starting up...");
        onlinePrintingAgentUser.run();
    }
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, std::string("Unhandled ") + typeid(ex).name(), ex.what());
        exitCode = ABNORMAL_TERMINATION;
    }
    catch (const CORBA::Exception& ex)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unhandled CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
        exitCode = ABNORMAL_TERMINATION;
    }
    catch (...)
    {
        LOG_EXCEPTION_DETAILS(SourceInfo, "Unhandled exception.", "");
        exitCode = ABNORMAL_TERMINATION;
    }

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Online Printing Agent shutting down...");
    return exitCode;
}
