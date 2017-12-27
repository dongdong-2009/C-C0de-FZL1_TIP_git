/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $Source: /cvs/1TS-9999-T/code/transactive/bus/alarm/alarm_store_library/src/AlarmStoreIndex.cpp,v $
* @author:  Andy Parker
* @version: $Revision: #1 $
*
* Last modification: $Date: 2015/01/19 $
* Last modified by:  $Author: CM $
*
* This class implements the alarm store index.
*
*/

///////////////////////////////////////////////////////////
//  AlarmStoreIndex.cpp
//  Implementation of the Class AlarmStoreIndex
//  Created on:      24-Jan-2004 04:18:02 PM
///////////////////////////////////////////////////////////

#include <algorithm>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"
#include "bus/alarm/alarm_store_library/src/AlarmStoreIndex.h"
#include "bus/alarm/alarm_store_library/src/AlarmFilterSet.h"
#include "bus/alarm/alarm_store_library/src/AlarmSortingSet.h"
#include "bus/alarm/alarm_store_library/src/IAlarmStoreIndexChanges.h"

#pragma warning( disable : 4503 )


namespace TA_Base_Bus
{
	
	std::string AlarmStoreIndex::s_sessionId = "";
	
	const unsigned long AlarmStoreIndex::MAX_DISPLAY_ALARMS = 30000;
	
	/**
	* Constructor.  The constructor will create a new store
	* @param indexID    Unique identifier for this index
	* @param callbackID    The callback to be associated with this index
	* @param filterSet    The filters to be used for this index
	* @param sortSet    The sorting to be used for this index
	* 
	*/
	AlarmStoreIndex::AlarmStoreIndex(long indexID, IAlarmStoreIndexChanges& callback, long callbackID, 
		AlarmFilterSet& filterSet, AlarmSortingSet& sortSet, TA_Base_Bus::RightsLibrary* rightsLibrary, bool severityProcessed)
		: m_filterSet(filterSet),
		m_sortSet(sortSet),
		m_callback(callback),
		m_indexID(indexID),
		m_notAckCount(0),
		m_totalCount(0),
		m_terminate(false),
		m_rightsLibrary(rightsLibrary),
		m_maxDisplayIsReached(false),
		m_isUsingSourceTime(false),
		m_isSeverityProcessed(severityProcessed)
	{
		FUNCTION_ENTRY("Constructor");
		
		// reserve the size of the vector to prevent any reallocation problem
		addCallback(callbackID);
		addAllAlarmsToCallback(callbackID);
		
		m_isUsingSourceTime = TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_USESOURCETIME.c_str());
		
