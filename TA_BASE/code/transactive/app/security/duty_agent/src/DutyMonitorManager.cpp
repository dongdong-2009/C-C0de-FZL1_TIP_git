/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitorManager.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of DutyMonitorManager.
  *
  */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyMonitorManager.h"
#include "app/security/duty_agent/src/DutyMonitorTask.h"
#include "app/security/duty_agent/src/DutyMonitorStateConverter.h"

namespace TA_Base_App
{
	
	DutyMonitorManager::DutyMonitorManager() :
		TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl>(*TA_Base_Bus::PeerStateChangeSubject::instance()),
		TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> (*TA_Base_Bus::AgentTerminateSubject::instance() ),
		TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::StateUpdateSubjectImpl>    (*TA_Base_Bus::StateUpdateSubject::instance()    )
	{
		FUNCTION_ENTRY("DutyMonitorManager");
		
		m_dutyMonitorTask.open(0);
		
		FUNCTION_EXIT;
	}


	DutyMonitorManager::~DutyMonitorManager()
	{
		FUNCTION_ENTRY("~DutyMonitorManager");

		stop();

		ACE_Message_Block* msg = new ACE_Message_Block;

		msg->msg_type( ACE_Message_Block::MB_HANGUP );
		if( TA_Base_Bus::DASConst::ACE_ERROR != m_dutyMonitorTask.putq(msg) )
		{
			m_dutyMonitorTask.wait();
		}

		FUNCTION_EXIT;
	}

	void DutyMonitorManager::start()
	{
		m_dutyMonitorTask.start();
	}

	void DutyMonitorManager::stop()
	{
		m_dutyMonitorTask.stop();
	}

	void DutyMonitorManager::update( TA_Base_Bus::PeerStateChangeSubjectImpl& peerStateChange, const TA_Base_Bus::DutyState & state )
	{
		FUNCTION_ENTRY("update");

        //TA_Base_Bus::DutyState* state = peerStateChange.getStateUpdate();

        // TD12082: Don't bother processing the update if no duty information is contained in it.
        if ( state.isEmpty() )
        {
            FUNCTION_EXIT;
            return;
        }

		DutyMonitorStateConverter dmsc(m_dutyMonitorTask);

		state.convert(dmsc);

		FUNCTION_EXIT;
	}

	void DutyMonitorManager::update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate )
	{
		FUNCTION_ENTRY("update");

		StateUpdateParent::detach();
		PeerStateParent::detach();
		AgentTerminateParent::detach();

		FUNCTION_EXIT;
	}

	void DutyMonitorManager::update( TA_Base_Bus::StateUpdateSubjectImpl& stateUpdate , const TA_Base_Bus::DutyState & state)
	{
		FUNCTION_ENTRY("update");

		DutyMonitorStateConverter dmsc(m_dutyMonitorTask);

		state.convert(dmsc);

		FUNCTION_EXIT;

	}
}
