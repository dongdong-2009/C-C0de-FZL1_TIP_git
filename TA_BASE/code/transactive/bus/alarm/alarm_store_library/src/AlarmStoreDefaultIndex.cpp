/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreDefaultIndex.cpp $
* @author:  Andy Parker
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* This class implements the alarm store index.
*
*/

///////////////////////////////////////////////////////////
//  AlarmStoreDefaultIndex.cpp
//  Implementation of the Class AlarmStoreDefaultIndex
//  Created on:      24-Jan-2004 04:18:02 PM
///////////////////////////////////////////////////////////

#include <algorithm>

#include "AlarmStoreDefaultIndex.h"
#include "AlarmFilterSet.h"
#include "AlarmSortingSet.h"
#include "IAlarmStoreIndexChanges.h"
#include "core/utilities/src/TAAssert.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"
#pragma warning( disable : 4503 )


namespace TA_Base_Bus
{
	
	
	std::string AlarmStoreDefaultIndex::s_sessionId = "";
	
	/**
	* Max number of displayable alarms which is constant
	* According to SRS and TD8629 the max number is 30000
	*/
	const unsigned long AlarmStoreDefaultIndex::MAX_DISPLAY_ALARMS = 30000;
	
	/**
	* Constructor.  The constructor will create a new default alarm store
	* @param indexID    Unique identifier for this index
	* @param callbackID    The callback to be associated with this index
	* @param filterSet    The filters to be used for this index
	* @param sortSet    The sorting to be used for this index
	* 
	*/
	AlarmStoreDefaultIndex::AlarmStoreDefaultIndex(long indexID, IAlarmStoreIndexChanges& callback, long callbackID,
		AlarmFilterSet& filterSet, TA_Base_Bus::RightsLibrary* rightsLibrary, bool severityProcessed)
		: m_callback(callback),
		m_indexID(indexID),
		m_filterSet(filterSet),
		m_openCount(0),
		m_normalCount(0),
		m_ackedCount(0),
		m_highestSeverity(0),
		m_lowestSeverity(0),
		m_terminate(false),
		m_rightsLibrary(rightsLibrary),
		m_maxDisplayIsReached(false),
		m_isSeverityProcessed(severityProcessed)
	{
		FUNCTION_ENTRY("Constructor");
		
		// reserve the size of the vector to prevent any reallocation problem
		
		addCallback(callbackID);
		addAllAlarmsToCallback(callbackID);
		
		m_isUsingSourceTime = TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_USESOURCETIME.c_str());
		