		start();
	}
	
	AlarmStoreIndex::~AlarmStoreIndex()
	{
		FUNCTION_ENTRY("Destructor");
		
		TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
		TA_Base_Core::ThreadGuard guardSeverity(m_severityMapLock);
		TA_Base_Core::ThreadGuard guard(m_jobQueueLock);
		
		terminateAndWait();
		
		AlarmFilterSet* fptr = &m_filterSet;
		if ( fptr != NULL )
		{
			delete fptr;
			fptr = NULL;
		}
		
		AlarmSortingSet* sptr = &m_sortSet;	
		if (sptr != NULL)
		{
			delete sptr;
			sptr = NULL;
		}
		
		// Clear all the maps and vectors
		if (m_callbackIDList.empty() == false) m_callbackIDList.clear();
		if (m_alarmSeverityMap.empty() == false) m_alarmSeverityMap.clear();
		if (m_alarmIndexVector.empty() == false) m_alarmIndexVector.clear();
		if (m_alarmTimeIndexVector.empty() == false) m_alarmTimeIndexVector.clear();
		
		m_jobQueue.clear();
		
		FUNCTION_EXIT;
	}
	
	
	void AlarmStoreIndex::clearAlarms()
	{
		FUNCTION_ENTRY("clearAlarms");
		
		TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
		TA_Base_Core::ThreadGuard guardSeverity(m_severityMapLock);
		TA_Base_Core::ThreadGuard guard(m_jobQueueLock);
		TA_Base_Core::ThreadGuard callListguard(m_callbackIDListLock);
		
		m_alarmIndexVector.clear();
		m_alarmTimeIndexVector.clear();
		m_alarmSeverityMap.clear();  
		m_jobQueue.clear();
		
		m_maxDisplayIsReached = false;
		
		m_notAckCount = 0;
		m_totalCount = 0;
		
		// Tell all the clients
		for (unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
		{
			m_callback.indexCleared(m_callbackIDList[iClient],m_indexID);
		}
		
		FUNCTION_EXIT;
	}
	
	/**
	* Add new alarm to the index
	* @param alarm
	* 
	*/
	void AlarmStoreIndex::addAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
	{
		FUNCTION_ENTRY("addAlarm");
		
		TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
		TA_Base_Core::ThreadGuard guard(m_callbackIDListLock);
		
		if( !m_filterSet.passesFilters( alarm))
		{
			// do nothing is can not pass the filters
			FUNCTION_EXIT;
			return;
		}
		
		try
		{
			// add alarm to the display index vector
			long pos = findInsertPositionByBinaryChop( alarm, m_alarmIndexVector, m_sortSet);
			
			AlarmIndexVector::iterator itr = m_alarmIndexVector.begin()+pos;
			
			m_alarmIndexVector.insert(itr, &alarm);
			
			// add alarm to the time index vector
			
			if( false == m_isUsingSourceTime)
			{
				itr = std::upper_bound( m_alarmTimeIndexVector.begin(), m_alarmTimeIndexVector.end(), &alarm, compare_alarmdisplaytime());
			}
			else
			{
				itr = std::upper_bound( m_alarmTimeIndexVector.begin(), m_alarmTimeIndexVector.end(), &alarm, compare_alarmsourcetime());
			}
			m_alarmTimeIndexVector.insert(itr, &alarm);
			
			// check
			if( m_alarmIndexVector.size() >= MAX_DISPLAY_ALARMS)
			{
				m_maxDisplayIsReached = true;
			}
			
			std::string ackBy(alarm.alarmAcknowledgedBy.in());
			if( ackBy.length()==0 )
			{
				m_notAckCount++;
				
				// add alarm to the alarm severity map
				addAlarmSeverity(alarm);
			}
			m_totalCount++;
			
			if( pos < MAX_DISPLAY_ALARMS )
			{
				// need to update the display of the client applications
				
				if( m_maxDisplayIsReached )
				{
					// remove the alarm in the bottom of the display index
					
					unsigned int iClient = 0;
					for (iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexDeleted( m_callbackIDList[iClient],m_indexID, MAX_DISPLAY_ALARMS-1,
							std::string(m_alarmIndexVector[MAX_DISPLAY_ALARMS-1]->alarmID.in()));
					}				
					
					// add the new alarm to the display index
					
					for ( iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, pos,
							std::string(alarm.alarmID.in() ));
					}
				}
				else
				{
					// add the new alarm to the display index
					
					for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, pos,
							std::string(alarm.alarmID.in() ));
						
						if( m_alarmIndexVector.size() >= MAX_DISPLAY_ALARMS )
						{
							m_maxDisplayIsReached = true;
						}
					}					
				}
			}
			
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"addAlarm: Failed to added alarm %s in alarm store index %d. Caught TransactiveException: %s", 
				alarm.alarmID.in(), m_indexID, ex.what());
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"addAlarm: Failed to added alarm %s in alarm store index %d. Caught Corba Exception",
				alarm.alarmID.in(), m_indexID);
		}
		catch (...) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "addAlarm: Failed to added alarm %s in alarm store index %d", alarm.alarmID.in(), m_indexID);
		}
		
		FUNCTION_EXIT;
}

void AlarmStoreIndex::deleteAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("deleteAlarm");
	TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
	TA_Base_Core::ThreadGuard guard(m_callbackIDListLock);
	try
	{
		//delete alarm from the display index vector
		long pos = findAlarmInVectorList(alarm);
		
		if( pos == -1)
		{
			// non-existing alarm, do nothing here
			FUNCTION_EXIT;
			return;
		}
		AlarmIndexVector::iterator itr = m_alarmIndexVector.begin()+pos;
		
		
		m_alarmIndexVector.erase(itr);
		
		//delete alarm from the time index vector
		long timeIndexPos = findAlarmInTimeIndexVectorList(alarm);
		
		if( timeIndexPos == -1)
		{
			TA_ASSERT(false, "The alarm is found in the display index vector but not found in the time index vector, there must be something missing");
		}
		itr = m_alarmTimeIndexVector.begin() + timeIndexPos;
		m_alarmTimeIndexVector.erase(itr);		
		
			std::string ackBy(alarm.alarmAcknowledgedBy.in());
			if( ackBy.length()==0)
			{
				m_notAckCount--;
				
				removeAlarmSeverity(alarm);
			}
			m_totalCount--;
		
		if( pos < MAX_DISPLAY_ALARMS)
		{
			// Tell all client applications about the deletion
			for (unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
			{
				m_callback.indexDeleted(m_callbackIDList[iClient],m_indexID, pos,
					std::string(alarm.alarmID.in() ));
			}
		}				
		
		if( m_maxDisplayIsReached && m_alarmIndexVector.size() < MAX_DISPLAY_ALARMS  )
		{
			m_maxDisplayIsReached = false;
		}			
	}
	catch (const TA_Base_Core::TransactiveException& ex)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"deleteAlarm: Failed to delete alarm %s from alarm store index %d. Caught TransactiveException: %s", 
			alarm.alarmID.in(), m_indexID, ex.what());
	}
	catch( const CORBA::Exception& ex)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
			"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
			"deleteAlarm: Failed to delete alarm %s from alarm store index %dd. Caught Corba Exception",
			alarm.alarmID.in(), m_indexID);
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "deleteAlarm: Failed to delete alarm %s from alarm store index %d", alarm.alarmID.in(), m_indexID);
	}
	
	FUNCTION_EXIT;
}

