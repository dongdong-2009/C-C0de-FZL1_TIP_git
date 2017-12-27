/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/main.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2012/06/12 13:35:44 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */
#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyAgentUser.h"

#include "ace/OS.h"

int main( int argc, char* argv[] )
{
    ACE::init();

	int returnVal = 0;

	TA_Base_App::DutyAgentUser* daUser = NULL;

	// All the various known (and the unknown) exception types
	// are caught here. This allows us to log the fact that the
	// app has not handled the exception properly rather than
	// just dissapearing. These try/catch pairs are placed around
	// the other thread entry point: DutyAgentImpl.
	try
	{
		int initCode = ACE_Object_Manager::instance()->init();
		daUser = new TA_Base_App::DutyAgentUser( argc, argv );

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Duty Agent starting, ace init code:%d", initCode );

		daUser->run();
	}
	catch( const std::exception& e )
	{
		LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );
		returnVal = 128;
	}
	catch( const CORBA::Exception& e )
	{
		LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
		returnVal = 128;
	}
    catch( ... )
	{
		LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
		returnVal = 128;
	}

	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Duty Agent shutting down..." );
	
	delete daUser;
	daUser = NULL;
	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "finish delete agentuser" );

	ACE_Message_Block* msg = new ACE_Message_Block;
	msg->msg_type( ACE_Message_Block::MB_HANGUP );
	TA_Base_Bus::PeerStateChangeTask::instance()->putq(msg);

	//int finiCode = (ACE_Object_Manager::instance())->fini();
	//LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "After Duty Agent done, ace finicode:%d", finiCode );

	return( returnVal );
}
