/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/command_line_interface/src/main.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This application provides a command line interface to the Scheduling Agent
  *
  */

#include <iostream>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "app/scheduling/scheduling_agent/command_line_interface/src/SchedulingAgentCLI.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::CorbaUtil;


const char* RPARAM_OPERATORNAME          = "OperatorName";
const char* RPARAM_PROFILEID             = "ProfileId";
const char* RPARAM_WORKSTATIONID         = "WorkstationId";
const char* RPARAM_SCHEDULINGAGENTENTITY = "SchedulingAgentEntity";

void showUsage(const char *execName)
{
    std::cout << std::endl
		          << "Usage: " << execName << " with the following parameters: "
				  << std::endl
				  << "\t--notify-hosts=<hostname(s)>" << std::endl
				  << "where <hostname(s)> is a list of notification "
				  << "service hosts (optionally with ports)." << std::endl
				  << "\t--db-connection=<connection>" << std::endl
                  << "\t--operator-name=<operator name>" << std::endl
                  << "\t--profile-id=<profile ID>" << std::endl
                  << "where the profile can run plans" << std::endl
                  << "\t--location-key=<location ID>" << std::endl
                  << "the location is where the plan is run" << std::endl
                  << "\t--workstation-id=<workstation ID>" << std::endl
                  << "the workstation of the operator" << std::endl
                  << "\t--scheduling-agent-entity=<name of scheduling agent>"
                  << std::endl
                  << "the scheduling agent entity name" << std::endl
                  << std::endl
                  << "\t(optional) --session-id=<existing session ID>" << std::endl
                  << "if specified, does not authenticate (--operator-name and --workstation-id are not required)"
                  << std::endl
				  << "eg:" << std::endl
				  << execName
                  << " --notify-hosts=jupiter:1234,jupiter:4321"
                  << " --db-connection=OCCB:ITS:ITS"
                  << " --operator-name=paulk"
                  << " --profile-id=4"
                  << " --location-key=1"
                  << " --workstation-id=42"
                  << " --scheduling-agent-entity=OccSchedulingAgent"
                  << std::endl << std::endl
                  << "Note that the scheduling agent must have access to the "
                  << "profile."
				  << std::endl;
}

int main( int argc, char **argv)
{
    TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugFatal);
	// Parse the command line
	RunParams::getInstance().parseCmdLine( argc, argv );


	//
	// Parse args for correctness
	//
	if ( ! RunParams::getInstance().isSet(RPARAM_NOTIFYHOSTS)             ||
         ! RunParams::getInstance().isSet(RPARAM_DBCONNECTION)            ||
         ! RunParams::getInstance().isSet(RPARAM_PROFILEID)               ||
         ! RunParams::getInstance().isSet(RPARAM_LOCATIONKEY)             ||
         ! RunParams::getInstance().isSet(RPARAM_SCHEDULINGAGENTENTITY) )
    {
        showUsage(argv[0]);
        return 0;
    }


    unsigned long profileId =
        atol(RunParams::getInstance().get(RPARAM_PROFILEID).c_str());
    unsigned long locationId =
        atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
    
    std::string schedulingAgentEntity =
        RunParams::getInstance().get(RPARAM_SCHEDULINGAGENTENTITY).c_str();


    // if a sessionID is specified
    if ( RunParams::getInstance().isSet(RPARAM_SESSIONID) )
    {
        // do ctor v2
        std::string sess = RunParams::getInstance().get(RPARAM_SESSIONID).c_str();

        try
        {
            TA_Base_App::SchedulingAgentCLI cli(
                sess, profileId, locationId, schedulingAgentEntity);

            cli.execute();
        }
        catch(const CORBA::Exception& ex)
        {
            std::cout << "Caught a corba exception: "
                      << CorbaUtil::getInstance().exceptionToString(ex)
                      << std::endl;
        }
        catch(const std::exception& ex)
        {
            std::cout << "Caught an exception: " << ex.what() << std::endl;
        }
        catch (...)
        {
		    std::cout << "Caught an unknown exception" << std::endl;
	    }
    }
    else
    {
        // ensure operator name and wstation id were specified
        if ( ! RunParams::getInstance().isSet(RPARAM_WORKSTATIONID) ||
         ! RunParams::getInstance().isSet(RPARAM_OPERATORNAME) )
        {
            showUsage(argv[0]);
            return 0;
        }

        std::string user =
        RunParams::getInstance().get(RPARAM_OPERATORNAME).c_str();
        unsigned long workstationId =
        atol(RunParams::getInstance().get(RPARAM_WORKSTATIONID).c_str());

        try
        {
            TA_Base_App::SchedulingAgentCLI cli(
                user, profileId, locationId, workstationId, schedulingAgentEntity);

            cli.execute();
        }
        catch(const CORBA::Exception& ex)
        {
            std::cout << "Caught a corba exception: "
                      << CorbaUtil::getInstance().exceptionToString(ex)
                      << std::endl;
        }
        catch(const std::exception& ex)
        {
            std::cout << "Caught an exception: " << ex.what() << std::endl;
        }
        catch (...)
        {
		    std::cout << "Caught an unknown exception" << std::endl;
	    }
	}


    
    
    

    

    return 0;
}

