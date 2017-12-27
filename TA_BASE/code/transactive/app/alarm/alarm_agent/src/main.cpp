/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <vector>
#include <string>

#include "app/alarm/alarm_agent/src/AlarmAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

using TA_Base_App::AlarmAgent;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;

#include "ace/OS.h"

int main( int argc, char* argv[] )
{
    try
    {
        AlarmAgent agent(argc, argv);
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