void AlarmStoreIndex::updateAlarm(const TA_Base_Core::AlarmDetailCorbaDef& oldAlarm, TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	FUNCTION_ENTRY("updateAlarm");
	TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
	long oldAlarmPos = -1;
	long newAlarmPos = -1;
	
	try
	{
		
		// Since the oldAlarm is only a copy of the alarm data before the update,
		// the pointer of AlarmDetailCorbaDef saved in this index is newAlarm,
		// so the details of the alarm might have been modified and the AlarmIndexVector is not correctly sorted.
		// In order to use binary search to find the alarm in the AlarmIndexVector, we recover the details of the alarm 
		// first, and update the details after we find the alarm in the AlarmIndexVector.
		
		// Here we only back up the details that might be modified during the alarm update.
		// If any other details might be updated during an alarm update, we should add codes to back up the details here.
		
		backupAlarmDetails(oldAlarm, newAlarm, alarmUpdate);
		
		// find the old alarm in the display index and time index and delete it.
		oldAlarmPos = findAlarmInVectorList(newAlarm);
		if( oldAlarmPos != -1 )
		{
			AlarmIndexVector::iterator itr = m_alarmIndexVector.begin()+oldAlarmPos;
			m_alarmIndexVector.erase(itr);
			
			long timeIndexPos = findAlarmInTimeIndexVectorList(newAlarm);
			if( timeIndexPos == -1)
			{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The alarm is found in the display index vector but not found in the time index vector, there must be something missing");
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "updateAlarm: Failed to update alarm %s in alarm store index %d", newAlarm.alarmID.in(), m_indexID);
					FUNCTION_EXIT;
					return;
			}
			itr = m_alarmTimeIndexVector.begin() + timeIndexPos;
			m_alarmTimeIndexVector.erase(itr);
			
				std::string ackBy(newAlarm.alarmAcknowledgedBy.in());
				if( ackBy.length()==0)
			{
				--m_notAckCount;
				removeAlarmSeverity(newAlarm);
			}
				--m_totalCount;
	}

		updateAlarmDetails( newAlarm, alarmUpdate);
		
		if( m_filterSet.passesFilters(newAlarm))
		{
			newAlarmPos = findInsertPositionByBinaryChop( newAlarm, m_alarmIndexVector, m_sortSet);
			AlarmIndexVector::iterator itr = m_alarmIndexVector.begin()+newAlarmPos;
			m_alarmIndexVector.insert(itr, &newAlarm);
			
			if( false == m_isUsingSourceTime)
			{
				itr = std::upper_bound( m_alarmTimeIndexVector.begin(), m_alarmTimeIndexVector.end(), &newAlarm, compare_alarmdisplaytime());
			}
			else
			{
				itr = std::upper_bound( m_alarmTimeIndexVector.begin(), m_alarmTimeIndexVector.end(), &newAlarm, compare_alarmsourcetime());
			}
			
			m_alarmTimeIndexVector.insert(itr, &newAlarm);
			
				std::string newAlarmAckBy(newAlarm.alarmAcknowledgedBy.in());
				if( newAlarmAckBy.length()==0)
				{
					++m_notAckCount;
					addAlarmSeverity(newAlarm);
				}
				++m_totalCount;
	}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm: old position is %ld and new position is %d", oldAlarmPos, newAlarmPos);
		
		// tell all client applications
		if( oldAlarmPos == -1 && newAlarmPos != -1 )
		{
			// We can not find the old alarm in the alarm index vectors
			// just add the new alarm.
				if( newAlarmPos < MAX_DISPLAY_ALARMS)
				{
					for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, newAlarmPos,
							std::string(newAlarm.alarmID.in() ));
					}
				}
			
		}
		else if ( oldAlarmPos >= MAX_DISPLAY_ALARMS && newAlarmPos != -1)
		{
			// The old alarm is not in the display range
			
			if( newAlarmPos < MAX_DISPLAY_ALARMS)
			{
				// Just add the new alarm to the display
				
				for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{						
					m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, newAlarmPos,
						std::string(newAlarm.alarmID.in() ));
				}
			}
			
			// else do nothing
		}
		else
		{
			// The old alarm is in the display range
			
			if( newAlarmPos >= MAX_DISPLAY_ALARMS || newAlarmPos == -1 )
			{
				// The new alarm is not in the display range
				// Just remove the index
				for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{						
					m_callback.indexDeleted( m_callbackIDList[iClient],m_indexID, oldAlarmPos,
						std::string(newAlarm.alarmID.in() ));
				}
			}
			else
			{
				// The new alarm is also in the display range
				for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{						
					m_callback.indexMoved( m_callbackIDList[iClient],m_indexID, oldAlarmPos, newAlarmPos,
						std::string(newAlarm.alarmID.in() ));
				}
			}
		}
		
	}
	catch (const TA_Base_Core::TransactiveException& ex)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"updateAlarm: Failed to update alarm %s in alarm store index %d. Caught TransactiveException: %s", 
			newAlarm.alarmID.in(), m_indexID, ex.what());
			//TA_ASSERT(false, "Failed to update alarm into alarm store index");
	}
	catch( const CORBA::Exception& ex)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
			"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
			"updateAlarm: Failed to update alarm %s in alarm store index %d. Caught Corba Exception",
			newAlarm.alarmID.in(), m_indexID);
			//TA_ASSERT(false, "Failed to update alarm into alarm store index");
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "updateAlarm: Failed to update alarm %s in alarm store index %d", newAlarm.alarmID.in(), m_indexID);
			//TA_ASSERT(false, "Failed to update alarm into alarm store index");
	}
	
	FUNCTION_EXIT;
}


