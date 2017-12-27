/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/WXL1_TIP/3002/transactive/app/scada/SNMPStatusAgent/src/SNMPStatusAgentMain.cpp $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2014/08/12 17:53:47 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	Main program for SNMPStatusAgent process
  *
  */

#include "SNMPStatusAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"

int main( int argc, char * argv[] )
{
	try
	{
		TA_IRS_App::SNMPStatusAgent nmsAgent( argc, argv );
		nmsAgent.startSNMPStatusAgent();
	}
    catch( const TA_Base_Core::GenericAgentException& gae )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "GenericAgentException", gae.what());
    }
	catch( const TA_Base_Core::TransactiveException & te)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
	}
	catch( std::exception & e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
	}
		
	return 1;
}
