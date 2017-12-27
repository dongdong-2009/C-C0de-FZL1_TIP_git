/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/AgentStateControler.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "app/event/event_agent/src/AgentStateController.h"
#include "app/event/event_agent/src/EventAgent.h"
#include "app/event/event_agent/src/LocalRunParamDefinitions.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_App
{
	
	/**		
	 * Constructor
	 */
	AgentStateController::AgentStateController()
		: m_bterminated(false)
		, m_strAgentState("")
		, m_pObjAgent(0)

	{
		FUNCTION_ENTRY("AgentStateController");
		FUNCTION_EXIT;
	}
	
	/**		
	 * Destructor
	 */
	AgentStateController::~AgentStateController()
	{
		FUNCTION_ENTRY("~AgentStateController");
		terminateAndWait();
		FUNCTION_EXIT;
	}		
	
	/**
	* run
	*
	* Overrides the pure virtual run method declared in Thread
	*/
	void AgentStateController::run()
	{
		FUNCTION_ENTRY("run");
		std::string agentCurrState;
		std::string agentOldState;

		while(!m_bterminated)
		{	
			m_objSemaphore.wait();
			if (m_bterminated)
				break;

			{	// Enclose in code block to lock state change
				TA_Base_Core::ThreadGuard guard(m_threadLock);
				if (0 == m_strAgentState.compare(agentCurrState))
				{
					continue; //State is the same do nothing
				}
				agentOldState = agentCurrState;
				agentCurrState = m_strAgentState;

				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Agent State Change from %s to %s.", agentOldState.c_str(), agentCurrState.c_str());
			}
			
			if( 0 == agentCurrState.compare(RPARAM_CONTROL) ||
				0 == agentCurrState.compare(RPARAM_MONITOR) ||
				0 == agentCurrState.compare(RPARAM_STANDBY) )
			{
				m_pObjAgent->processStateChange(agentOldState, agentCurrState);
			}
			else
			{
				const char * errorMsg = "the control/monitor state is not set"; 
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMsg);
				throw TA_Base_Core::OperationModeException(errorMsg);
			}
		}

		FUNCTION_EXIT;
	}

	/**
	* terminate
	*/
	void AgentStateController::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_bterminated = true;
		m_objSemaphore.post();

		FUNCTION_EXIT;
	}

	/**
	* SetAgentState
	*/
	void AgentStateController::SetAgentState(std::string strAgentState)
	{
		FUNCTION_ENTRY("SetAgentState");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		m_strAgentState = strAgentState;
		m_objSemaphore.post();

		FUNCTION_EXIT;
	}

	/**
	* SetMonitoredAgent
	*/
	void AgentStateController::SetMonitoredAgent(EventAgent* _pObjAgent)
	{
		FUNCTION_ENTRY("SetMonitoredAgent");
		m_pObjAgent = _pObjAgent;

		FUNCTION_EXIT;
	}
}