long AlarmStoreIndex::findAlarmInVectorList (const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("findAlarmInVectorList");
	
	long returnPos = -1;
	std::string alarmID = std::string(alarm.alarmID.in() );
	
	// Since the display list is not sorted in order of alarm ID, binary search cannot work properly
	// However we can use the rough insert pos to know the start point for search and minimise search time
	if (m_alarmIndexVector.size() < 1) return -1; // no alarm in list return not found
	
	unsigned long startPos = 0;
	unsigned long endPos = m_alarmIndexVector.size()-1;
	unsigned long lastPos = 0;
	unsigned long curPos = 0; // break the loop when curPos is -1 or equal to m_alarmDisplayVector's size
	IAlarmSort::ECompareResult lastResult = IAlarmSort::SORT_EQUAL;
	
	while (true)
	{
		//Find mid-point to start the search
		curPos = startPos + ( (endPos-startPos)/2 );
		
		// See if we need to be before or after the midpoint
		TA_ASSERT(curPos>=0,"Invalid insertion point (<0)");
		TA_ASSERT(curPos<m_alarmIndexVector.size(),"Invalid insertion point (>size of list)");
		
		// Check whether the source time is the same
		const TA_Base_Core::AlarmDetailCorbaDef* retAlarm = m_alarmIndexVector[curPos];
		std::string retID = std::string(retAlarm->alarmID.in());
		IAlarmSort::ECompareResult compare = m_sortSet.compareAlarms(alarm, *retAlarm);
		
		if (compare==IAlarmSort::SORT_BEFORE)
		{
			lastResult = compare;
			lastPos = curPos;
			
			if (curPos==startPos || startPos==endPos) break;
			// Must be between start and midpoint
			endPos = curPos-1;
		}
		else if (compare==IAlarmSort::SORT_AFTER)
		{
			lastResult = compare;
			lastPos = curPos;
			
			if (curPos==endPos || startPos==endPos) break;
			// Must be between midpoint and end
			startPos = curPos+1;
		}
		else // SORT_EQUAL
		{
			// Match found. Check the id
			if (retID.compare(alarmID) == 0) // it is a strike!
			{
				returnPos = curPos;
				break;
			}
			else
			{
				// Since there are some EQUAL results, there are several items that may
				// be skipped by binary search. 
				// Note that the index is not sorted in alarmID order
				// It is end of search, and the EQUAL can't find the alarm
				// No choice use linear search
				
				// Should not use linear search to search the whole vector now.
				// Since the index vector is sorted, the alarm we are looking for must be some place nearby.
				
				if( curPos > 0 )
				{
					unsigned long backwardPos = curPos - 1;
					
					
					while( backwardPos >= 0 )
					{
						retAlarm = m_alarmIndexVector[backwardPos];
						retID = std::string(retAlarm->alarmID.in());
						compare = m_sortSet.compareAlarms(alarm, *retAlarm);
						if (retID.compare(alarmID) == 0 && compare == IAlarmSort::SORT_EQUAL) 
						{
							returnPos = backwardPos;
							break;
						}
						else if( compare != IAlarmSort::SORT_EQUAL)
						{
							break;
						}
						
						if( backwardPos == 0)
						{
							break;
						}
						else
						{
							--backwardPos;
						}
					}
				}
				
				unsigned long forwardPos = curPos + 1;
				
				if( returnPos == -1) // if returnPos != -1, means already found.
				{
					while( forwardPos < m_alarmIndexVector.size())
					{
						retAlarm = m_alarmIndexVector[forwardPos];
						retID = std::string(retAlarm->alarmID.in());
						compare = m_sortSet.compareAlarms(alarm, *retAlarm);
						if (retID.compare(alarmID) == 0 && compare == IAlarmSort::SORT_EQUAL) 
						{
							returnPos = forwardPos;
							break;
						}
						else if( compare != IAlarmSort::SORT_EQUAL)
						{
							break;
						}
						++forwardPos;
					}
				}
				break;
			}
		}
	} // end of while loop
	
	TA_ASSERT( returnPos<(long)(m_alarmIndexVector.size()), "The alarm position found in the alarm index can not be out of the index vector size");
	
	if( returnPos == -1)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "The alarm %s is not found in the alarm index vector of alarm index %d", alarm.alarmID.in(), m_indexID);
	}
	
	FUNCTION_EXIT;
	return returnPos; // return either -1 ie not found or the exact position of the alarm
}

