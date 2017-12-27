/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventHelper.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_HELPER_H_INCLUDED__
#define __EVENT_HELPER_H_INCLUDED__

#include "core/event/src/EventItem.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"

#include "core/event/IDL/src/EventAgentCorbaDef.h"
#include "core/naming/src/NamedObject.h"

#include <string>

namespace TA_Base_Core
{
	class EventHelper
	{
	public: // Functions
		// Destructor
		~EventHelper(){};
		// Get Instance
		static EventHelper& getInstance();

		// Release Instance
		static void releaseInstance();

		// Call to initialize agent location
		void init(unsigned long _agentLocation);
	
		// Push events
		void submitEvent(TA_Base_Core::EventItemInfo* event);
		void submitEvents(TA_Base_Core::T_EventItemInfoList& events);
		
		// Retrieve all events
		void getAllEvents(std::string sessionID, TA_Base_Core::T_EventItemList& eventList);

		// Retrieve the latest events
		void getLatestEvents(std::string eventID, std::string sessionID, TA_Base_Core::T_EventItemList& eventList);

		// Retrieves the latest N events;
		void getTopEvents(unsigned long ulEventSize, std::string sessionID, TA_Base_Core::T_EventItemList& eventList);

		// Check event status
		bool checkStatus();

		// Set the call timeout to server
		void setAgentCallTimeout(unsigned long ulTimeout);

	private: // Functions
		// Hide to disable copy
		EventHelper();
		EventHelper(const EventHelper& rhs);
		EventHelper& operator=(const EventHelper& rhs);

	private: // Variables
		// Singleton instance of this class
		static EventHelper* m_pInstance;

		//Singleton lock
		static ReEntrantThreadLockable m_singletonLock; 
		
		// thread lock
		ReEntrantThreadLockable m_threadLock;

		//Event Agent
		typedef TA_Base_Core::NamedObject<TA_Base_Core::EventAgentCorbaDef,
			TA_Base_Core::EventAgentCorbaDef_ptr,
			TA_Base_Core::EventAgentCorbaDef_var> EventAgentNameObj;

		// event agent for propagation
		EventAgentNameObj m_eventAgentNameObj;

		// My Location
		unsigned long m_ulMyLocation;

		//Submit timeout
		unsigned short m_SubmitTimeout;
		//Query timeout
		unsigned short m_QueryTimeout;
		//Check Status timeout
		unsigned short m_CheckTimeout;
	};

}

#endif //__EVENT_HELPER_H_INCLUDED__

