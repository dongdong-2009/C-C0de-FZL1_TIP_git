/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/EventCache.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_CACHE_H_INCLUDED__
#define __EVENT_CACHE_H_INCLUDED__

#include "app/event/event_agent/src/EventDataIndices.h"

#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/event/src/EventItem.h"

#include "core/event/IDL/src/EventDetailCorbaDef.h"
#include "core/event/IDL/src/EventSyncCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
//#include "app/event/event_agent/src/NonReentrantRWThreadLockable.h"

#include <string>
#include <list>
#include <vector>

namespace TA_Base_App
{
	class EventCache
	{
	public: // functions
		/**
          * Destructor
          */
		~EventCache();

		/**
          * getInstance
		  * 
		  * Static function to retrieve the single instance of this class
          */
		static EventCache& getInstance();

		/**
          * releaseInstance
		  * 
		  * releases or free the single instance object of this class
          */
		static void releaseInstance();

		/**
          * initialize
		  * 
		  * performs all necessary initialization needed for this class
          */
		void initialize();

		/**
          * insertEvents
		  * 
		  * inserts multiple events into the cache.
		  *
		  * @Param TA_Base_Core::EventSequence* pEvents pointer to the event sequence to submit
          */
		void insertEvents(TA_Base_Core::EventSequence* pEvents);
		void insertEvents(TA_Base_Core::EvDataPkgCorbaDef* pEvents);
		
		/**
          * insertEvents
		  * 
		  * inserts multiple converted events into the cache.
		  *
		  * @Param std::vector < EventData* >& pEvents pointer to the event sequence to submit
          */
		void insertEvents(std::vector < TA_Base_Core::EventItem* >& pEvents);		

		/**
          * insertEvent
		  * 
		  * inserts a single event into the cache.
		  *
		  * @Param TA_Base_Core::EventSequence* pEvents pointer to the event sequence to submit
          */
		void insertEvent(TA_Base_Core::EventDetailCorbaDef* pEvent);

		/**
          * getLatestEvents
		  * 
		  * retrieves all the latest events newer than the event ID provided.
		  *
		  * @Param const std::string& eventID event ID of the latest event from client
		  * @Param const std::string& sessionID session ID of the client
		  *
		  * @Return a pointer to sequence of events
          */
		TA_Base_Core::EvDataPkgCorbaDef* getLatestEvents(const std::string& eventID, const std::string& sessionID );
		
		/**
          * getAllEvents
		  * 
		  * retrieves all the events.
		  *
		  * @Param const std::string& sessionID session ID of the client
		  *
		  * @Return a pointer to sequence of events
          */
		TA_Base_Core::EvDataPkgCorbaDef* getAllEvents(const std::string& sessionID );
		
		/**
          * getTopEvents
          *
          * This method allows client to retreive the lastest N events based on the size specified
		  * in the paramter.
          *
          * @param The size of the events required by the client
		  * @param The session ID
          */
		TA_Base_Core::EvDataPkgCorbaDef* getTopEvents(unsigned long p_ulEventSize, const char* sessionID);

		/**
          * getRelatedEvents
		  * 
		  * retrieves all the related events based on the alarmID passed.
		  *
		  * @Param const std::string& alarmID ID of the alarm
		  * @Param const std::string& sessionID session ID of the client\
		  *
		  * @Return a pointer to sequence of events
          */
		TA_Base_Core::EvDataPkgCorbaDef* getRelatedEvents(const std::string& alarmID, const std::string& sessionID);
		
		/**
          * getSyncData
		  * 
		  * retrieves the event data and information need to synchronize with monitor agent.
		  *
		  * @Param const std::string& p_eventID the ID of the latest event from the monitor agent.
		  *
		  * @Return a pointer to the EventSyncCorbaDef structure
          */
		TA_Base_Core::EventSyncCorbaDef* getSyncData(const std::string& p_eventID);
		