long AlarmStoreIndex::findAlarmInTimeIndexVectorList (const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("findAlarmInYimeIndexVectorList");
	
	int returnPos = -1;
	
	std::pair< AlarmIndexVector::iterator, AlarmIndexVector::iterator > result;
	
	if(false == m_isUsingSourceTime)
	{
		result = std::equal_range(m_alarmTimeIndexVector.begin(), m_alarmTimeIndexVector.end(), &alarm, compare_alarmdisplaytime());
	}
	else
	{
		result = std::equal_range(m_alarmTimeIndexVector.begin(), m_alarmTimeIndexVector.end(), &alarm, compare_alarmsourcetime());
	}
	
	AlarmIndexVector::iterator itr;
	
	std::string toFindAlarmId(alarm.alarmID.in());
	
	for( itr= result.first; itr != result.second; itr++ )
	{
		std::string findId( (*itr)->alarmID.in());
		if( findId.compare(toFindAlarmId) == 0 )
		{
			returnPos = itr - m_alarmTimeIndexVector.begin();
			break;
		}
	}
	
	TA_ASSERT(returnPos<(long)(m_alarmTimeIndexVector.size()), "The alarm position found in the alarm time index can not be out of the index vector size");
	
	
	if( returnPos == -1)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The alarm %s is not found in the alarm time index vector of alarm index %d", alarm.alarmID.in(), m_indexID);
	}
	
	FUNCTION_EXIT;
	return returnPos; // return either -1 ie not found or the exact position of the alarm
}



unsigned long AlarmStoreIndex::findInsertPositionByBinaryChop(const TA_Base_Core::AlarmDetailCorbaDef& alarm, std::vector<const TA_Base_Core::AlarmDetailCorbaDef*>& vectorList, AlarmSortingSet& sortSet)
{
	FUNCTION_ENTRY("findInsertPositionByBinaryChop");
	
	// If no alarms simply add it.
	if (vectorList.size()<1)
	{
		FUNCTION_EXIT;
		return 0;
	}
	
	unsigned long startPos  = 0;
	unsigned long endPos    = vectorList.size()-1; 
	unsigned long insertPos = 0;
	
	while (true)
	{
		// Find the mid point to start our compare
		insertPos = startPos + ((endPos - startPos)/2);
		
		// See if we need to be before or after the midpoint
		TA_ASSERT(insertPos>=0,"Invalid insertion point (<0)");
		TA_ASSERT(insertPos<vectorList.size(),"Invalid insertion point (>size of list)");
		
		//AlarmIndexVector::iterator itr = m_alarmIndexVector.begin() + insertPos;
		//const TA_Base_Core::AlarmDetailCorbaDef* retAlarm = *itr;
		IAlarmSort::ECompareResult compare = sortSet.compareAlarms(alarm, *vectorList[insertPos]);
		
		if (compare==IAlarmSort::SORT_BEFORE)
		{
			// If the is the last point to check then finish
			if (insertPos==startPos) break;
			if (startPos==endPos) break;
			
			// Must be between start and midpoint
			endPos = insertPos-1;
		}
		else if (compare==IAlarmSort::SORT_AFTER)
		{
			// If the is the last point to check then finish
			if (startPos==endPos)
			{
				++insertPos;
				break;
			}
			if (insertPos==endPos) break;
			
			// Must be between midpoint and end
			startPos = insertPos+1;
		}
		else // SORT_EQUAL
		{
			// Match found, insert before this one
			break;
		}
	} // while
	
	TA_ASSERT(insertPos>=0,"Invalid insertion point (<0)");
	TA_ASSERT(insertPos<=vectorList.size(),"Invalid insertion point (>size of list)");
	
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "findInsertPositionByBinaryChop: insertPos of alarm %s in display list is %lu", alarm.alarmID.in(), insertPos);
	FUNCTION_EXIT;
	return insertPos;
}