		start();
	}
	
	AlarmStoreDefaultIndex::~AlarmStoreDefaultIndex()
	{
		FUNCTION_ENTRY("Destructor");
		// Alarm index list will clear itself
		// CallbackID list will clear itself
		// m_alarmSeverityMap will clear itself
		
		TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
		TA_Base_Core::ThreadGuard guardSeverity(m_severityMapLock);
		TA_Base_Core::ThreadGuard guardJob(m_jobQueueLock);
		
		terminateAndWait();
		
		
		AlarmIndexMap::iterator itr;
		
		for( itr = m_openAlarmMap.begin(); itr != m_openAlarmMap.end(); itr++)
		{
			if( itr->second.empty() == false )
			{	
				itr->second.clear();
			}
		}
		m_openAlarmMap.clear();
		
		for( itr = m_normalAlarmMap.begin(); itr != m_normalAlarmMap.end(); itr++)
		{
			if( itr->second.empty() == false )
			{
				itr->second.clear();
			}
		}
		m_normalAlarmMap.clear();
		
		for( itr = m_ackedAlarmMap.begin(); itr != m_ackedAlarmMap.end(); itr++)
		{
			if( itr->second.empty() == false )
			{
				itr->second.clear();
			}
		}
		m_ackedAlarmMap.clear();
		
		// Clear all the maps and vectors
		if (m_alarmSeverityMap.empty() == false)
		{
			m_alarmSeverityMap.clear();
		}
		if (m_callbackIDList.empty() == false)
		{
			m_callbackIDList.clear();
		}
		
		m_jobQueue.clear();
		
		
		FUNCTION_EXIT;
	}
	
	
	/**
	* This method will clear all the entries from the index.
	*/
	void AlarmStoreDefaultIndex::clearAlarms()
	{
		FUNCTION_ENTRY("clearAlarms");
		
		TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
		TA_Base_Core::ThreadGuard guardSeverity(m_severityMapLock);
		TA_Base_Core::ThreadGuard guardJob(m_jobQueueLock);
		
		AlarmIndexMap::iterator itrMap;
		
		for( itrMap = m_openAlarmMap.begin(); itrMap !=m_openAlarmMap.end(); itrMap++)
		{
			if( itrMap->second.empty() == false )
			{
				itrMap->second.clear();
			}
		}
		m_openAlarmMap.clear();
		
		for( itrMap = m_normalAlarmMap.begin(); itrMap !=m_normalAlarmMap.end(); itrMap++)
		{
			if( itrMap->second.empty() == false )
			{
				itrMap->second.clear();
			}
		}
		m_normalAlarmMap.clear();
		
		for( itrMap = m_ackedAlarmMap.begin(); itrMap !=m_ackedAlarmMap.end(); itrMap++)
		{
			if( itrMap->second.empty() == false )
			{
				itrMap->second.clear();
			}
		}
		m_ackedAlarmMap.clear();
		
		m_openCount = 0; 
		m_normalCount = 0;
		m_ackedCount = 0;
		
		m_highestSeverity = 0;
		m_lowestSeverity = 0;
		
		m_alarmSeverityMap.clear();  // reset the severity counts
		
		m_maxDisplayIsReached = false;
		
		// Tell all the clients
		for (unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
		{
			m_callback.indexCleared(m_callbackIDList[iClient],m_indexID);
		}
		
		m_jobQueue.clear();
		
		FUNCTION_EXIT;
	}
	
	
	/**
	* Add new alarm to the index
	* @param alarm
	* 
	*/
	void AlarmStoreDefaultIndex::addAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
	{
		FUNCTION_ENTRY("addAlarm");
		
		TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
		TA_Base_Core::ThreadGuard guardCallback(m_callbackIDListLock);
		
		if( !m_filterSet.passesFilters( alarm))
		{
			// do nothing is can not pass the filters
			FUNCTION_EXIT;
			return;
		}
		
		try 
		{
			
			TA_ASSERT( alarm.alarmSeverity > 0, " The severity of alarm must be positive."  );
			
			// The default filter set is empty
			std::string ackBy(alarm.alarmAcknowledgedBy.in());
			if( alarm.state == TA_Base_Core::AlarmOpen && ackBy.length()==0 )
			{
				// Open alarm, add to m_openAlarmMap
				
				// check if the vector of the alarms with the same severity level exists
				
				AlarmIndexMap::iterator itrMap;
				
				// The position of the alarm to be displayed in client application.
				unsigned long displayPos;
				
				itrMap = m_openAlarmMap.find( alarm.alarmSeverity );
				
				if( itrMap == m_openAlarmMap.end())
				{
					// create a new index vector
					AlarmIndexVector openAlarmIndex;
					
					openAlarmIndex.push_back( &alarm); 
					
					// Deal with the severity and source time
					
					m_openAlarmMap.insert( std::pair< unsigned long, AlarmIndexVector >( alarm.alarmSeverity, openAlarmIndex) );
					
					if( alarm.alarmSeverity < m_highestSeverity || m_highestSeverity == 0 )
					{
						m_highestSeverity = alarm.alarmSeverity;
					}
					
					if( m_lowestSeverity < alarm.alarmSeverity )
					{
						m_lowestSeverity = alarm.alarmSeverity;
					}
					
					m_openCount++;
					
					displayPos = 0;
					
				}
				else
				{
					AlarmIndexVector::iterator itrVec;
					
					if(m_isUsingSourceTime)
					{
						itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &alarm, compare_alarmdisplaytime());
					}
					else
					{
						itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &alarm, compare_alarmsourcetime());
					}
					
					displayPos = itrVec - itrMap->second.begin();
					
					itrMap->second.insert( itrVec, &alarm);
					
					m_openCount++;				
					
				}
				
				//check if the alarm count exceed maximum alarm display count
				
				addAlarmSeverity( alarm);
				
				for( unsigned long i = m_highestSeverity; i < alarm.alarmSeverity; i++ )
				{
					itrMap = m_openAlarmMap.find(i);
					
					if( itrMap!= m_openAlarmMap.end() )
					{
						displayPos += itrMap->second.size();
					}
				}
				
				if( displayPos < MAX_DISPLAY_ALARMS )
				{
					// need to update the display of the client applications
					
					if( m_maxDisplayIsReached )
					{
						// remove the alarm in the bottom of the display index
						unsigned int iClient = 0;
						for (iClient = 0;iClient<m_callbackIDList.size();++iClient)
						{						
							m_callback.indexDeleted( m_callbackIDList[iClient],m_indexID, MAX_DISPLAY_ALARMS-1,
								findAlarmIdAtPosition(MAX_DISPLAY_ALARMS-1));
						}				
						
						// add the new alarm to the display index
						
						for ( iClient = 0;iClient<m_callbackIDList.size();++iClient)
						{						
							m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, displayPos,
								std::string(alarm.alarmID.in() ));
						}
					}
					else
					{
						// add the new alarm to the display index
						
						for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
						{						
							m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, displayPos,
								std::string(alarm.alarmID.in() ));
							
							if( (m_openCount+m_normalCount+m_ackedCount) >= MAX_DISPLAY_ALARMS )
							{
								m_maxDisplayIsReached = true;
							}
						}					
					}
				}
				
		}
		else if( ackBy.length()==0 )
		{			
			// Normalized and not_acked alarm, add to m_normalAlarmMap
			
			// check if the vector of the alarms with the same severity level exists
			
			AlarmIndexMap::iterator itrMap;
			
			// The position of the alarm to be displayed in client application.
			unsigned long displayPos;
			
			
			itrMap = m_normalAlarmMap.find( alarm.alarmSeverity );
			
			if( itrMap == m_normalAlarmMap.end())
			{
				// create a new index vector
				AlarmIndexVector normalAlarmIndex;
				
				normalAlarmIndex.push_back( &alarm); 
				
				displayPos = 0;
				
				// To Do deal with the severity and source time
				
				m_normalAlarmMap.insert( std::pair< unsigned long, AlarmIndexVector >( alarm.alarmSeverity, normalAlarmIndex) );
				
				if( alarm.alarmSeverity < m_highestSeverity || m_highestSeverity == 0)
				{
					m_highestSeverity = alarm.alarmSeverity;
				}
				
				if( m_lowestSeverity < alarm.alarmSeverity )
				{
					m_lowestSeverity = alarm.alarmSeverity;
				}
				
				m_normalCount++;
				
			}
			else
			{
				AlarmIndexVector::iterator itrVec;
				
				if(m_isUsingSourceTime)
				{
					itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &alarm, compare_alarmsourcetime());
				}
				else
				{
					itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &alarm, compare_alarmdisplaytime());
				}
				
				displayPos = itrVec - itrMap->second.begin();
				
				itrMap->second.insert( itrVec, &alarm);
				
				m_normalCount++;
				
				
			}
			
			// Check if the alarm count exceed maximum alarm display count
			
			addAlarmSeverity( alarm);
			
			displayPos += m_openCount; 
			
			for( unsigned long i = m_highestSeverity; i < alarm.alarmSeverity; i++ )
			{
				itrMap = m_normalAlarmMap.find(i);
				
				if( itrMap!= m_normalAlarmMap.end() )
				{
					displayPos += itrMap->second.size();
				}
			}
			
			if( displayPos < MAX_DISPLAY_ALARMS )
			{
				// need to update the display of the client applications
				
				unsigned int iClient = 0;
				if( m_maxDisplayIsReached )
				{
					// remove the alarm in the bottom of the display index
					
					for ( iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexDeleted( m_callbackIDList[iClient],m_indexID, MAX_DISPLAY_ALARMS-1,
							findAlarmIdAtPosition(MAX_DISPLAY_ALARMS-1));
					}				
					
					// add the new alarm to the display index
					
					for ( iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, displayPos,
							std::string(alarm.alarmID.in() ));
					}
				}
				else
				{
					// add the new alarm to the display index
					
					for ( iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, displayPos,
							std::string(alarm.alarmID.in() ));
						
						if( (m_openCount+m_normalCount+m_ackedCount) >= MAX_DISPLAY_ALARMS )
						{
							m_maxDisplayIsReached = true;
						}
					}					
				}
			}
		}
		else
		{
			// Acked alarm, add to m_ackedAlarmMap
			
			AlarmIndexMap::iterator itrMap = m_ackedAlarmMap.begin();
			
			// The position of the alarm to be displayed in client application.
			unsigned long displayPos;
			
			
			itrMap = m_ackedAlarmMap.find( alarm.alarmSeverity );
			
			if( itrMap == m_ackedAlarmMap.end())
			{
				// create a new index vector
				AlarmIndexVector ackedAlarmIndex;
				
				ackedAlarmIndex.push_back( &alarm); 
				
				m_ackedAlarmMap.insert( std::pair< unsigned long, AlarmIndexVector >( alarm.alarmSeverity, ackedAlarmIndex) );
				
				if( alarm.alarmSeverity < m_highestSeverity || m_highestSeverity ==0 )
				{
					m_highestSeverity = alarm.alarmSeverity;
				}
				
				if( m_lowestSeverity < alarm.alarmSeverity )
				{
					m_lowestSeverity = alarm.alarmSeverity;
				}
				
				displayPos = 0;
				
				m_ackedCount++;
				
			}
			else
			{
				AlarmIndexVector::iterator itrVec;
				
				if(m_isUsingSourceTime)
				{
					itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &alarm, compare_alarmsourcetime());
				}
				else
				{
					itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &alarm, compare_alarmdisplaytime());
				}
				
				displayPos = itrVec - itrMap->second.begin();
				
				itrMap->second.insert( itrVec, &alarm);
				
				m_ackedCount++;
				
			}
			
			displayPos += m_openCount+m_normalCount; 
			
			for( unsigned long i = m_highestSeverity; i < alarm.alarmSeverity; i++ )
			{
				itrMap = m_ackedAlarmMap.find(i);
				
				if( itrMap!= m_ackedAlarmMap.end() )
				{
					displayPos += itrMap->second.size();
				}
			}
			
			if( displayPos < MAX_DISPLAY_ALARMS )
			{
				// need to update the display of the client applications
				
				unsigned int iClient = 0;
				if( m_maxDisplayIsReached )
				{
					// remove the alarm in the bottom of the display index
					
					for ( iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexDeleted( m_callbackIDList[iClient],m_indexID, MAX_DISPLAY_ALARMS-1,
							findAlarmIdAtPosition(MAX_DISPLAY_ALARMS-1));
					}				
					
					// add the new alarm to the display index
					
					for ( iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, displayPos,
							std::string(alarm.alarmID.in() ));
					}
				}
				else
				{
					// add the new alarm to the display index
					
					for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{						
						m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, displayPos,
							std::string(alarm.alarmID.in() ));
						
						if( (m_openCount+m_normalCount+m_ackedCount) >= MAX_DISPLAY_ALARMS )
						{
							m_maxDisplayIsReached = true;
						}
					}					
				}
			}
		}
		
		
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "addAlarm: Failed to added alarm %s in an alarm store default index %d", alarm.alarmID.in(), m_indexID);		
	}
	
	FUNCTION_EXIT;
}

