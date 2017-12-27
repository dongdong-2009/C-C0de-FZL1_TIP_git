/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/AgentStateControler.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __AGENT_STATE_CONTROLER_H_INCLUDED__
#define __AGENT_STATE_CONTROLER_H_INCLUDED__


#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <string>

namespace TA_Base_App
{
	class EventAgent; //Forward declaration

	class AgentStateController
		: public TA_Base_Core::Thread
	{
	public: //functions
		/**		
		 * Constructor
		 */
		AgentStateController();		
		
		/**		
		 * Destructor
		 */
		~AgentStateController();		
		
		/**
		* run
		*
		* Overrides the pure virtual run method declared in Thread
		*/
		void run();

		/**
		* terminate
		*
		* Overrides the pure virtual terminate method declared in Thread
		*/
		void terminate();

		/**
		* SetAgentState
		*
		* Set the current state of this agent
		*/
		void SetAgentState(std::string strAgentState);

		/**
		* SetMonitoredAgent
		*
		* Set the agent to monitor
		*/
		void SetMonitoredAgent(EventAgent* _pObjAgent);

	private: // functions

	private: // variables
		// Flag for terminate thread
		volatile bool m_bterminated;

		// Signal to process new state
		TA_Base_Core::Semaphore m_objSemaphore;

		// Thread lock when setting new state
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;

		// Current State
		std::string m_strAgentState;

		// Pointer to EventAgent
		EventAgent* m_pObjAgent;
	};
}

#endif //__AGENT_STATE_CONTROLER_H_INCLUDED__