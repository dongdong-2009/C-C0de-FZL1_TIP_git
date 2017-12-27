// AuthenPeerTaskMO.cpp: implementation of the AuthenPeerTaskMO class.
//
//////////////////////////////////////////////////////////////////////


#include "bus/security/authentication_library/src/AuthenPeerTaskMO.h"
#include "bus/security/authentication_library/src/AuthenPeerTask.h"

#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	AuthenPeerTaskMO::AuthenPeerTaskMO(AuthenPeerTask * pAuthenPeerTask, const std::string & sessionId, ACE_Future<bool> & execResult)
	:m_pAuthenPeerTask(pAuthenPeerTask)
	{
		TA_Assert( pAuthenPeerTask != NULL );
		 m_sessionId = sessionId;
		 m_execResult = execResult;
	}

	int AuthenPeerTaskMO::call()
	{
		try
		{
			execute();
			m_execResult.set(true);
		}
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException, reason=%s", ex.what() );
			try
			{
				m_execResult.set(false);
			}
			catch(...)
			{
				LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set return value. Dead-lock has occurred." );
			}
        }
		catch( ... )
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Error talking to peers." );

			try
			{
				m_execResult.set(false);
			}
			catch(...)
			{
				LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set return value. Dead-lock has occurred." );
			}
		}
		
		return 0 ;
	}

} // TA_Base_Bus