void AlarmStoreDefaultIndex::updateAlarm(const TA_Base_Core::AlarmDetailCorbaDef& oldAlarm, TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	FUNCTION_ENTRY("updateAlarm");
	
	TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
		TA_Base_Core::ThreadGuard guardCallback(m_callbackIDListLock);
	try
	{
		long oldPosition;
		long newPosition;
		
		AlarmIndexMap::iterator itrMap;
		AlarmIndexVector::iterator itrVec;
		
		// Since the oldAlarm is only a copy of the alarm data before the update,
		// the pointer of AlarmDetailCorbaDef saved in this index is newAlarm,
		// so the displayTime might have been modified and the AlarmIndexVector is not correctly sorted.
		// In order to use binary search to find the alarm in the AlarmIndexVector, we recover the displayTime 
		// first, and update the displayTime after we find the alarm in the AlarmIndexVector.
		
		backupAlarmDetails(oldAlarm, newAlarm, alarmUpdate);
		// find the old position of the alarm in the index vector 
		
		std::string ackBy(newAlarm.alarmAcknowledgedBy.in());
		if( newAlarm.state == TA_Base_Core::AlarmOpen && ackBy.length()==0 )
		{
			// the old alarm is an open and not_acked alarm.
			itrMap = m_openAlarmMap.find(newAlarm.alarmSeverity);
			
			if( itrMap == m_openAlarmMap.end() )
			{
				oldPosition = -1;
			}
			else
			{
				oldPosition = findAlarmInVectorList( newAlarm, itrMap->second );
				
				if( oldPosition != -1)
				{
					// The old alarm has been found in the alarm index vector.
					
					itrMap->second.erase( itrMap->second.begin()+oldPosition);
					if( itrMap->second.empty())
					{
						m_openAlarmMap.erase(newAlarm.alarmSeverity);
					}
					m_openCount--;
					
					removeAlarmSeverity( newAlarm);
					
					for( unsigned int i = m_highestSeverity; i< newAlarm.alarmSeverity; i++)
					{
						itrMap = m_openAlarmMap.find(i);
						
						if( itrMap != m_openAlarmMap.end() )
						{
							oldPosition += itrMap->second.size();
						}
					}
				}
			}
		}
		else if( ackBy.length()== 0)
		{
			// the old alarm is a normalized and not_acked alarm.
			itrMap = m_normalAlarmMap.find(newAlarm.alarmSeverity);
			
			if( itrMap == m_normalAlarmMap.end() )
			{
				oldPosition = -1;
			}
			else
			{
				oldPosition = findAlarmInVectorList( newAlarm, itrMap->second );
				if( oldPosition != -1)
				{
					// The old alarm has been found in the alarm index vector
					itrMap->second.erase( itrMap->second.begin()+oldPosition);
					if( itrMap->second.empty())
					{
						m_normalAlarmMap.erase(newAlarm.alarmSeverity);
					}
					
					removeAlarmSeverity( newAlarm);
					
					m_normalCount--;
					for( unsigned int i = m_highestSeverity; i< newAlarm.alarmSeverity; i++)
					{
						itrMap = m_normalAlarmMap.find(i);
						
						if( itrMap != m_normalAlarmMap.end() )
						{
							oldPosition += itrMap->second.size();
						}
					}
					oldPosition += m_openCount;
				}
			}
		}
		else
		{
			// the old alarm is an acked alarm.
			
			itrMap = m_ackedAlarmMap.find(newAlarm.alarmSeverity);
			
			if( itrMap == m_ackedAlarmMap.end() )
			{
				oldPosition = -1;
			}
			else
			{
				oldPosition = findAlarmInVectorList( newAlarm, itrMap->second );
				if( oldPosition != -1)
				{
					// The old alarm has been found in the alarm index vector
					itrMap->second.erase( itrMap->second.begin()+oldPosition);
					if( itrMap->second.empty())
					{
						m_ackedAlarmMap.erase(newAlarm.alarmSeverity);
					}
					m_ackedCount--;
					for( unsigned int i = m_highestSeverity; i< newAlarm.alarmSeverity; i++)
					{
						itrMap = m_ackedAlarmMap.find(i);
						
						if( itrMap != m_ackedAlarmMap.end() )
						{
							oldPosition += itrMap->second.size();
						}
					}
					oldPosition += (m_openCount+m_normalCount);
				}
			}
			
		}
		
		updateAlarmDetails(newAlarm, alarmUpdate);
		
		// find the new alarm's new insert position
		
		if( m_filterSet.passesFilters(newAlarm))
		{
			std::string ackBy(newAlarm.alarmAcknowledgedBy.in());
			
			if( newAlarm.state == TA_Base_Core::AlarmOpen && ackBy.length()==0 )
			{
				itrMap = m_openAlarmMap.find( newAlarm.alarmSeverity );
				
				if( itrMap == m_openAlarmMap.end())
				{
					
					// Create a new alarm index vector
					AlarmIndexVector openAlarmIndex;
					
					openAlarmIndex.push_back(&newAlarm);
					
					m_openAlarmMap.insert( std::pair< unsigned long, AlarmIndexVector >( newAlarm.alarmSeverity, openAlarmIndex) );
					
					newPosition = 0;
					
					m_openCount++;
				}
				else
				{
					if(m_isUsingSourceTime)
					{
						itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &newAlarm, compare_alarmsourcetime());
					}
					else
					{
						itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &newAlarm, compare_alarmdisplaytime());
					}
					
					newPosition = itrVec - itrMap->second.begin();
					
					itrMap->second.insert( itrVec, &newAlarm);
					
					m_openCount++;				
				}
				
				addAlarmSeverity( newAlarm);
				
				for( unsigned long i = m_highestSeverity; i < newAlarm.alarmSeverity; i++ )
				{
					itrMap = m_openAlarmMap.find(i);
					
					if( itrMap!= m_openAlarmMap.end() )
					{
						newPosition += itrMap->second.size();
					}
				}
				
			}
			else if( ackBy.length()==0)
			{
				itrMap = m_normalAlarmMap.find( newAlarm.alarmSeverity );
				
				if( itrMap == m_normalAlarmMap.end())
				{
					
					// Create a new alarm index vector
					AlarmIndexVector normalAlarmIndex;
					
					normalAlarmIndex.push_back(&newAlarm);
					
					m_normalAlarmMap.insert( std::pair< unsigned long, AlarmIndexVector >( newAlarm.alarmSeverity, normalAlarmIndex) );
					
					newPosition = 0;
					
					m_normalCount++;
				}
				else
				{
					if(m_isUsingSourceTime)
					{
						itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &newAlarm, compare_alarmsourcetime());
					}
					else
					{
						itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &newAlarm, compare_alarmdisplaytime());
					}
					
					newPosition = itrVec - itrMap->second.begin();
					
					itrMap->second.insert( itrVec, &newAlarm);
					
					m_normalCount++;				
				}
				
				addAlarmSeverity( newAlarm);
				
				for( unsigned long i = m_highestSeverity; i < newAlarm.alarmSeverity; i++ )
				{
					itrMap = m_normalAlarmMap.find(i);
					
					if( itrMap!= m_normalAlarmMap.end() )
					{
						newPosition += itrMap->second.size();
					}
				}
				
				newPosition += m_openCount;
			}
			else
			{
				itrMap = m_ackedAlarmMap.find( newAlarm.alarmSeverity );
				
				if( itrMap == m_ackedAlarmMap.end())
				{
					
					// Create a new alarm index vector
					AlarmIndexVector ackedAlarmIndex;
					
					ackedAlarmIndex.push_back(&newAlarm);
					
					m_ackedAlarmMap.insert( std::pair< unsigned long, AlarmIndexVector >( newAlarm.alarmSeverity, ackedAlarmIndex) );
					
					newPosition = 0;
					
					m_ackedCount++;
				}
				else
				{
					if(m_isUsingSourceTime)
					{
						itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &newAlarm, compare_alarmsourcetime());
					}
					else
					{
						itrVec = std::upper_bound( itrMap->second.begin(), itrMap->second.end(), &newAlarm, compare_alarmdisplaytime());
					}
					
					newPosition = itrVec - itrMap->second.begin();
					
					itrMap->second.insert( itrVec, &newAlarm);
					
					m_ackedCount++;				
				}
				
				for( unsigned long i = m_highestSeverity; i < newAlarm.alarmSeverity; i++ )
				{
					itrMap = m_ackedAlarmMap.find(i);
					
					if( itrMap!= m_ackedAlarmMap.end() )
					{
						newPosition += itrMap->second.size();
					}
				}			
				newPosition += (m_openCount+m_normalCount);
			}
		}
		else
		{
			newPosition = -1;
		}
		
		if( oldPosition == -1 && newPosition != -1 )
		{
			// We can not find the old alarm in the alarm index vectors
			// just add the new alarm.
			
			// update the highest and lowest alarm severity
			if( newAlarm.alarmSeverity < m_highestSeverity)
			{
				m_highestSeverity = newAlarm.alarmSeverity;
			}
			
			if( newAlarm.alarmSeverity > m_lowestSeverity )
			{
				m_lowestSeverity = newAlarm.alarmSeverity;
			}
			
			if( newPosition < MAX_DISPLAY_ALARMS)
			{		
				for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{						
					m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, newPosition,
						std::string(newAlarm.alarmID.in() ));
				}
			}
			
		}
		else if ( oldPosition >= MAX_DISPLAY_ALARMS && newPosition != -1)
		{
			// The old alarm is not in the display range
			
			if( newPosition < MAX_DISPLAY_ALARMS)
			{
				// Just add the new alarm to the display
				
				for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{						
					m_callback.indexInserted( m_callbackIDList[iClient],m_indexID, newPosition,
						std::string(newAlarm.alarmID.in() ));
				}
			}
			
			// else do nothing
		}
		else
		{
			// The old alarm is in the display range
			
			if( newPosition >= MAX_DISPLAY_ALARMS || newPosition == -1 )
			{
				// The new alarm is not in the display range
				// Just remove the index
				for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{						
					m_callback.indexDeleted( m_callbackIDList[iClient],m_indexID, oldPosition,
						std::string(newAlarm.alarmID.in() ));
				}
			}
			else
			{
				// The new alarm is also in the display range
				for ( unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{						
					m_callback.indexMoved( m_callbackIDList[iClient],m_indexID, oldPosition, newPosition,
						std::string(newAlarm.alarmID.in() ));
				}
			}
		}
		
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "updateAlarm: Failed to update alarm %s in an alarm store default index %d.", newAlarm.alarmID.in(), m_indexID);
	}
	
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm: updated alarm %s in alarm store index", alarm.alarmID.in());
	FUNCTION_EXIT;
}


