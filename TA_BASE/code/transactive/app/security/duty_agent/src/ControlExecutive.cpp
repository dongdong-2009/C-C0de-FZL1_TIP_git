/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/ControlExecutive.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the TA_Base_App::ControlExecutive interface.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/ControlExecutive.h"
#include "app/security/duty_agent/src/IDutyAgentCommand.h"

namespace TA_Base_App
{
	void ControlExecutive::execute( IDutyAgentCommand& command, TA_Base_Bus::RightsLibrary& rightsLib,
		ta_uint32 resourceKey, bool checkRights/*=true*/ ) const
	{
		try
		{
			std::string reason;

			bool bOk = false;

			if ( !checkRights )
			{
				bOk = true;
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "ControlExecutive::execute peer no need to check rights ");
			} 
			else
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "ControlExecutive::execute before check rights ");
				bOk = command.rightsOk( rightsLib, resourceKey , reason);
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "ControlExecutive::execute after check rights ");
			}

			if( bOk )
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "ControlExecutive::execute before execute %s", typeid(command).name());
				command.execute();
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "ControlExecutive::execute after execute %s",typeid(command).name());
			}
			else
			{
				throw( TA_Base_Core::AccessDeniedException(reason.c_str()) );
			}
		}
		catch( const TA_Base_Core::RightsException &e )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::RightsException", e.what() );
			
			throw( TA_Base_Core::AccessDeniedException(e.what()) );
		}
        catch( const TA_Base_Core::AccessDeniedException& )
        {
            // This needs to be caught here and rethrown so that it is not caught as a CORBA::Exception
            // below and rethrown as an unhandled exception.
            throw;
        }
		catch( const TA_Base_Core::DatabaseException &e )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DatabaseException", e.what() );
			
			throw( TA_Base_Core::DatabaseErrorException(e.what()) );
		}
		catch( const TA_Base_Core::DataException &e )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DataException", e.what() );
			
			throw( TA_Base_Core::DatabaseErrorException(e.what()) );
		}
		catch( const TA_Base_Core::AuthenticationAgentException &e )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::AuthenticationAgentException", e.what() );
			
			throw( TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException(e.what()) );
		}
		catch( const TA_Base_Core::ObjectResolutionException &e )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", e.what() );
			
			throw( TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException(e.what()) );
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "UnhandledException." );
			throw( TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( e.what() ) );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "UnhandledException." );
			throw( TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() ) );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "UnhandledException." );
			throw( TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( "Unknown exception" ) );
		}
	}
}
