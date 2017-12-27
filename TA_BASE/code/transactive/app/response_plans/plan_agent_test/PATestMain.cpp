/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent_test/PATestMain.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#include "PlanAgentTest.h"

int main( int argc, char** argv )
{
	if( argc < 6 )
	{
		std::cerr << "usage: PlanAgentTest --node-id=<nodeId>" << std::endl 
                  << "                     --session-id=<session>" << std::endl
                  << "                     --location-key=<location>" << std::endl
                  << "                     --db-connection=<dbase>" << std::endl
                  << "                     --notify-hosts=<hosts>" << std::endl
                  << "                     [--debug-level=<level>]" << std::endl
                  << "                     [--debug-file=<file>]" << std::endl
                  << std::endl;

		return( -1 );
	}

    TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv);

	const PaData::NodeId nodeId = ::atol(TA_Base_Core::RunParams::getInstance().get("NodeId").c_str());
	
	try
    {
		PlanAgentTest pat;

        pat.getActivePlanDetails(nodeId);
        pat.getAllActivePlanDetails();
    }
	catch( PaIf::PlanAgentError &e )
	{
		std::cerr << "Plan agent error: " << e.what.in() << "." << std::endl;
	}
	/*
	*   The following exceptypes are not defined. Hence it has been commented.
	*
	*

	catch( PaIf::PathNotFound &e )
	{
		std::cerr << "Path not found: " << e.what.in() << "." << std::endl;
	}
	catch( PaIf::InstanceNotFound &e )
	{
		std::cerr << "Instance not found: " << e.what.in() << "." << std::endl;
	}
	*/
	catch( CORBA::Exception &e )
	{
		std::cerr << "Caught a CORBA exception, type: " << typeid(e).name() 
			      << ", message: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) << std::endl;
	}

    catch (...)
	{
		std::cerr << "Unhandled exception, bugger!" << std::endl;
	}

	return( 0 );
}