/**
* Remove alarm from index (if it is present)
* @param alarmID
* 
*/
void AlarmStoreDefaultIndex::deleteAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("deleteAlarm");
	
	TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
	
	
	try
	{
		// find the right alarm to be deleted.
		
		std::string ackBy(alarm.alarmAcknowledgedBy.in());
		if( alarm.state == TA_Base_Core::AlarmOpen && ackBy.length()==0)
		{
			// the alarm is open and not_acked
			
			AlarmIndexMap::iterator itrMap = m_openAlarmMap.find( alarm.alarmSeverity);
			
			if( itrMap == m_openAlarmMap.end())
			{
				// non-existing alarm, do nothing here
				FUNCTION_EXIT;
				return;
			}
			
			int pos = findAlarmInVectorList( alarm, itrMap->second );
			
			if( pos >= 0)
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.begin() + pos;
				itrMap->second.erase(itrVec);
				
				m_openCount--;
				
				// Check the severity and maximum display
				
				if( itrMap->second.size() == 0 )
				{
					// the alarm index vector with this alarm severity and alarm state is empty now.
					// need to remove from the alarm index map
					
					m_openAlarmMap.erase( alarm.alarmSeverity);
					
					// need to update m_higestSeverity and m_lowestSeverity
					
					if( m_normalAlarmMap.find(alarm.alarmSeverity) == m_normalAlarmMap.end() 
						&& m_ackedAlarmMap.find(alarm.alarmSeverity) == m_ackedAlarmMap.end())
					{
						if( alarm.alarmSeverity == m_highestSeverity)
						{
							if( m_highestSeverity == m_lowestSeverity )
							{
								// no alarms and no severity now
								m_highestSeverity = 0;
								m_lowestSeverity = 0;
							}
							else
							{
								for( ; m_highestSeverity <= m_lowestSeverity; m_highestSeverity++ )
								{
									if( m_openAlarmMap.find(m_highestSeverity) != m_openAlarmMap.end()
										|| m_normalAlarmMap.find(m_highestSeverity) != m_normalAlarmMap.end() 
										|| m_ackedAlarmMap.find(m_highestSeverity) != m_ackedAlarmMap.end() )
									{
										break;
									}
								}
							}
						}
						
						if( m_lowestSeverity != 0 && alarm.alarmSeverity == m_lowestSeverity)
						{
							for( ; m_lowestSeverity >= m_highestSeverity; m_lowestSeverity--)
							{
								if( m_openAlarmMap.find(m_lowestSeverity) != m_openAlarmMap.end()
									|| m_normalAlarmMap.find(m_lowestSeverity) != m_normalAlarmMap.end() 
									|| m_ackedAlarmMap.find(m_lowestSeverity) != m_ackedAlarmMap.end() )
								{
									break;
								}
							}
						}
					}
					
				}
				
				if( m_maxDisplayIsReached )
				{
					if( (m_openCount + m_normalCount + m_ackedCount) < MAX_DISPLAY_ALARMS )
					{
						m_maxDisplayIsReached = false;
					}
				}
				
				
				unsigned int severity;
				
				for( severity = m_highestSeverity ; severity < alarm.alarmSeverity ; severity++)
				{
					itrMap = m_openAlarmMap.find(severity);
					if( itrMap != m_openAlarmMap.end())
					{
						pos += itrMap->second.size(); 
					}
				}
				
				// Tell all client applications abt the delete
				
				removeAlarmSeverity( alarm);
				
				for (unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{
					m_callback.indexDeleted(m_callbackIDList[iClient],m_indexID, pos,
						std::string(alarm.alarmID.in() ));
				}
				
			}
			else
			{
				// non-existing alarm, do nothing here.
				FUNCTION_EXIT;
				return;
			}
		}
		else if( ackBy.length() == 0)
		{
			// the alarm is normalized but not_acked
			AlarmIndexMap::iterator itrMap = m_normalAlarmMap.find( alarm.alarmSeverity);
			
			if( itrMap == m_normalAlarmMap.end())
			{
				// non-existing alarm, do nothing here
				FUNCTION_EXIT;
				return;
			}
			
			int pos = findAlarmInVectorList( alarm, itrMap->second );
			
			if( pos >= 0)
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.begin() + pos;
				itrMap->second.erase(itrVec);
				
				m_normalCount--;
				
				// Check the severity and maximum display
				
				if( itrMap->second.size() == 0 )
				{
					// the alarm index vector with this alarm severity and alarm state is empty now.
					// need to remove from the alarm index map
					
					m_normalAlarmMap.erase( alarm.alarmSeverity);
					
					// need to update m_higestSeverity and m_lowestSeverity
					
					if( m_openAlarmMap.find(alarm.alarmSeverity) == m_openAlarmMap.end() 
						&& m_ackedAlarmMap.find(alarm.alarmSeverity) == m_ackedAlarmMap.end())
					{
						if( alarm.alarmSeverity == m_highestSeverity)
						{
							if( m_highestSeverity == m_lowestSeverity )
							{
								// no alarms and no severity now
								m_highestSeverity = 0;
								m_lowestSeverity = 0;
							}
							else
							{
								for( ; m_highestSeverity <= m_lowestSeverity; m_highestSeverity++ )
								{
									if( m_openAlarmMap.find(m_highestSeverity) != m_openAlarmMap.end()
										|| m_normalAlarmMap.find(m_highestSeverity) != m_normalAlarmMap.end() 
										|| m_ackedAlarmMap.find(m_highestSeverity) != m_ackedAlarmMap.end() )
									{
										break;
									}
								}
							}
						}
						
						if( m_lowestSeverity != 0 && alarm.alarmSeverity == m_lowestSeverity)
						{
							for( ; m_lowestSeverity >= m_highestSeverity; m_lowestSeverity--)
							{
								if( m_openAlarmMap.find(m_lowestSeverity) != m_openAlarmMap.end()
									|| m_normalAlarmMap.find(m_lowestSeverity) != m_normalAlarmMap.end() 
									|| m_ackedAlarmMap.find(m_lowestSeverity) != m_ackedAlarmMap.end() )
								{
									break;
								}
							}
						}
					}
					
				}
				
				unsigned int severity;
				
				for( severity = m_highestSeverity ; severity < alarm.alarmSeverity ; severity++)
				{
					itrMap = m_normalAlarmMap.find(severity);
					if( itrMap != m_normalAlarmMap.end())
					{
						pos += itrMap->second.size(); 
					}
				}
				
				pos += m_openCount;
				
				// Tell all client applications abt the delete
				
				removeAlarmSeverity( alarm);
				
				for (unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
				{
					m_callback.indexDeleted(m_callbackIDList[iClient],m_indexID, pos,
						std::string(alarm.alarmID.in() ));
				}				
				
				
				if( m_maxDisplayIsReached && (m_openCount + m_normalCount + m_ackedCount) < MAX_DISPLAY_ALARMS )
				{
					m_maxDisplayIsReached = false;					
				}
			}
			else
			{
				// non-existing alarm, do nothing here.
				FUNCTION_EXIT;
				return;
			}
			
		}
		else
		{
			// the alarm is acked
			
			AlarmIndexMap::iterator itrMap = m_ackedAlarmMap.find( alarm.alarmSeverity);
			
			if( itrMap == m_ackedAlarmMap.end())
			{
				// non-existing alarm, do nothing here
				FUNCTION_EXIT;
				return;
			}
			
			int pos = findAlarmInVectorList( alarm, itrMap->second );
			
			if( pos >= 0)
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.begin() + pos;
				itrMap->second.erase(itrVec);
				
				m_ackedCount--;
				
				// Check the severity and maximum display
				
				if( itrMap->second.size() == 0 )
				{
					// the alarm index vector with this alarm severity and alarm state is empty now.
					// need to remove from the alarm index map
					
					m_ackedAlarmMap.erase( alarm.alarmSeverity);
					
					// need to update m_higestSeverity and m_lowestSeverity
					
					if( m_normalAlarmMap.find(alarm.alarmSeverity) == m_normalAlarmMap.end() 
						&& m_openAlarmMap.find(alarm.alarmSeverity) == m_openAlarmMap.end())
					{
						if( alarm.alarmSeverity == m_highestSeverity)
						{
							if( m_highestSeverity == m_lowestSeverity )
							{
								// no alarms and no severity now
								m_highestSeverity = 0;
								m_lowestSeverity = 0;
							}
							else
							{
								for( ; m_highestSeverity <= m_lowestSeverity; m_highestSeverity++ )
								{
									if( m_openAlarmMap.find(m_highestSeverity) != m_openAlarmMap.end()
										|| m_normalAlarmMap.find(m_highestSeverity) != m_normalAlarmMap.end() 
										|| m_ackedAlarmMap.find(m_highestSeverity) != m_ackedAlarmMap.end() )
									{
										break;
									}
								}
							}
						}
						
						if( m_lowestSeverity != 0 && alarm.alarmSeverity == m_lowestSeverity)
						{
							for( ; m_lowestSeverity >= m_highestSeverity; m_lowestSeverity--)
							{
								if( m_openAlarmMap.find(m_lowestSeverity) != m_openAlarmMap.end()
									|| m_normalAlarmMap.find(m_lowestSeverity) != m_normalAlarmMap.end() 
									|| m_ackedAlarmMap.find(m_lowestSeverity) != m_ackedAlarmMap.end() )
								{
									break;
								}
							}
						}
					}
					
				}
				unsigned int severity;
				
				for( severity =m_highestSeverity ; severity < alarm.alarmSeverity ; severity++)
				{
					itrMap = m_ackedAlarmMap.find(severity);
					if( itrMap != m_ackedAlarmMap.end())
					{
						pos += itrMap->second.size(); 
					}
				}
				
				pos += (m_openCount+m_normalCount);
				
				if( pos < MAX_DISPLAY_ALARMS)
				{
					// Tell all client applications abt the delete
					for (unsigned int iClient = 0;iClient<m_callbackIDList.size();++iClient)
					{
						m_callback.indexDeleted(m_callbackIDList[iClient],m_indexID, pos,
							std::string(alarm.alarmID.in() ));
					}
				}				
				
				if( m_maxDisplayIsReached && (m_openCount + m_normalCount + m_ackedCount) < MAX_DISPLAY_ALARMS  )
				{
					m_maxDisplayIsReached = false;
				}			
				
			}
			else
			{
				// non-existing alarm, do nothing here.
				FUNCTION_EXIT;
				return;
			}
			
		}
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "deleteAlarm: Failed to delete alarm %s in alarm store index", alarm.alarmID.in());
	}
	
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "deleteAlarm: deleted alarm %s in alarm store index", alarm.alarmID.in());
	FUNCTION_EXIT;
}


	/**
	* Add another callback for this index
	* @param callbackID    ID of the callback to be added.
	* 
	*/
	void AlarmStoreDefaultIndex::addCallback(long callbackID)
	{
		FUNCTION_ENTRY("addCallback");
		TA_Base_Core::ThreadGuard guard(m_callbackIDListLock);	
		
		m_callbackIDList.push_back(callbackID);
			
		FUNCTION_EXIT;
	}

	void AlarmStoreDefaultIndex::addAllAlarmsToCallback(long callbackID)
	{
		FUNCTION_ENTRY("addCallback");
		TA_Base_Core::ThreadGuard guardIndex(m_indexLock);
		TA_Base_Core::ThreadGuard guardCallback(m_callbackIDListLock);	
	// Send the initial clear
	m_callback.indexCleared(callbackID,m_indexID);
	
	// Notify this new call back of all the alarms in the index
	
	AlarmIndexMap::iterator itrMap;
	AlarmIndexVector::iterator itrVec;
	
	unsigned long pos = 0;
	
	unsigned long i = 0;
	for( i = m_highestSeverity; i<= m_lowestSeverity; i++)
	{
		itrMap = m_openAlarmMap.find(i);
		
		if( itrMap != m_openAlarmMap.end())
		{
			if( itrMap->second.empty() == false )
			{
				for( itrVec = itrMap->second.begin(); itrVec != itrMap->second.end(); itrVec++)
				{
					m_callback.indexInserted(callbackID,m_indexID,pos,std::string( (*itrVec)->alarmID.in() ));
					pos++;
				}
			}
		}
	}
	
	for( i = m_highestSeverity; i<= m_lowestSeverity; i++)
	{
		itrMap = m_normalAlarmMap.find(i);
		
		if( itrMap != m_normalAlarmMap.end())
		{
			if( itrMap->second.empty() == false )
			{
				for( itrVec = itrMap->second.begin(); itrVec != itrMap->second.end(); itrVec++)
				{
					m_callback.indexInserted(callbackID,m_indexID,pos,std::string( (*itrVec)->alarmID.in() ));
					pos++;
				}
			}
		}
	}
	
	for( i = m_highestSeverity; i<= m_lowestSeverity; i++)
	{
		itrMap = m_ackedAlarmMap.find(i);
		
		if( itrMap != m_ackedAlarmMap.end())
		{
			if( itrMap->second.empty() == false )
			{
				for( itrVec = itrMap->second.begin(); itrVec != itrMap->second.end(); itrVec++)
				{
					m_callback.indexInserted(callbackID,m_indexID,pos,std::string( (*itrVec)->alarmID.in() ));
					pos++;
				}
			}
		}
	}
	
	
	FUNCTION_EXIT;
}


