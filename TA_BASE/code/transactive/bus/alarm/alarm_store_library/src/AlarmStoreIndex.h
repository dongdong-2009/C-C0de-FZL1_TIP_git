/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreIndex.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmStoreIndex.h
//  Implementation of the Class AlarmStoreIndex
//  Created on:      24-Jan-2004 04:18:02 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmStoreIndex_31283A60_E0FE_4438_8609_D384C29A0C08__INCLUDED_)
#define AlarmStoreIndex_31283A60_E0FE_4438_8609_D384C29A0C08__INCLUDED_


#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockableWithMutex.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "AlarmSortByField.h"
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
	class AlarmStoreIndex : public TA_Base_Core::Thread
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
			AlarmStoreIndex(long indexID, IAlarmStoreIndexChanges& callback,long callbackID, 
				            AlarmFilterSet& filterSet, AlarmSortingSet& sortSet, TA_Base_Bus::RightsLibrary* rightsLibrary, bool severityProcessed);

			virtual ~AlarmStoreIndex();


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
			 * Determined if index is in use.  This will be true if any callbacks are defined.
			 */
			bool isUsed() ;

			/**
			 * Determine if callback id is in this index.
			 */
			bool isCallbackIDPresent(long callbackID) ;

			/**
			 * Determined if index can be re-used as it has the same filter and sorting.
			 */
			bool isSameFilterAndSort(const AlarmFilterSet& filterSet, const AlarmSortingSet& sortSet) const;


			long getAlarmIndexID() const { return m_indexID; };

			/**
			 * Return the highest severity (most critical) alarm.
			 */
			//unsigned long getHighestSeverityAlarm() const;
			unsigned long getHighestSeverityAlarm();

			/**
			 * Return the not ack count.
			 */
			unsigned long getNotAckAlarmCount() const { return (MAX_DISPLAY_ALARMS<m_notAckCount)?MAX_DISPLAY_ALARMS:m_notAckCount;}; 

			/**
			 * Return the total alarm count.
			 */
			unsigned long getTotalAlarmCount() const { return (MAX_DISPLAY_ALARMS<m_totalCount)?MAX_DISPLAY_ALARMS:m_totalCount;};  

			/**
			 * Return the max display alarm indicator
			 */
			bool isMaxDisplayReached() const { return m_maxDisplayIsReached;};

			bool isSeverityProcessed() const { return m_isSeverityProcessed;};

            /**
             * getFilterSet
             *
             * Retrieve the filter set for this index
             *
             * @return A reference to the filter set for this index
             */
            const AlarmFilterSet& getFilterSet() const { return m_filterSet; };

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

            /**
			 * Return the time of earliest alarm.
			 */
            unsigned long getEarliestSourceTime();

            /**
			 * Return the time of latest alarm.
			 */
            unsigned long getLatestSourceTime();


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
			// Critical maps, vectors and their associated locks
		private:

			// Class is not cloneable. Hide the copy constructor and assignment operator
			AlarmStoreIndex(const AlarmStoreIndex& theAlarmStoreIndex);
			AlarmStoreIndex& operator=(const AlarmStoreIndex& rhs);

			// Alarm severity map management functions
            void addAlarmSeverity( const TA_Base_Core::AlarmDetailCorbaDef& alarm );
            void removeAlarmSeverity( const TA_Base_Core::AlarmDetailCorbaDef& alarm );
			bool checkAckPermission(unsigned long messageTypeKey, unsigned long locationKey, unsigned long subsystemKey, unsigned long assocEntityKey ) const;

			// This function is to find the insert pos of a new or updated alarm for the vector using binary sort and the m_sortSetForIndex
			unsigned long findInsertPositionByBinaryChop(const TA_Base_Core::AlarmDetailCorbaDef& alarm, std::vector<const TA_Base_Core::AlarmDetailCorbaDef*>& vectorlist, AlarmSortingSet& sortSet);

			/**
			 * 
			 * The following attributes and operations are to determine the alarms to be Displayed and added
			 * from the current list of indexed alarms to maintain the max number allowed for display
			 * It does not affect the alarm store alarms and update the client if the alarms exceeded 
			 * the MAX_DISPLAY_ALARMS.
			 *
			 */			
			
			bool m_maxDisplayIsReached; // this indicator will be set to TRUE, if the MAX_DISPLAY_ALARMS is exceeded			

			long findAlarmInVectorList (const TA_Base_Core::AlarmDetailCorbaDef& alarm);

			long findAlarmInTimeIndexVectorList (const TA_Base_Core::AlarmDetailCorbaDef& alarm);
			/** 
			  * AlarmStoreIndex::insertAlarm
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

			

			struct compare_alarmdisplaytime : public std::binary_function<const TA_Base_Core::AlarmDetailCorbaDef&,const TA_Base_Core::AlarmDetailCorbaDef&,bool>
			{    
				/**
				 * operator()
				 *
				 * Determines the sort order of two alarm references. By default, standard template functions will only do
				 * pointer comparisons (not useful), so this method de-references the pointers.
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
				 * Determines the sort order of two alarm references. By default, standard template functions will only do
				 * pointer comparisons (not useful), so this method de-references the pointers.
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
					if( arg1->sourceTime.time == arg2->sourceTime.time )
					{
						return arg1->sourceTime.millitm > arg2->sourceTime.millitm;
					}
					else return arg1->sourceTime.time > arg2->sourceTime.time;					
				}				
			};



			void backupAlarmDetails( const TA_Base_Core::AlarmDetailCorbaDef& backupAlarm, TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);

			void updateAlarmDetails( TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);
			
		private:

			/**
			 *  Type for the index vector
			 */
			typedef std::vector<const TA_Base_Core::AlarmDetailCorbaDef*> AlarmIndexVector;

			/**
			 *  Vector of alarms not to be displayed in sorted order of time. Sorted by time, severity and ack_by
			 */
			AlarmIndexVector m_alarmIndexVector; 

			/**
			 *  Locks m_alarmIndexVector during updating or adding
			 */
			AlarmIndexVector m_alarmTimeIndexVector;

			/**
			 *  Locks m_alarmIndexVector during updating or adding
			 */
			TA_Base_Core::ReEntrantThreadLockableWithMutex m_indexLock;
			
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
			 *  Locks m_alarmSeverityMap from updating and deletion
			 */
			TA_Base_Core::ReEntrantThreadLockableWithMutex m_severityMapLock; 
			
			bool m_isUsingSourceTime;

			bool m_isSeverityProcessed;

			bool									m_terminate;

			TA_Base_Core::Semaphore					m_jobQueueSemaphore;

			TA_Base_Core::ReEntrantThreadLockableWithMutex	m_jobQueueLock;
			
			std::vector<JobQueueEntry>				m_jobQueue;
			
			TA_Base_Bus::RightsLibrary*  m_rightsLibrary;// Used to check Acknowledge permission

            static std::string           s_sessionId; // One session for all indexes
            
			/**
			 * Index id for this index
			 */
			long m_indexID;

			/*
			* Not ack counter for display
			*
			*/
			unsigned long volatile m_notAckCount;

			/*
			* Not ack counter for display
			*
			*/
			unsigned long volatile m_totalCount;

			/**
			 * Set of filters used for this index
			 */
			AlarmFilterSet& m_filterSet;
			
			/**
			 * Set of sorts used for this index
			 */
			AlarmSortingSet& m_sortSet;

			/**
			 * This holds a list of callbackID associated with this list.  The callbackID
			 * simply hides the CorbaDef reference from this module.
			 */
			std::vector<long>				m_callbackIDList;
			TA_Base_Core::ReEntrantThreadLockableWithMutex   m_callbackIDListLock;
			
			IAlarmStoreIndexChanges&		m_callback;		
	};
};
#endif // !defined(AlarmStoreIndex_31283A60_E0FE_4438_8609_D384C29A0C08__INCLUDED_)

