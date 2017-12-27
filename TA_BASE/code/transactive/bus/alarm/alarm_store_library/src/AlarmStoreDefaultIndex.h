/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreDefaultIndex.h $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
*/
///////////////////////////////////////////////////////////
//  AlarmStoreIndex.h
//  Implementation of the Class AlarmStoreDefaultIndex
//  This class is only for indexing of default sorting
//  Created on:      24-Jan-2004 04:18:02 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmStoreDefaultIndex_31283A60_E0FE_4438_8609_D384C29A0C08__INCLUDED_)
#define AlarmStoreDefaultIndex_31283A60_E0FE_4438_8609_D384C29A0C08__INCLUDED_


#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockableWithMutex.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "AlarmSortByField.h"

#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
    class RightsLibrary; // Forward declaration
	class AlarmFilterSet;
	class AlarmSortingSet;
	class IAlarmStoreIndexChanges;  // Forward declaration
	
									/**
									* This class manages a list of alarm store items.  These are references to the
									* alarm store data.  These are filtered and sorted ready for immediate use by a
									* client application
									* @version 1.0
									* @created 24-Jan-2004 04:18:02 PM
	*/
	class AlarmStoreDefaultIndex : public TA_Base_Core::Thread
	{
		
	public:
		
	/**
			 * Max number of displayable alarms which is constant
		*/
		static const unsigned long MAX_DISPLAY_ALARMS;
		
		
		/**
		* Constructor.  The constructor will create a new store
		* @param indexID    Unique identifier for this index
		* @param callbackID    The callback to be associated with this index
		* @param filterSet    The filters to be used for this index
		* @param sortSet    The sorting to be used for this index
		* 
		*/
		AlarmStoreDefaultIndex(long indexID, IAlarmStoreIndexChanges& callback,long callbackID, 
			AlarmFilterSet& filterSet, TA_Base_Bus::RightsLibrary* rightsLibrary, bool severityProcessed);
		
		virtual ~AlarmStoreDefaultIndex();
		
		
		/* 
		* These methods below will be called by AlarmStoreManager
		*/
		
		
		/**
		* Determined if index can be re-used as it has the same filter.
		*/
		bool isSameFilter(const AlarmFilterSet& filterSet) const;
		
		/**
		* getFilterSet
		*
		* Retrieve the filter set for this index
		*
		* @return A reference to the filter set for this index
		*/
		const AlarmFilterSet& getFilterSet() const { return m_filterSet; };
		
		/**
		* This method will clear all the entries from the index.			 
		*/
		void clearAlarms();
		
		/**
		* Add new alarm to the index
		* @param alarm
		* 
		*/
		void addAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);
		
		/**
		* Add, move or remove alarm from index
		* @param alarmID
		* @param oldSeverity - needed to maintain severity counts in index 
		* @param oldUnacked - needed to maintain the not ack count
		* 
		*/
		
		void updateAlarm(const TA_Base_Core::AlarmDetailCorbaDef& oldAlarm, TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);
		
		/**
		* Remove alarm from index (if it is present)
		* @param alarmID
		* 
		*/
		void deleteAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);
		
		/**
		* Add another callback for this index
		* @param callbackID    ID of the callback to be added.
		* 
		*/
		void addCallback(long callbackID);

		/**
		 * Inform the callback to add all the current alarms of the index
		 * @param callbackID    ID of the callback to be added.
		 * 
		 */
		void addAllAlarmsToCallback(long callbackID);
		
		/**
		* Remove a callback from this index
		* @param callbackID    ID of callback to be removed
		* 
		*/
		void removeCallback(long callbackID);
		
		/**
		* Return the highest severity (most critical) alarm.
		*/			
		unsigned long getHighestSeverityAlarm();
		
		/**
		* Return the not ack count.
		*/
		unsigned long getNotAckAlarmCount() const { return ((m_openCount+m_normalCount)>MAX_DISPLAY_ALARMS)?MAX_DISPLAY_ALARMS:(m_openCount+m_normalCount); };
		
		/**
		* Return the total alarm count.
		*/
		unsigned long getTotalAlarmCount() const { return ((m_openCount+m_normalCount+m_ackedCount)>MAX_DISPLAY_ALARMS)?MAX_DISPLAY_ALARMS:(m_openCount+m_normalCount+m_ackedCount); };
		
		/**
		* Return the max display alarm indicator
		*/
		bool isMaxDisplayReached() const { return m_maxDisplayIsReached;};
		
		/**
		* Return the time of earliest alarm.
		*/
		unsigned long getEarliestSourceTime();
		
		/**
		* Return the time of latest alarm.
		*/
		unsigned long getLatestSourceTime();
		
		long getAlarmIndexID() const { return m_indexID; };
		
		/**
		* Determined if index is in use.  This will be true if any callbacks are defined.
		*/
		bool isUsed() ;
		
		/**
		* Determine if callback id is in this index.
		*/
		bool isCallbackIDPresent(long callbackID) ;

		bool isSeverityProcessed() { return m_isSeverityProcessed; };
		
		/**
		* getFilterSet
		*
		* Retrieve the filter set for this index
		*
		* @return A reference to the filter set for this index
		*/
		//const AlarmFilterSet& getFilterSet() const { return m_filterSet; };
		
		/**
		* setSessionId
		*
		* Sets the session ID to be used to check if an alarm is acknowledgable.
		*
		* @param The session ID.
		*/
		static void setSessionId(const std::string& sessionId)
		{
			s_sessionId = sessionId;
		}
		
		
		protected:
			/**
			* This is the work method for the thread.  It will create the new index
			* to avoid a long delay on the user corba call
				*/
			virtual void run();
			
			/**
			* The terminate() method should cause run() to return. eg: CorbaUtil::
			* GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
			* to call this method directly - call terminateAndWait() instead
			*/
			virtual void terminate();
		private:
			
			// Alarm severity map management functions
            void addAlarmSeverity( const TA_Base_Core::AlarmDetailCorbaDef& alarm );
            void removeAlarmSeverity( const TA_Base_Core::AlarmDetailCorbaDef& alarm );
			bool checkAckPermission(unsigned long messageTypeKey, unsigned long locationKey, unsigned long subsystemKey, unsigned long assocEntityKey) const;
			
			/*
			* Background thread communications
			*/
			struct JobQueueEntry
			{
				bool					isToAddSeverity;    // Indicates if the job is to add alarm severity or remove alarm severity
				unsigned long			entityKey;			// Indicates the entity key associated to the alarm
				unsigned long			locationKey;		// Indicates the location key associated to the alarm
				unsigned long			subsystemKey;		// Indicates the subsystem key associated to the alarm
				std::string				alarmId;			// Alarm ID
				unsigned long			messageTypeKey;		// Alarm type
				unsigned long			severity;			// Indicates the severity of the alarm
				
				JobQueueEntry()
				{
					entityKey =0;
					locationKey =0;
					subsystemKey =0;
					severity =0;
				}
				
			};
			
			
			std::string findAlarmIdAtPosition( unsigned long pos);			
			
			
			// This function is to find the position of the submitted alarm by Binary chop in the Index List
			// The index vector is sorted by descending timestamp
			// It will return -1 if the alarm can't be found
			long findAlarmInVectorList (const TA_Base_Core::AlarmDetailCorbaDef& alarm, 
				std::vector<const TA_Base_Core::AlarmDetailCorbaDef*>& vectorList);
			
			struct compare_alarmdisplaytime : public std::binary_function<const TA_Base_Core::AlarmDetailCorbaDef&,const TA_Base_Core::AlarmDetailCorbaDef&,bool>
			{    
			/**
			* operator()
			*
			* Determines the sort order of two alarm referrences. By default, standard template functions will only do
			* pointer comparrisons (not useful), so this method de-references the pointers.
			*
			* @param arg1 The "left hand side" of the function
			* @param arg2 The "right hand side" of the function
			*
			* @return True if arg1 should come before arg2 under current sorting conditions.
			*
			* NOTE: This function MUST be declared const, otherwise it will not compile (a restriction of overriding the template)
				*/
				
				inline bool operator()(const TA_Base_Core::AlarmDetailCorbaDef* arg1,const TA_Base_Core::AlarmDetailCorbaDef* arg2) const
				{
					if (arg1 == NULL || arg2 == NULL)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "arg1 == NULL || arg2 == NULL");
						return false;
					}
					
					if( arg1->displayTime.time == arg2->displayTime.time )
					{
						return arg1->displayTime.millitm > arg2->displayTime.millitm;
					}
					else return arg1->displayTime.time > arg2->displayTime.time;					
				}				
			};
			
			struct compare_alarmsourcetime : public std::binary_function<const TA_Base_Core::AlarmDetailCorbaDef&,const TA_Base_Core::AlarmDetailCorbaDef&,bool>
			{    
			/**
			* operator()
			*
			* Determines the sort order of two alarm referrences. By default, standard template functions will only do
			* pointer comparrisons (not useful), so this method de-references the pointers.
			*
			* @param arg1 The "left hand side" of the function
			* @param arg2 The "right hand side" of the function
			*
			* @return True if arg1 should come before arg2 under current sorting conditions.
			*
			* NOTE: This function MUST be declared const, otherwise it will not compile (a restriction of overriding the template)
				*/
				
				inline bool operator()(const TA_Base_Core::AlarmDetailCorbaDef* arg1,const TA_Base_Core::AlarmDetailCorbaDef* arg2) const
				{
					if (arg1 == NULL || arg2 == NULL)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "arg1 == NULL || arg2 == NULL");
						return false;
					}
					if( arg1->displayTime.time == arg2->displayTime.time )
					{
						return arg1->displayTime.millitm > arg2->displayTime.millitm;
					}
					else return arg1->displayTime.time > arg2->displayTime.time;					
				}				
			};
			
			void backupAlarmDetails( const TA_Base_Core::AlarmDetailCorbaDef& backupAlarm, TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);
			void updateAlarmDetails( TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);
			
			// Class is not cloneable. Hide the copy constructor and assignment operator
			AlarmStoreDefaultIndex(const AlarmStoreDefaultIndex& theAlarmStoreIndex);
			AlarmStoreDefaultIndex& operator=(const AlarmStoreDefaultIndex& rhs);
			
			private:
				//AlarmSortingSet* m_sortSetForIndex; // sort set for Display index in order of source time, severity & ack
				bool m_maxDisplayIsReached; // this indicator will be set to TRUE, if the MAX_DISPLAY_ALARMS is exceeded

				/**
				* Set of filters used for this index
				*/
				AlarmFilterSet& m_filterSet;
				
				
				// Critical maps, vectors and their associated locks
				
				/**
				*  Type for the index vector
				*/
				typedef std::vector<const TA_Base_Core::AlarmDetailCorbaDef*> AlarmIndexVector;
				
				
				// Type for alarm vectors in different alarm severity with same alarm state and ack_state
				// The key of the map is the severity
				typedef std::map<unsigned long, AlarmIndexVector> AlarmIndexMap;
				
				// AlarmIndexMap of the not_acked and open alarms
				AlarmIndexMap	m_openAlarmMap;
				
				// AlarmIndexMap of the normalized but not_acked alarms
				AlarmIndexMap	m_normalAlarmMap;
				
				// AlarmIndexMap of the acked and open alarms
				AlarmIndexMap	m_ackedAlarmMap;
				
				unsigned long	m_highestSeverity;
				
				unsigned long	m_lowestSeverity;
				
				/**
				*  Holds the set of pointers to alarms grouped according to severity
				*/
				typedef std::set<std::string> AlarmSet;
				
				/*
				* Map of severity counts for the alarms in the index.  This is kept updated as alarms 
				* are added/removed from the index.  It is used by the get highest severity function 
				* avoiding continual iteration through the list.  The map is indexed on severity and holds
				* alarm counts for each severity.
				*/
				typedef std::map<unsigned long,AlarmSet>  AlarmSeverityMap;
				
				/**
				*  Holds the severity maps to get highest and lowest severities
				*/
				AlarmSeverityMap                 m_alarmSeverityMap;
				
				
				/**
				*  Locks m_alarmIndexVector during updating or adding
				*/
				TA_Base_Core::ReEntrantThreadLockableWithMutex m_indexLock;
				
				
				/**
				*  Locks m_alarmSeverityMap from updating and deletion
				*/
				TA_Base_Core::ReEntrantThreadLockableWithMutex m_severityMapLock; 
				
				
				TA_Base_Bus::RightsLibrary*  m_rightsLibrary;// Used to check Acknowledge permission
				static std::string           s_sessionId; // One session for all indexes
				
				//const std::map<std::string, unsigned long>& m_alarmTypeNameToKeyMap; // Map to convert alarm type names into keys.
				
				/**
				* Index id for this index
				*/
				long m_indexID;
				
				
				/*
				* Not ack counter for display
				*
				*/
				//unsigned long m_notAckCount;
				
				// Counter of alarms open and not_acked
				unsigned long m_openCount;
				
				// Counter of alarms normalized and not_acked
				unsigned long m_normalCount;
				
				// Counter of alarms acked
				unsigned long m_ackedCount;
				
				/**
				* Set of filters used for this index
				*/
				//AlarmFilterSet& m_filterSet;
				
				
				/**
				* This holds a list of callbackID associated with this list.  The callbackID
				* simply hides the CorbaDef reference from this module.
				*/
				std::vector<long>				m_callbackIDList;
				TA_Base_Core::ReEntrantThreadLockableWithMutex   m_callbackIDListLock;

				IAlarmStoreIndexChanges&		m_callback;
				bool						         	m_terminate;
				TA_Base_Core::Semaphore                 m_jobQueueSemaphore;
				TA_Base_Core::ReEntrantThreadLockableWithMutex   m_jobQueueLock;
				std::vector<JobQueueEntry>				m_jobQueue;
				
				bool m_isUsingSourceTime;
				bool m_isSeverityProcessed;
	};
	
};
#endif // !defined(AlarmStoreDefaultIndex_31283A60_E0FE_4438_8609_D384C29A0C08__INCLUDED_)

