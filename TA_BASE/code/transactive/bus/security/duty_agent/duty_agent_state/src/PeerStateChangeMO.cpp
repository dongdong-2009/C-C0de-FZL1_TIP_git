// PeerStateChangeMO.cpp: implementation of the PeerStateChangeMO class.
//
//////////////////////////////////////////////////////////////////////

#include "DutyAgentState.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	PeerStateChangeMO::PeerStateChangeMO(DutyState& state):m_state(state)
	{
	
	}
	
	PeerStateChangeMO::~PeerStateChangeMO()
	{
		
	}
	int PeerStateChangeMO::call()
	{
		try
		{
			PeerStateChangeSubject::instance()->update(m_state);
		}
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException, reason=%s", ex.what() );			
        }
		catch( ... )
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Error update PeerStateChangeSubject." );			
		
		}
		
		return 0 ;
	}
}