		/**
          * proccessSyncData
		  * 
		  * processes the data retrieve from the controlling agent.
		  *
		  * @Param TA_Base_Core::EventSyncCorbaDef* pSyncData a pointer to the sync data to process
		  */
		void proccessSyncData(TA_Base_Core::EventSyncCorbaDef* pSyncData);

		/**
          * proccessDbData
		  * 
		  * processes the data retrieve from database.
		  *
		  * @Param std::list<TA_Base_Core::ICombinedEventData*>& events data from database to process.
		  */
		void proccessDbData(std::list<TA_Base_Core::ICombinedEventData*>& events, unsigned long ulLatestDbSeq);

		/**
          * getLatestEventID
		  * 
		  * retrieves the latest event ID from cache order by sequence number.
		  *
		  * @Return latest event ID.
		  */
		std::string getLatestEventID(); // Data will be retrieve via multi index
		
		/**
          * getLatestDbSeqNumber
		  * 
		  * retrieves the latest database sequence number.
		  *
		  * @Return the last known database sequence number.
		  */
		unsigned long getLatestDbSeqNumber(); // Data will be retrieve via multi index

		/**
          * deleteItem
		  * 
		  * deallocated an event data.
		  */
		int deleteItem(TA_Base_Core::EventItem* pEventData);

		/**
          * deleteAllCacheData
		  * 
		  * deletes all existing events from cache.
		  */
		void deleteAllCacheData();

		/**
		  * findEvent
		  *
		  * Find the event from the cache
		  *
		  * @Returns true the event is found otherwise false
		  */
		bool findEvent(const std::string& strEventID);
		
	private: //functions
		EventCache();
		// Disable copy constructor and assignment operator
		EventCache( const EventCache& theEventCache);
		EventCache& operator=(const EventCache &);

		/**
          * _getNextMsgSeqNum
		  * 
		  * retrieves the next sequence number.
		  */
		const TA_Base_Core::EvMgsSequenceNumber& _getNextMsgSeqNum();

		/**
          * _insertInternal
		  * 
		  * performs the actual insertion of the event data to the cache.
		  */
		void _insertInternal(TA_Base_Core::EventDetailCorbaDef* pEvent);
		
		void _insertInternal(TA_Base_Core::EventItem* pEventData);
		/**
          * _getLatestEventsInternal
		  * 
		  * performs the actual retrieval of the events.
		  */
		TA_Base_Core::EvDataPkgCorbaDef* _getLatestEventsInternal(const std::string& eventID, const std::string& sessionID);
		
		/**
          * _dbDataToEventData
		  * 
		  * data conversion from dbdata to event data.
		  */
		TA_Base_Core::EventItem* _dbDataToEventData(TA_Base_Core::ICombinedEventData* pEventData);

		/**
          * _corbaDefToEventData
		  * 
		  * data conversion from corba define to event data.
		  */
		TA_Base_Core::EventItem* _corbaDefToEventData(TA_Base_Core::EventDetailCorbaDef* pEvent);

		/**
          * _eventDataToCorbaDef
		  * 
		  * data conversion from event data to corba define.
		  */
		void _eventDataToCorbaDef(TA_Base_Core::EventItem* pEventData, TA_Base_Core::EventDetailCorbaDef& refRetVal);

		/**
          * _pushToCache
		  * 
		  * check cache size and remove the oldest event if maximum size has been reach and inserts
		  * new event into cache.
		  */
		void _pushToCache(TA_Base_Core::EventItem* pEventData);
		
	private: //variables
		// Instance of this class
		static EventCache* m_pInstance;
		
		// static lock for singleton object creation and retrieval
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
		
		// lock for thread access
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		//NonReentrantRWThreadLocable m_threadLock;

		// container for the cached events
		EvCacheData m_evCacheData;

		//Enforce Maximum cache size
		unsigned long m_ulMaxCacheSize;

		//Message sequence number
		TA_Base_Core::EvMgsSequenceNumber m_evMsgSeqNumber;

		//Latest Database sequence number
		unsigned long m_ulLastestDbSeq;

	};
}

#endif //__EVENT_CACHE_H_INCLUDED__