/*
* Update the alarm severity map based on the addition of the specified alarm.
*/
void AlarmStoreIndex::addAlarmSeverity( const TA_Base_Core::AlarmDetailCorbaDef& alarm )
{
	FUNCTION_ENTRY("addAlarmSeverity");
	
		if( m_isSeverityProcessed)
		{
		
			JobQueueEntry job;
			
			job.isToAddSeverity = true;
			job.entityKey		= alarm.assocEntityKey;
			job.locationKey		= alarm.locationKey;
			job.messageTypeKey	= alarm.messageTypeKey;
			job.subsystemKey	= alarm.subsystemKey;
			job.alarmId			= std::string(alarm.alarmID.in());
			job.severity		= alarm.alarmSeverity;
			
			TA_Base_Core::ThreadGuard  queueGuard(m_jobQueueLock);
			m_jobQueue.push_back(job);
			m_jobQueueSemaphore.post();
		}
		
		FUNCTION_EXIT;
	}

	/*
	* Update the alarm severity map based on the removal of the specified alarm.
	*/
	void AlarmStoreIndex::removeAlarmSeverity( const TA_Base_Core::AlarmDetailCorbaDef& alarm )
	{
		FUNCTION_ENTRY("removeAlarmSeverity");
		
		if(m_isSeverityProcessed)
		{	
			JobQueueEntry job;
			
			job.isToAddSeverity = false;
			job.entityKey		= alarm.assocEntityKey;
			job.locationKey		= alarm.locationKey;
			job.messageTypeKey	= alarm.messageTypeKey;
			job.subsystemKey	= alarm.subsystemKey;
			job.alarmId			= std::string(alarm.alarmID.in());
			job.severity		= alarm.alarmSeverity;
			
			TA_Base_Core::ThreadGuard  queueGuard(m_jobQueueLock);
			m_jobQueue.push_back(job);
			m_jobQueueSemaphore.post();
		}
	
	FUNCTION_EXIT;
}

unsigned long AlarmStoreIndex::getLatestSourceTime()
{
	FUNCTION_ENTRY("getLatestSourceTime");
	TA_Base_Core::ThreadGuard guard(m_indexLock);
	
	unsigned long sourcetime(0);
	
	const TA_Base_Core::AlarmDetailCorbaDef* alarmDetail;
	
	if( false == m_alarmTimeIndexVector.empty())
	{
		alarmDetail = m_alarmTimeIndexVector.front();
		
		if( NULL != alarmDetail )
		{
			sourcetime = alarmDetail->displayTime.time;
		}
	}
	
	FUNCTION_EXIT;
	return sourcetime;
}


unsigned long AlarmStoreIndex::getEarliestSourceTime()
{
	FUNCTION_ENTRY("getEarliestSourceTime");
	TA_Base_Core::ThreadGuard guard(m_indexLock);
	
	unsigned long sourcetime(0);
	
	const TA_Base_Core::AlarmDetailCorbaDef* alarmDetail = NULL;
	
	if( m_alarmTimeIndexVector.size() > MAX_DISPLAY_ALARMS )
	{
		alarmDetail = m_alarmTimeIndexVector[MAX_DISPLAY_ALARMS-1];
	}
	else if( false == m_alarmTimeIndexVector.empty())
	{
		alarmDetail = m_alarmTimeIndexVector.back();
	}
	
	if( NULL != alarmDetail )
	{
		sourcetime = alarmDetail->displayTime.time;
	}
	
	FUNCTION_EXIT;
	return sourcetime;
}

unsigned long AlarmStoreIndex::getHighestSeverityAlarm()
{
	FUNCTION_ENTRY("getHighestSeverityAlarm");
	
		if( false == m_isSeverityProcessed)
		{
			FUNCTION_EXIT;
			return 0;
		}

	TA_Base_Core::ThreadGuard guardSeverity(m_severityMapLock);
	
	unsigned long severity=0;  // no alarms, no severity!
	bool          isFirst = true;
	
	AlarmSeverityMap::const_iterator   mapIT = m_alarmSeverityMap.begin();
	while (mapIT!=m_alarmSeverityMap.end())
	{
		if ( 0<mapIT->second.size() )
		{
			if ( isFirst||( mapIT->first<severity ) )
			{
				severity = mapIT->first;
				isFirst = false;
			}
		}
		++mapIT;
	}
	FUNCTION_EXIT;
	return severity;
}

/**
* Add another callback for this index
* @param callbackID    ID of the callback to be added.
* 
*/
void AlarmStoreIndex::addCallback(long callbackID)
{
	FUNCTION_ENTRY("addCallback");
	TA_Base_Core::ThreadGuard guard(m_callbackIDListLock);
	
	m_callbackIDList.push_back(callbackID);
	
	FUNCTION_EXIT;
}

