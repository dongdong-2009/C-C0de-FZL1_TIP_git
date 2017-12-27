/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskMO.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  A base class for DutyPeerTask method objects that encapsulates the error
  *  handling.
  */

#include "DutyAgentState.h"
#include <ctime>
using TA_Base_Core::DebugUtil;
namespace TA_Base_Bus
{
	
	const unsigned long DutyPeerTaskMO::MAX_PEER_TIMEOUT = 300;
	const unsigned long DutyPeerTaskMO::TIME_OF_FLIGHT = 10;
	unsigned long DutyPeerTaskMO::m_timeout=0;
	
	DutyPeerTaskMO::DutyPeerTaskMO( const ACE_Future<bool>& future ) :
		m_future(future)
	{
		 m_createtime=time(NULL); //CL-21317, luohuirong, dutyAgent hang
	}
    
	//CL-21317, luohuirong, dutyAgent hang
	unsigned long DutyPeerTaskMO::getTimeOutSeconds()
	{
		if (m_timeout<=0 || m_timeout> MAX_PEER_TIMEOUT)
		{					
			try
			{
				m_timeout = DutyDbCache::instance()->getPopupTimeout().sec() + TIME_OF_FLIGHT;				
			}
			catch(...)
			{
				m_timeout=TIME_OF_FLIGHT*2;
				LOG_GENERIC( SOURCE_INFO, GENERIC_LOG_TYPE, FATAL_LOG_LEVEL, "DutyPeerTaskMO fail to get DutyDbCache::instance()->getPopupTimeout");
			}
			if( m_timeout > MAX_PEER_TIMEOUT )
			{
				m_timeout = MAX_PEER_TIMEOUT;
			}
			if(m_timeout<=0)
			{
				m_timeout=TIME_OF_FLIGHT*2;
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DutyPeerTaskMO set timeout to=%d", m_timeout);
		}
		return  m_timeout;
	}

	int DutyPeerTaskMO::call()
	{
		try
		{
			unsigned long currentTime=time(NULL);
			unsigned long maxTimeout=m_createtime+getTimeOutSeconds();
			if (currentTime>=maxTimeout)  //CL-21317, luohuirong, dutyAgent hang
			{
				try
				{
					m_future.set(false);
				}
				catch(...)
				{
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, FATAL_LOG_LEVEL, "Failed to set return value. Dead-lock has occurred." );
				}
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "DutyPeerTaskMO run task timeout currentTime=%d  maxTimeout=%d", currentTime,maxTimeout);							 
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DutyPeerTaskMO run task begin");
				execute();
				m_future.set(true);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DutyPeerTaskMO run task end");
			}		
		}
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TransactiveException", ex.what() );

			try
			{
				m_future.set(false);
			}
			catch(...)
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, FATAL_LOG_LEVEL, "Failed to set return value. Dead-lock has occurred." );
			}
        }
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Error talking to peers." );

			try
			{
				m_future.set(false);
			}
			catch(...)
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, FATAL_LOG_LEVEL, "Failed to set return value. Dead-lock has occurred." );
			}
		}
		
		return(0);
	}

} // TA_Base_Bus
