/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/rights_agent/TestConsole/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"
#include "core/naming/src/NamedObject.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::CorbaUtil;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::NamedObject;

int main( int argc, char* argv[] )
{
	TA_Base_Bus::RightsLibrary* rl = NULL;

    try
    {
        RunParams::getInstance().parseCmdLine(argc,argv);

		// Added Bu Nous
		TA_Base_Core::RunParams::getInstance().set( RPARAM_LOCATIONKEY ,"0");
		TA_Base_Core::RunParams::getInstance().set(RPARAM_DBCONNECTION , "tra_dev2:unittest:unittest");
		TA_Base_Core::RunParams::getInstance().set( RPARAM_NOTIFYHOSTS,"1");
		// Added by Nous

        DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);
        DebugUtil::getInstance().setFile("TestConsole.log");

        TA_Base_Core::CorbaUtil::getInstance().initialise();
	    TA_Base_Core::CorbaUtil::getInstance().activate();

		// build the Auth library
		std::cout << "Building the auth library.\n";

		
			TA_Base_Bus::AuthenticationLibrary authLib; 
		
		
		const unsigned long USERID        = 3; 
		const unsigned long PROFILEID     = 0; 
		const unsigned long LOCATIONID    = 0;
		const unsigned long WORKSTATIONID = 31; 
		const std::string PASSWORD        = "<empty>";

		std::string sessionId =  authLib.requestSession(USERID, PROFILEID, LOCATIONID,
														WORKSTATIONID, PASSWORD);

		std::cout << "Using session " << sessionId << std::endl;

        std::cout << "About to request permission for an action: ";
        std::cout << "isActionPermittedOnResource(0,0,0,emptyString,decisionModule)" << std::endl;

		
		// build the Rights Library
		TA_Base_Bus::RightsLibraryFactory rlFactory;

        std::cout << "Building rights library" << std::endl;
		try
		{
			rl = rlFactory.buildRightsLibrary();
		}
		catch(TransactiveException& ex)
		{
			std::cerr << "Transactive Exception: " << ex.what() << std::endl;
			LOG (SourceInfo, DebugUtil::ExceptionCatch, ex.what());

			return 128;
		}
		catch(CORBA::Exception& ex)
		{
			std::cerr << "Corba Exception: ";
			std::string reason = CorbaUtil::getInstance().exceptionToString(ex);
			std::cout << reason << std::endl;
			return 128;
		}
		catch( ... )
		{
			std::cerr << "Unknown Exception" << std::endl;
			return 128;
		}

		TA_Base_Bus::EDecisionModule decisionModule;
		bool response = false;
		
		std::string reason;

		decisionModule = TA_Base_Bus::UNDEFINED_DM;
		reason = "";

		std::cout << "Calling library" << std::endl;

		try
		{
			const unsigned long RESOURCEKEY = 0;
			const unsigned long ACTIONKEY   = 1;
			response = rl->isActionPermittedOnResource( sessionId, RESOURCEKEY,
														ACTIONKEY, reason, decisionModule );
			authLib.endSession(sessionId);

		}
		catch(TA_Base_Core::RightsException &ex )
		{
			std::cerr << "Rights Exception: " << ex.what() << std::endl;
			return 128;
		}
		catch(TransactiveException& ex)
		{
			std::cerr << "Transactive Exception: " << ex.what() << std::endl;
			LOG (SourceInfo, DebugUtil::ExceptionCatch, ex.what());
			return 128;
		}
		catch(CORBA::Exception& ex)
		{
			std::cerr << "Corba Exception: ";
			std::string reason = CorbaUtil::getInstance().exceptionToString(ex);
			std::cerr << reason << std::endl;
			return 128;
		}
		catch( ... )
		{
			std::cerr << "Unknown Exception" << std::endl;
			return 128;
		}
	
		std::cout << "Library returned:" << std::endl;

		std::cout << "Reason: " << reason << std::endl;

		std::cout << "Response: ";
		switch (response)
		{
		case true:
			std::cout << "Permit" << std::endl;
			break;

		case false:
			std::cout << "Deny" << std::endl;
			break;
		}

		std::cout << "Decision module: ";
		switch (decisionModule)
		{
		case 0:
			std::cout << "Rights" << std::endl;
			break;
		case 1:
			std::cout << "Mutex" << std::endl;
			break;
		default:
			std::cout << "No value" << std::endl;
			break;
		}
	}
	catch(TA_Base_Core::RightsException &ex )
	{
		std::cerr << "Rights Exception: " << ex.what() << std::endl;
	}
    catch(TransactiveException& ex)
    {
        LOG (SourceInfo, DebugUtil::ExceptionCatch, ex.what());
    }
    catch(CORBA::Exception& ex)
    {
        std::cerr << "Corba Exception: ";
        std::string reason = CorbaUtil::getInstance().exceptionToString(ex);
        std::cerr << reason << std::endl;
    }
    catch( ... )
    {
          std::cerr << "Unknown Exception" << std::endl;
    }

	delete rl;
	
    return 0;
}