void AlarmStoreIndex::addAllAlarmsToCallback(long callbackID)
{
	FUNCTION_ENTRY("addAllAlarmsToCallback");
	TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
	TA_Base_Core::ThreadGuard guard(m_callbackIDListLock);
	
	// Send the initial clear
	m_callback.indexCleared(callbackID,m_indexID);
	
	// Notify this new call back of all the alarms in the index
	unsigned long pos = 0;
	
	for( AlarmIndexVector::iterator itr = m_alarmIndexVector.begin(); itr != m_alarmIndexVector.end(); itr++)
	{
		m_callback.indexInserted(callbackID, m_indexID, pos, (*itr)->alarmID.in());
		++pos;
	}
	
	FUNCTION_EXIT;
}



/**
* Remove a callback from this index
* @param callbackID    ID of callback to be removed
* 
*/
void AlarmStoreIndex::removeCallback(long callbackID)
{
	FUNCTION_ENTRY("removeCallback");
	
	TA_Base_Core::ThreadGuard guard(m_callbackIDListLock);
	std::vector<long>::iterator it = m_callbackIDList.begin();
	while (it!=m_callbackIDList.end())
	{
		if (*it==callbackID)
		{
			m_callbackIDList.erase(it);
			break;
		}
		++it;
	}
	FUNCTION_EXIT;
}

/**
* Determined if index is in use.  This will be true if any callbacks are defined.
*/
bool AlarmStoreIndex::isUsed() 
{
	FUNCTION_ENTRY("isUsed");
	FUNCTION_EXIT;
	TA_Base_Core::ThreadGuard guard(m_callbackIDListLock);
	return m_callbackIDList.size()>0;
}

/**
* Determine if callback id is in this index.
*/
bool AlarmStoreIndex::isCallbackIDPresent(long callbackID) 
{
	FUNCTION_ENTRY("isCallbackIDPresent");
	TA_Base_Core::ThreadGuard guard(m_callbackIDListLock);
	std::vector<long>::const_iterator it = m_callbackIDList.begin();
	while (it!=m_callbackIDList.end())
	{
		if (*it==callbackID)
		{
			FUNCTION_EXIT;
			return true;
		}
		++it;
	}
	FUNCTION_EXIT;
	return false;
}

bool AlarmStoreIndex::isSameFilterAndSort(const AlarmFilterSet& filterSet, const AlarmSortingSet& sortSet) const
{
	FUNCTION_ENTRY("isSameFilterAndSort");
	FUNCTION_EXIT;
	return ((m_filterSet==filterSet)&&(m_sortSet==sortSet));
}

	bool AlarmStoreIndex::checkAckPermission(unsigned long messageTypeKey, unsigned long locationKey, unsigned long subsystemKey, unsigned long assocEntityKey) const
{
	FUNCTION_ENTRY("checkAckPermission");
	
    if (s_sessionId == "")
    {
        // No session ID set. Alarm sound should be played regardless of rights.
        FUNCTION_EXIT;
        return true;
    }
    else
    {
		
			TA_Base_Bus::AccessControlledAction action = TA_Base_Bus::AlarmActions::getAlarmAction( messageTypeKey, 
			TA_Base_Bus::AlarmActions::ACK_ALARM);
		
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;
		
        try
        {
			bool canAck = m_rightsLibrary->isActionPermittedOnResourceAndLocSub(std::string(s_sessionId), 
				assocEntityKey, locationKey, subsystemKey, action, reason, decisionModule);
			
            FUNCTION_EXIT;
            return canAck;
        }
        catch(...)
        {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "isActionPermittedOnResource threw an exception for alarm type %u and session ID %s",
					messageTypeKey, s_sessionId.c_str());
            // Rights library encountered an error. Just return true.
            FUNCTION_EXIT;
            return true;
        }
    }
}

void AlarmStoreIndex::backupAlarmDetails( const TA_Base_Core::AlarmDetailCorbaDef& backupAlarm, TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	FUNCTION_ENTRY("backupAlarmDetails");
	if (alarmUpdate.alarmHasBeenAcknowledged)
	{
		newAlarm.alarmAcknowledgedBy = backupAlarm.alarmAcknowledgedBy;
		newAlarm.ackTime = backupAlarm.ackTime;
	}
	
	if (alarmUpdate.alarmHasBeenClosed)
	{
		newAlarm.state = backupAlarm.state;
		newAlarm.closeTime = backupAlarm.closeTime;
		newAlarm.displayTime.time = backupAlarm.displayTime.time;
		newAlarm.displayTime.millitm = backupAlarm.displayTime.millitm;

		newAlarm.alarmValue = backupAlarm.alarmValue;
	}
	
	if (alarmUpdate.alarmHasBeenOpened)
	{
		newAlarm.state = backupAlarm.state;
		newAlarm.closeTime = backupAlarm.closeTime;
	}
	
	if (alarmUpdate.commentsWereAdded)
	{
		newAlarm.alarmComments = backupAlarm.alarmComments;  // Note: Allen assures me that no further str mngmnt is needed!
	}
	
	if (alarmUpdate.decisionSupportStateHasChanged)
	{
		newAlarm.decisionSupportState = backupAlarm.decisionSupportState;
	}
	
	if (alarmUpdate.mmsStateHasChanged)
	{
		newAlarm.mmsState = backupAlarm.mmsState;
	}
	FUNCTION_EXIT;
}