/**
* Remove a callback from this index
* @param callbackID    ID of callback to be removed
* 
*/
void AlarmStoreDefaultIndex::removeCallback(long callbackID)
{
	FUNCTION_ENTRY("removeCallback");
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
bool AlarmStoreDefaultIndex::isUsed() 
{
	FUNCTION_ENTRY("isUsed");
	FUNCTION_EXIT;
	return m_callbackIDList.size()>0;
}

/**
* Determine if callback id is in this index.
*/
bool AlarmStoreDefaultIndex::isCallbackIDPresent(long callbackID) 
{
	FUNCTION_ENTRY("isCallbackIDPresent");
	
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




unsigned long AlarmStoreDefaultIndex::getEarliestSourceTime()
{
	
	TA_Base_Core::ThreadGuard guard(m_indexLock);
	
	unsigned long sourceTime(0);
	
	AlarmIndexMap::iterator itrMap;
	
	for( unsigned int severity = m_highestSeverity; severity <= m_lowestSeverity; severity ++)
	{
		
		itrMap = m_openAlarmMap.find(severity);
		
		if( itrMap != m_openAlarmMap.end() )
		{
			if( itrMap->second.empty() == false )
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.end();
				itrVec--;
				unsigned long tempTime = (*itrVec)->displayTime.time;
				if( sourceTime == 0 || tempTime < sourceTime )
				{
					sourceTime = tempTime;
				}
			}
		}
		
		itrMap = m_normalAlarmMap.find(severity);
		
		if( itrMap != m_normalAlarmMap.end() )
		{
			if( itrMap->second.empty() == false )
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.end();
				itrVec--;
				unsigned long tempTime = (*itrVec)->displayTime.time;
				if( sourceTime == 0 || tempTime < sourceTime )
				{
					sourceTime = tempTime;
				}
			}
		}
		
		itrMap = m_ackedAlarmMap.find(severity);
		
		if( itrMap != m_ackedAlarmMap.end() )
		{
			if( itrMap->second.empty() == false )
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.end();
				itrVec--;
				unsigned long tempTime = (*itrVec)->displayTime.time;
				if( sourceTime == 0 || tempTime < sourceTime )
				{
					sourceTime = tempTime;
				}
			}
		}
	}
	
	return sourceTime;
}


