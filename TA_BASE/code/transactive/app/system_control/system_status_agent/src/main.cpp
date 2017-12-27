/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/main.cpp $
  * @author:  christiandw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This file contains the main that start the System Status Agent application
  *
  */

//we need to pick up the sockets stuff


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "app/system_control/system_status_agent/src/SystemStatusAgent.h"
#include "app/system_control/system_status_agent/src/pingDriver/SocketFailedException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;

//--entity-name=OccSystemStatusAgent --operation-mode=Control --mgr-port=9000 --debug-level=INFO --debug-file=c:\transactive\logs\ssa.log

int main( int argc, char* argv[] )
{
	//Note that to get windows to use the correct socket libraries
	//you must add the following to your project preprocesor directives:
	//,_WIN32_WINNT=0x0400,WINVER=0x0400

	FUNCTION_ENTRY("main");

	//Init for socket (pinging) stuff:
	#ifdef WIN32
		try
		{
			WSAData winsockData;
			WSAStartup( WINSOCK_VERSION, &winsockData);
		}
		catch(...)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "WSAStartup threw");
		}

	#endif	

	TA_Base_App::SystemStatusAgent* agent = 0;

    try
    {		
		agent = new TA_Base_App::SystemStatusAgent(argc, argv);
		agent->run();
		delete agent;
    }
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "GenericAgentException", gae.what());
		if(0 != agent) delete agent;
    }
    catch( const TA_Base_Core::DatabaseException& dbe )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "DatabaseException", dbe.what());
		if(0 != agent) delete agent;
    }
    catch( const TA_Base_Core::DataException& de )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "DataException", de.what());
		if(0 != agent) delete agent;
    }
	catch (TA_Base_App::SocketFailedException& sfe)
	{
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "SocketFailedException", sfe.what());
		if(0 != agent) delete agent;
	}
	catch (std::exception e)
	{
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "generic exception class", e.what());
		if(0 != agent) delete agent;
	}
	catch (...)
	{
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "exception of unknown type caught");
		if(0 != agent) delete agent;
	}

    FUNCTION_EXIT;

	#ifdef WIN32
	WSACleanup();
	#endif

	return 0;
}