void AlarmStoreIndex::updateAlarmDetails( TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	FUNCTION_ENTRY("updateAlarmDetails");
	if (alarmUpdate.alarmHasBeenAcknowledged)
	{
		newAlarm.alarmAcknowledgedBy = alarmUpdate.alarmAcknowledgedBy;
		newAlarm.ackTime = alarmUpdate.ackTime;
	}
	
	if (alarmUpdate.alarmHasBeenClosed)
	{
		newAlarm.state = TA_Base_Core::AlarmClosed;
		newAlarm.closeTime = alarmUpdate.closeTime;
		// update the display time to reflect the closed time
		newAlarm.displayTime.time = alarmUpdate.closeTime;
		newAlarm.displayTime.millitm = 0;

		newAlarm.alarmValue = alarmUpdate.closeValue;
	}
	
	if (alarmUpdate.alarmHasBeenOpened)
	{
		newAlarm.state = TA_Base_Core::AlarmOpen;
		newAlarm.closeTime = 0;
	}
	
	if (alarmUpdate.commentsWereAdded)
	{
		newAlarm.alarmComments = alarmUpdate.alarmComments;  // Note: Allen assures me that no further str mngmnt is needed!
	}
	
	if (alarmUpdate.decisionSupportStateHasChanged)
	{
		newAlarm.decisionSupportState = alarmUpdate.decisionSupportState;
	}
	
	if (alarmUpdate.mmsStateHasChanged)
	{
		newAlarm.mmsState = alarmUpdate.mmsState;
	}
	
	FUNCTION_EXIT;
}

/**
* This is the work method for the thread.  It will process the checking ack-permission and severity
* to avoid a long delay on the user corba call
*/
void AlarmStoreIndex::run()
{
	FUNCTION_ENTRY("run");
	while (!m_terminate)
	{
			try
			{
				// Wait to be notified of a job to do
				m_jobQueueSemaphore.wait();
				if (m_terminate) break;
				
				// Get job from the queue
				JobQueueEntry    job;
				{ // Scope for lock
					TA_Base_Core::ThreadGuard  guard(m_jobQueueLock);
					if(m_jobQueue.size()==0)
					{
						//TA_ASSERT(m_jobQueue.size()>0,"No job on work queue.");
						continue;
					}
					job = m_jobQueue.at(0);  // copy the job
					m_jobQueue.erase(m_jobQueue.begin());
				}
				
				// Process the job
				{ // Nest for lock scope
					if( job.isToAddSeverity)
					{
						if ( checkAckPermission( job.messageTypeKey, job.locationKey, job.subsystemKey, job.entityKey))
						{
							// Protect from updating
							TA_Base_Core::ThreadGuard guard(m_severityMapLock);
							
							m_alarmSeverityMap[job.severity].insert( job.alarmId );
						}
						
					}
					else
					{
						// Protect from updating
						TA_Base_Core::ThreadGuard guard(m_severityMapLock);
						
						AlarmSeverityMap::iterator severityIt = m_alarmSeverityMap.find( job.severity );
						if ( m_alarmSeverityMap.end() != severityIt )
						{
							// If the alarm wasn't used when calculating the severity count just ignore it.
							AlarmSet::iterator alarmIt = severityIt->second.find( job.alarmId );
							if ( severityIt->second.end() != alarmIt )
							{
								// Otherwise it needs to be removed from the set.
								severityIt->second.erase( alarmIt );
							}
						}
					}
				}
			}
			catch (const TA_Base_Core::TransactiveException& ex)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Caught TransactiveException: %s in the run() of AlarmStoreIndex", 
					ex.what());
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
					"Caught Corba Exception in the run() of AlarmStoreIndex");
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception in the run() of AlarmStoreIndex");
			}
					
		} // end of while loop
	
	m_terminate = false;
	FUNCTION_EXIT;
}


/**
* The terminate() method should cause run() to return. eg: CorbaUtil::
* GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
* to call this method directly - call terminateAndWait() instead
*/
void AlarmStoreIndex::terminate()
{
	FUNCTION_ENTRY("terminate");
	m_terminate = true;
	m_jobQueueSemaphore.post();
	
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Terminate called. Alarm store index is stop running.");
	
	FUNCTION_EXIT;
}
}