unsigned long AlarmStoreDefaultIndex::getLatestSourceTime()
{
	
	TA_Base_Core::ThreadGuard guard(m_indexLock);
	
	unsigned long sourceTime(0);
	
	AlarmIndexMap::iterator itrMap;
	
	for( unsigned int severity = m_highestSeverity; severity <= m_lowestSeverity; severity ++)
	{
		
		itrMap = m_openAlarmMap.find(severity);
		
		if( itrMap != m_openAlarmMap.end() )
		{
			if( itrMap->second.empty() == false )
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.begin();
				unsigned long tempTime = (*itrVec)->displayTime.time;
				sourceTime = tempTime > sourceTime ? tempTime:sourceTime;
			}
		}
		
		itrMap = m_normalAlarmMap.find(severity);
		
		if( itrMap != m_normalAlarmMap.end() )
		{
			if( itrMap->second.empty() == false )
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.begin();
				unsigned long tempTime = (*itrVec)->displayTime.time;
				sourceTime = tempTime > sourceTime ? tempTime:sourceTime;			
			}
		}
		
		itrMap = m_ackedAlarmMap.find(severity);
		
		if( itrMap != m_ackedAlarmMap.end() )
		{
			if( itrMap->second.empty() == false )
			{
				AlarmIndexVector::iterator itrVec = itrMap->second.begin();
				unsigned long tempTime = (*itrVec)->displayTime.time;
				sourceTime = tempTime > sourceTime ? tempTime:sourceTime;
			}
		}
	}
	
	return sourceTime;
}


