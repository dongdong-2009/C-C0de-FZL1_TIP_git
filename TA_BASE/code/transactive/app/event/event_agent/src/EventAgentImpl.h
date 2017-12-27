/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/EventAgentImpl.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_AGENT_IMPL_H_INCLUDED__
#define __EVENT_AGENT_IMPL_H_INCLUDED__

#include "app/event/event_agent/src/PropagateActor.h"
//#include "app/event/event_agent/src/NonReentrantRWThreadLockable.h"

#include "core/corba/src/ServantBase.h"
#include "core/event/IDL/src/EventAgentCorbaDef.h"
#include "core/event/IDL/src/EventDetailCorbaDef.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
	class EventAgentImpl :  public virtual POA_TA_Base_Core::EventAgentCorbaDef,
		public virtual TA_Base_Core::ServantBase
	{
	public:
		 /**
          * Constructor
          */
        EventAgentImpl();

        /**
         * Destructor
         */
        virtual ~EventAgentImpl();

		/**
         * submitEvent
		 *
		 * This method allows clients to submit a new event
		 *
		 * @param The event message
		 */
		void submitEvent(const ::TA_Base_Core::EventInfoCorbaDef& p_eventData);

		/**
         * submitEvents
		 *
		 * This method allows Local Event Source clients to submit a batch of new events
		 *
		 * @param The event message sequence
		 */
		void submitEvents(const ::TA_Base_Core::EventInfoSequence& p_eventDataSeq);
		
		/**
         * submitEventsEx
		 *
		 * This method allows Event Agent clients to submit a batch of new events
		 *
		 * @param The event message sequence
		 * @param location key of the Source Event Agent
		 */
		void submitEventsEx(const ::TA_Base_Core::EvDataPkgCorbaDef& p_eventDataSeq, ::CORBA::ULong p_ulSourceLoc);
		
		/**
         * getAllEvents
		 *
		 * This method allows clients retrieve all the existing events in the cache.
		 *
		 * @param The Session ID for rights checking
		 */
		TA_Base_Core::EvDataPkgCorbaDef* getAllEvents(const char* sessionID);

		/**
         * getLatestEvents
		 *
		 * This method allows clients retrieve the latest events.
		 *
		 * @param event ID of the lastest event on the the client
		 * @param The Session ID for rights checking
		 */
		TA_Base_Core::EvDataPkgCorbaDef* getLatestEvents(const char* eventID, const char* sessionID);
		
		/**
         * getRelatedEvents
		 *
		 * This method allows clients retrieve the events related to a particular alarm.
		 *
		 * @param alarm ID 
		 * @param The Session ID for rights checking
		 */
		TA_Base_Core::EvDataPkgCorbaDef* getRelatedEvents(const char* alarmID, const char* sessionID);

		/**
          * getSyncData
          *
          * This method allows monitor agent to retrieve data which were not synchronized.
          *
          * @param The last known sequence number from the monitor agent
          */
		TA_Base_Core::EventSyncCorbaDef* getSyncData(const char* p_eventID);

		/**
		 * initialize
		 *
		 * performs all necessary initialization
		 */
		void initialize();

		/**
		 * changeStateProcess
		 *
		 * change the state specific processes
		 */
		void changeStateProcess(const std::string& oldState, const std::string& newState);

		/**
          * checkStatus
          *
          * This method allows client to check the agents status.
          *
          * @param: none
          */
		::CORBA::Boolean checkStatus();

		/**
          * getTopEvents
          *
          * This method allows client to retreive the lastest N events based on the size specified
		  * in the paramter.
          *
          * @param The size of the events required by the client
		  * @param The session ID
          */
		TA_Base_Core::EvDataPkgCorbaDef* getTopEvents(::CORBA::ULong p_ulEventSize, const char* sessionID);
		
	private: //Functions
		// Disable copy constructor and assignment operator
		EventAgentImpl( const EventAgentImpl& theEventAgentImpl);
		EventAgentImpl& operator=(const EventAgentImpl &);

		/**
		 * EnsureInControlState
		 */
		void EnsureInControlState();

		/**
		 * EnsureInControlState
		 */
		void _stopCurrentStateProcess(const std::string& oldState);


	private: //Variables
		// thread lock
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		//NonReentrantRWThreadLocable m_threadLock;
		
		// Propagation Mechanism
		PropagateActor* m_pPropagateActor;

		// Use to break the state change loop waiting for loadData
		volatile bool m_bStopDataLoading;
	};
}

#endif //__EVENT_AGENT_IMPL_H_INCLUDED__