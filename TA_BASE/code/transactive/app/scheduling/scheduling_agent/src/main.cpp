/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/main.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  * This file contains the main() function for the Scheduling Agent.
  *
  */


#include <vector>
#include <string>

#include "ace/OS.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAgent.h"
#include "app/scheduling/scheduling_agent/src/SchedulingException.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"


int main( int argc, char* argv[] )
{
    try
    {
        TA_Base_App::SchedulingAgent* agent = new TA_Base_App::SchedulingAgent(argc, argv);
        TA_ASSERT(agent != NULL, "agent should never be NULL");
        agent->run();
        delete agent;
        agent = NULL;
    }
    catch( const TA_Base_Core::GenericAgentException& gae )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "GenericAgentException", gae.what() );
    }
    catch( const TA_Base_Core::DatabaseException& dbe )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", dbe.what() );
    }
    catch( const TA_Base_Core::DataException& de )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "DataException", de.what() );
    }
    catch( const TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException& bse )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "BadScheduleException", bse.what.in() );
    }
    catch( const TA_Base_App::SchedulingException& se )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "SchedulingException", se.what() );
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
    }

    return 0;
}