long AlarmStoreDefaultIndex::findAlarmInVectorList (const TA_Base_Core::AlarmDetailCorbaDef& alarm, 
													std::vector<const TA_Base_Core::AlarmDetailCorbaDef*>& vectorList)
{
	FUNCTION_ENTRY("findAlarmInVectorList");
	
	int returnPos = -1;
	
	
	std::pair< AlarmIndexVector::iterator, AlarmIndexVector::iterator > result;
	
	if(m_isUsingSourceTime)
	{
		result = std::equal_range(vectorList.begin(), vectorList.end(), &alarm, compare_alarmsourcetime());
	}
	else
	{
		result = std::equal_range(vectorList.begin(), vectorList.end(), &alarm, compare_alarmdisplaytime());
	}
	
	
	
	AlarmIndexVector::iterator itr;
	
	std::string toFindAlarmId(alarm.alarmID.in());
	
	for( itr= result.first; itr != result.second; itr++ )
	{
		std::string findId( (*itr)->alarmID.in());
		if( findId.compare(toFindAlarmId) == 0 )
		{
			returnPos = itr - vectorList.begin();
			break;
		}
	}
	
	if( returnPos == -1)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The alarm %s is not found in the alarm index vector", alarm.alarmID.in());
	}
	
	FUNCTION_EXIT;
	return returnPos; // return either -1 ie not found or the exact position of the alarm
}


