/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/datalog/datalog_agent/src/main.cpp $
  * @author:  luohuirong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/12/13 14:17:48 $
  * Last modified by:  $Author: luohuirong $
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <vector>
#include <string>

//#include "Test.h"  //TODO: just for test
#include "app/datalog/datalog_agent/src/DataLogAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

using TA_Base_App::DataLogAgent;
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
        DataLogAgent agent(argc, argv);
        agent.run();

		// todo: to be deleted:
// 		DataLogAgent *  agent= new DataLogAgent(argc, argv);
// 		TA_Base_App::Test test;

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
