/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "ace/Object_Manager.h"

#include "core/corba/src/CorbaUtil.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/PlanAgent.h"

int main( int argc, char* argv[] )
{
	int returnVal = 0;

	ACE_Object_Manager* aceObjectManager = NULL;
	TA_Base_App::PlanAgentUser* pa = NULL;

	// All the various known (and the unknown) exception types
	// are caught here. This allows us to log the fact that the
	// app has not handled the exception properly rather than
	// just dissapearing. These try/catch pairs are placed around
	// the other two thread entry points: PlanExecutionEngine and PlanAgentCorbaDefImpl.
	try
	{
		aceObjectManager = new ACE_Object_Manager;
		aceObjectManager->init();

		pa = TA_Base_App::PlanAgentUser::getInstance();
		pa->initAgentUser(argc, argv);

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan Agent starting..." );
		pa->run();
	}
	catch( const std::exception& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
		returnVal = 128;
	}
	catch( const CORBA::Exception& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() );
		returnVal = 128;
	}
    catch( ... )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled." );
		returnVal = 128;
	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Shutting down..." );
	
	delete pa;
	if( aceObjectManager != NULL )
	{
		aceObjectManager->fini();
	}
	delete aceObjectManager;

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan Agent done." );

	return returnVal;
}