bool AlarmStoreDefaultIndex::checkAckPermission(unsigned long messageTypeKey, unsigned long locationKey, unsigned long subsystemKey, unsigned long assocEntityKey) const
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
        // Convert the alarm type name to an alarm type key.
		// The messageTypeKey of AlarmDetailCorbaDef should be the type key instead of the type name
		/*
        std::map<std::string, unsigned long>::iterator iter = m_alarmTypeNameToKeyMap.find(messageType);
        if (iter == m_alarmTypeNameToKeyMap.end())
        {
		// Alarm type not found. Just return true.
		FUNCTION_EXIT;
		return true;
        }
		*/
        
		//std::ostringstream alarmTypeKeyAsString;
        //alarmTypeKeyAsString << iter->second;
		
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


std::string AlarmStoreDefaultIndex::findAlarmIdAtPosition( unsigned long pos)
{
	TA_ASSERT( pos>=0, "The position of the alarm in the display index can not be negative." );
	
	if( pos < m_openCount)
	{
		// The alarm is in the part of open and not_acked alarms
		
		unsigned long currentPos = 0;
		unsigned long previousPos = 0;
		
		AlarmIndexMap::iterator itrMap;
		AlarmIndexVector::iterator itrVec;
		
		for(unsigned long i = m_highestSeverity; i>= m_lowestSeverity; i++)
		{
			itrMap = m_openAlarmMap.find(i);
			
			if( itrMap != m_openAlarmMap.end())
			{
				currentPos += itrMap->second.size();
				
				if( currentPos >= pos )
				{
					itrVec = itrMap->second.begin()+(pos-previousPos);
					return std::string((*itrVec)->alarmID.in());
				}
			}
		}
	}
	else if( pos < (m_openCount+m_normalCount) )
	{
		// The alarm is in the part of normalized and not_acked alarms
		
		unsigned long currentPos = m_openCount;
		unsigned long previousPos = m_openCount;
		
		AlarmIndexMap::iterator itrMap;
		AlarmIndexVector::iterator itrVec;
		
		for(unsigned long i = m_highestSeverity; i>= m_lowestSeverity; i++)
		{
			itrMap = m_normalAlarmMap.find(i);
			
			if( itrMap != m_normalAlarmMap.end())
			{
				currentPos += itrMap->second.size();
				
				if( currentPos >= pos )
				{
					itrVec = itrMap->second.begin()+(pos-previousPos);
					return std::string((*itrVec)->alarmID.in());
				}
			}
		}
	}
	else
	{
		// the alarm is in the part of acked alarms
		unsigned long currentPos = m_openCount+m_normalCount;
		unsigned long previousPos = m_openCount+m_normalCount;
		
		AlarmIndexMap::iterator itrMap;
		AlarmIndexVector::iterator itrVec;
		
		for(unsigned long i = m_highestSeverity; i>= m_lowestSeverity; i++)
		{
			itrMap = m_ackedAlarmMap.find(i);
			
			if( itrMap != m_ackedAlarmMap.end())
			{
				currentPos += itrMap->second.size();
				
				if( currentPos >= pos )
				{
					itrVec = itrMap->second.begin()+(pos-previousPos);
					return std::string((*itrVec)->alarmID.in());
				}
			}
		}
	}
	
	return "";
}

/*
* Update the alarm severity map based on the addition of the specified alarm.
*/
void AlarmStoreDefaultIndex::addAlarmSeverity( const TA_Base_Core::AlarmDetailCorbaDef& alarm )
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
void AlarmStoreDefaultIndex::removeAlarmSeverity( const TA_Base_Core::AlarmDetailCorbaDef& alarm )
{
	FUNCTION_ENTRY("removeAlarmSeverity");
	
	if( m_isSeverityProcessed)
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

unsigned long AlarmStoreDefaultIndex::getHighestSeverityAlarm()
{
	FUNCTION_ENTRY("getHighestSeverityAlarm");
	
	if( false == m_isSeverityProcessed)
	{
		FUNCTION_EXIT;
		return 0;
	}

	//marvin++
	TA_Base_Core::ThreadGuard guardSeverity(m_severityMapLock);
	//++marvin
	
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
* The terminate() method should cause run() to return. eg: CorbaUtil::
* GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
* to call this method directly - call terminateAndWait() instead
*/
void AlarmStoreDefaultIndex::terminate()
{
	FUNCTION_ENTRY("terminate");
	m_terminate = true;
	m_jobQueueSemaphore.post();
	
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Terminate called. Alarm store default index is stop running.");
	
	FUNCTION_EXIT;
}


/**
* This is the work method for the thread.  It will process the checking ack-permission and severity
* to avoid a long delay on the user corba call
*/
void AlarmStoreDefaultIndex::run()
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
				"Caught TransactiveException: %s in the run() of AlarmStoreDefaultIndex", 
				ex.what());
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Caught Corba Exception in the run() of AlarmStoreDefaultIndex");
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception in the run() of AlarmStoreDefaultIndex");
		}
		
	} // end of while loop
	
	m_terminate = false;
	FUNCTION_EXIT;
}


bool AlarmStoreDefaultIndex::isSameFilter(const AlarmFilterSet& filterSet) const
{
	FUNCTION_ENTRY("isSameFilterAndSort");
	FUNCTION_EXIT;
	return (m_filterSet==filterSet);
}

void AlarmStoreDefaultIndex::backupAlarmDetails( const TA_Base_Core::AlarmDetailCorbaDef& backupAlarm, TA_Base_Core::AlarmDetailCorbaDef& newAlarm,const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	FUNCTION_ENTRY("backupAlarmDetails");
	if (alarmUpdate.alarmHasBeenAcknowledged)
	{
		newAlarm.alarmAcknowledgedBy = CORBA::string_dup(backupAlarm.alarmAcknowledgedBy.in());
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
		newAlarm.alarmComments = CORBA::string_dup(backupAlarm.alarmComments.in());  // Note: Allen assures me that no further str mngmnt is needed!
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

void AlarmStoreDefaultIndex::updateAlarmDetails( TA_Base_Core::AlarmDetailCorbaDef& newAlarm, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	FUNCTION_ENTRY("updateAlarmDetails");
	if (alarmUpdate.alarmHasBeenAcknowledged)
	{
		newAlarm.alarmAcknowledgedBy = CORBA::string_dup(alarmUpdate.alarmAcknowledgedBy.in());
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
		newAlarm.alarmComments = CORBA::string_dup(alarmUpdate.alarmComments.in());  // Note: Allen assures me that no further str mngmnt is needed!
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
}