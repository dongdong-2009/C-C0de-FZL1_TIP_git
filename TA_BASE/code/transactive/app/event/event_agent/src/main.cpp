/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/alarm/alarm_agent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "app/event/event_agent/src/EventAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

using namespace TA_Base_App;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;

#include "ace/OS.h"
int main( int argc, char* argv[] )
{
    try
    {
        EventAgent agent(argc, argv);
        agent.run();
    }
	catch( const GenericAgentException& gae )
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch,
			"GenericAgentException", gae.what());
	}
	catch( const DatabaseException& dbe )
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch,
			"DatabaseException", dbe.what());
	}
	catch( const DataException& de )
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch,
			"DataException", de.what());
	}

    return 0;
}
