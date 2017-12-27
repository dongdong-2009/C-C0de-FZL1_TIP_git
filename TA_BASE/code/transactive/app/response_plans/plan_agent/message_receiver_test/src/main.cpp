/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/message_receiver_test/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "PlanAgentCommsSubscriber.h"
#include "PlanAgentConfigSubscriber.h"
#include "PlanAgentCustomSubscriber.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/utilities/src/RunParams.h"
#include "core/naming/src/Naming.h"
#include "windows.h"

namespace Msg = TA_Base_Core;

bool completed = false;

BOOL WINAPI messageReceiverTestHandlerRoutine( DWORD dwCtrlType  );

int main( int argc, char** argv )
{
	std::cout << "MessageReceivedTest started." << std::endl;

	SetConsoleCtrlHandler( messageReceiverTestHandlerRoutine, TRUE );

	if( argc < 3 )
	{
		TA_Base_Core::RunParams::getInstance().set(RPARAM_DBCONNECTION, "transact::its:its" );
		TA_Base_Core::RunParams::getInstance().set(RPARAM_NOTIFYHOSTS, "jupiter:2112" );
	}
	else
    {
		if( false == TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv) )
		{
			std::cerr << "usage: MessageReceiverTest" << std::endl;
			std::cerr << "       OR                 " << std::endl;
			std::cerr << "       MessageReceiverTest  --db-connection=schema:username:password --notify-hosts=host1[:port],hostn[:port]" << std::endl;
			std::cerr << "exiting due to command line errors..." << std::endl;
			return( 128 );
		}
		
    }

	TA_Base_Core::CorbaUtil::getInstance().initialise();
	TA_Base_Core::CorbaUtil::getInstance().activate();

	std::cout << "subscribing..." << std::endl;

	PlanAgentCommsSubscriber* activeUpdatesSubscriber = new PlanAgentCommsSubscriber;
	PlanAgentConfigSubscriber* configUpdatesSubscriber = new PlanAgentConfigSubscriber;
	PlanAgentCustomSubscriber* customUpdatesSubscriber = new PlanAgentCustomSubscriber;
					 
	Msg::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( Msg::PlanAgentBroadcastComms::ActivePlanUpdate, 
		activeUpdatesSubscriber, 0, 0, 0, NULL );

	Msg::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( Msg::PlanAgentBroadcastComms::PlanConfigUpdate, 
		configUpdatesSubscriber, 0, 0, 0, NULL );

	Msg::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( Msg::PlanAgentBroadcastComms::PlanCustomisationUpdate, 
		customUpdatesSubscriber, 0, 0, 0, NULL );

	// loop until we are told to exit
	while( !completed )
	{
		Sleep(1000);
	}

	std::cout << "unsubscribing..." << std::endl;
	Msg::MessageSubscriptionManager::getInstance().unsubscribeToMessages( activeUpdatesSubscriber );
	delete activeUpdatesSubscriber;

	Msg::MessageSubscriptionManager::getInstance().unsubscribeToMessages( configUpdatesSubscriber );
	delete configUpdatesSubscriber;

    TA_Base_Core::Naming::cleanUp();
    TA_Base_Core::MessageSubscriptionManager::getInstance().cleanUp();
    TA_Base_Core::MessagePublicationManager::getInstance().cleanUpSuppliers();
    TA_Base_Core::CorbaUtil::getInstance().shutdown();
    TA_Base_Core::CorbaUtil::cleanup();

	SetConsoleCtrlHandler( messageReceiverTestHandlerRoutine, FALSE );

	std::cout << "MessageReceivedTest done." << std::endl;

	return( 0 );
}

BOOL WINAPI messageReceiverTestHandlerRoutine( DWORD dwCtrlType  )
{
	BOOL handled = FALSE;
	switch( dwCtrlType )
	{
		case CTRL_C_EVENT:
		{
			completed = true;
			std::cout << "received ctrl+c, exiting." << std::endl;
			handled = TRUE;
		}
		break;
	}

	return handled;
}
