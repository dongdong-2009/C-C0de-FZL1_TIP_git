/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmStoreManager.cpp
//  Implementation of the Class AlarmStoreManager
//  Created on:      24-Jan-2004 04:17:51 PM
///////////////////////////////////////////////////////////

#pragma warning(disable:4284)   // return type for 'identifier::operator->' is not a UDT or reference to a UDT

#define  DEFAULT_ALARM_VISIBLE  "defaultalarmvisible"
#define  FAS_ALARM_TYPE_KEY  "FASAlarmTypeKey"

#include "AlarmStoreManager.h"
#include "AlarmStoreException.h"

#include "bus/alarm/alarm_store_library/src/AlarmFilterByProfile.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"


#include "core/alarm/src/AlarmConfig.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/threadguard.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/corba/src/CorbaUtil.h"
//#include "SharedAlarmAccessFactory.h"
#include "AlarmAccessAdaptor.h"

#include "core/utilities/src/RunParams.h"

#include "core/synchronisation/src/ProcessGuard.h"
#include "core/synchronisation/src/NonReEntrantProcessLockable.h"
#include "core/synchronisation/src/IProcessLockable.h"
#include "core/message/types/DataPointAlarms_MessageTypes.h"
//-- DTL_Alarm_Display_Filter
#include "core/data_access_interface/src/IAlarmDisplayFilterData.h"
#include "core/data_access_interface/src/AlarmDisplayFilterAccessFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/synchronisation/src/ThreadGuard.h"
using namespace TA_Base_Bus;

using TA_Base_Core::DebugUtil;


/**
 * Constructor
 * @param indexCallback
 * 
 */
AlarmStoreManager::AlarmStoreManager(TA_Base_Bus::RightsLibrary* rightsLibrary) 
				  : m_preFilterSet(NULL),
					m_rightsLibrary(rightsLibrary),
					m_alarmDefaultVisible(true),
					m_lLoadAgents(0),
					m_systemDisplayFilterSet(NULL)
					
{
	FUNCTION_ENTRY("Constructor");

	// Get maps of alarm type keys to names, and vice versa.
	std::string paraValue="false";

	if (TA_Base_Core::RunParams::getInstance().isSet(DEFAULT_ALARM_VISIBLE))
	{
	   paraValue=TA_Base_Core::RunParams::getInstance().get(DEFAULT_ALARM_VISIBLE);
	}
	m_FAS_Alarm_Type_Key=atol(TA_Base_Core::DataPointAlarms::FireAlarm.getTypeKey().c_str());
	if (TA_Base_Core::RunParams::getInstance().isSet(FAS_ALARM_TYPE_KEY))
	{
		m_FAS_Alarm_Type_Key=atol(TA_Base_Core::RunParams::getInstance().get(FAS_ALARM_TYPE_KEY).c_str());
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"FAS Alarm type=%d",m_FAS_Alarm_Type_Key);

   if (paraValue=="false")
   {
	   m_alarmDefaultVisible=false;
	   LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		"defaultalarmvisible=%s", paraValue.c_str() );
   }
   else
   {
	   m_alarmDefaultVisible=true;		   
	   LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		"defaultalarmvisible=%s", paraValue.c_str() );
   }

	//start();
	FUNCTION_EXIT;
}

AlarmStoreManager::~AlarmStoreManager()
{
	FUNCTION_ENTRY("Destructor");
	// DTL_Alarm_Display_Filter
	if (NULL != m_systemDisplayFilterSet)
	{
		delete m_systemDisplayFilterSet;
		m_systemDisplayFilterSet = NULL;
	}
	FUNCTION_EXIT;
	return;
}

// DTL_Alarm_Display_Filter
void AlarmStoreManager::setSystemDisplayFilter(const unsigned long& _locationkey)
{
	FUNCTION_ENTRY("setDisplayFilter");
	
	try
	{
		std::vector<TA_Base_Core::IAlarmDisplayFilterData*> vecDbFilterData = TA_Base_Core::AlarmDisplayFilterAccessFactory::getInstance().getLocationsFilterData(_locationkey);

		if (vecDbFilterData.size() > 0)
		{
			Vec_displayFilter_T vecDisplayFilter;
			for (int idx=0; idx<vecDbFilterData.size(); idx++)
			{
				displayFilter* pNewFilter = new displayFilter;
				TA_Base_Core::IAlarmDisplayFilterData* pDisplayFilter = vecDbFilterData[idx];

				pNewFilter->filterKey = pDisplayFilter->getKey();
				pNewFilter->alarmType = pDisplayFilter->getAlarmType();
				pNewFilter->subSystemKey = pDisplayFilter->getSubSystemKey();
				pNewFilter->alarmLocationKey = pDisplayFilter->getAlarmLocationKey();
				pNewFilter->displayCondition = (displayCond)pDisplayFilter->getDisplayCondition();

				vecDisplayFilter.push_back(pNewFilter);

				//Release data after use
				delete pDisplayFilter;
				pDisplayFilter = NULL;
			}
			
			// Create new system display filter
			m_systemDisplayFilterSet = new AlarmFilterBySystemDisplay(vecDisplayFilter);
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loaded Alarm Display Filter Count: %d", vecDbFilterData.size());

			//Clear container
			vecDbFilterData.clear();
		}
	}
	catch(const TA_Base_Core::DataException& dataEx)
	{
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "DataException caught: reason=%s", dataEx.what());
	}
	catch(const TA_Base_Core::DatabaseException& dbEx)
	{
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "DatabaseException caught: reason=%s", dbEx.what());
	}
	catch(...)
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown reason while trying to retrieve data from AlarmDisplayFilter");
	}

	FUNCTION_EXIT;
}


/**
 * This function checks to see if the supplied alarm passes the pre-filter check
 * ok.  This will be used to determine if it should be added to the store.
 */
bool AlarmStoreManager::passesPrefilter(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("passesPrefilter");
	// DTL_Alarm_Display_Filter
	if (NULL != m_systemDisplayFilterSet)
	{
		if (!m_systemDisplayFilterSet->includeAlarm(alarm))
		{
			return false;
		}
	}

	// If no prefilter than it must be included
	if (m_preFilterSet==NULL) 
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			 "AlarmDetailCorbaDef m_preFilterSet==NULL" );
		return m_alarmDefaultVisible;
	}

	FUNCTION_EXIT;
	return m_preFilterSet->passesFilters(alarm);
}
/*------------------------------------------- New Alarm Store ----------------------------------------------*/
bool AlarmStoreManager::passesPrefilter(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("passesPrefilter");
	// DTL_Alarm_Display_Filter
	if (NULL != m_systemDisplayFilterSet)
	{
		if (!m_systemDisplayFilterSet->includeAlarm(alarm))
		{
			return false;
		}
	}

	// If no prefilter than it must be included
	if (m_preFilterSet==NULL)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"m_preFilterSet==NULL" );
		return m_alarmDefaultVisible;
	}
	
	FUNCTION_EXIT;
	return m_preFilterSet->passesFilters(alarm);
}
/*----------------------------------------------------------------------------------------------------------*/
bool AlarmStoreManager::passesPrefilter(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("passesPrefilter");
	// DTL_Alarm_Display_Filter
	if (NULL != m_systemDisplayFilterSet)
	{
		if (!m_systemDisplayFilterSet->includeAlarm(alarm))
		{
			return false;
		}
	}

	//TA_Base_Core::ThreadGuard guard(m_storeLock);
	// If no prefilter than it must be included
	if (m_preFilterSet==NULL)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"m_preFilterSet==NULL" );
		return m_alarmDefaultVisible;
	}
	
	FUNCTION_EXIT;
	return m_preFilterSet->passesFilters(alarm);
}

bool AlarmStoreManager::hasPrefilterChanged(const AlarmFilterSet& filterSet)
{
	FUNCTION_ENTRY("hasPrefilterChanged");


	//Add this lock, so the sharedAlarm.dll can use the lock by function point.
// 	TA_Base_Core::NonReEntrantProcessLockable DutyLock(ALLALARMLOCKNAME); 
// 	TA_Base_Core::ProcessGuard   DutyGuard (DutyLock );
	TA_Base_Core::ThreadGuard  DutyGuard(m_ProfileLock);
	if (m_preFilterSet==NULL)
	{
		// No current pre - filter set
		return true;
	}
	

	FUNCTION_EXIT;
	return !(filterSet==*m_preFilterSet);
	
}


/**
 * Sets a new pre-filter to the store.  This will require a total reload of alarm
 * records if a pre-filter was already assigned.  If there was no previous pre-
 * filter then just apply filter to current store.  Remeber to update index
 * entries!
 * @param filter    Set of pre-filters to be applied.
 * 
 */
bool AlarmStoreManager::setPreFilter(AlarmFilterSet* filter)
{
	FUNCTION_ENTRY("setPreFilter");
	bool bRetVal = false;
	// Protect from other updates
	// Wenguang--
	// Remove this thread guard since thread guard already exists in deleteAllAlarms()

	// validate arguments
	if (NULL == filter)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			"new filter==NULL" );
		return bRetVal;
	}

	//Add this lock, so the sharedAlarm.dll can use the lock by function point.
// 	TA_Base_Core::NonReEntrantProcessLockable DutyLock(ALLALARMLOCKNAME); 
// 	TA_Base_Core::ProcessGuard   DutyGuard (DutyLock );

	// Remove any existing pre-filter
	TA_Base_Core::ThreadGuard  DutyGuard(m_ProfileLock);
	if (m_preFilterSet !=NULL )
	{
		if(NULL != filter && *filter==*m_preFilterSet)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
				 "the prefile is same, and two threads currently access the filterset" );
			FUNCTION_EXIT
			return bRetVal;
		}
		
		delete m_preFilterSet;
		m_preFilterSet = NULL;
	}

	if (filter==NULL)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			 "new filter==NULL" );
	}

	m_preFilterSet = filter;
	bRetVal = true;
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			 "setPreFilter called. m_preFilterSet is replaced with the param filter." );

	// The store will be reloaded by the servant/AlarmAgentInterface Manager classes
	// but to make it more efficient delete all alarms and indexes.  Otherwise they
	// will be done per location requiring lookups between store and indexes!
	
	//deleteAllAlarms(); no use for new alarmstore service

	FUNCTION_EXIT;
	return bRetVal;
}

//++Noel
void AlarmStoreManager::removePrefilter()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "AlarmStoreManager::removePrefilter coming in");

	TA_Base_Core::ThreadGuard  DutyGuard(m_ProfileLock);
	delete m_preFilterSet;
	m_preFilterSet = NULL;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "AlarmStoreManager::removePrefilter coming out");
}


/**
 * New alarm
 * @param alarm
 * 
 */
/*-------------------------------------New Alarm Store -------------------------------------*/
void AlarmStoreManager::newAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("newAlarm");
	
	//TA_Base_Core::ThreadGuard guard(m_storeLock);
	
	std::string ackBy(alarm.alarmAcknowledgedBy.in());
	if( alarm.state == TA_Base_Core::AlarmClosed && ackBy.length() != 0 )
	{
		// The alarm should not be visible if it is normalized and acknowledged
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "Discard alarm '%s' is not visible since it is normalized and acknowledged.", alarm.alarmID.in() );//TD14337 yezh++
	    FUNCTION_EXIT;
		return;
	}
	
    bool passedPrefilter = true;

    try
    {
		TA_Base_Core::NonReEntrantProcessLockable DutyLock(DUTYCACHELOCK);  
 		TA_Base_Core::ProcessGuard   DutyGuard (DutyLock);
		passedPrefilter = passesPrefilter(alarm);
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"An error occurred while determining alarm visibility for alarm with ID %s",
            alarm.alarmID.in());//TD14337 yezh++
    }

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "newAlarm, passPreFilter:%d, alarmID:%s, assetName:%s, entityKey:%ld, messageType:%ld", 
		(int)passedPrefilter, alarm.alarmID.in(), alarm.assetName.in(), alarm.assocEntityKey, alarm.messageTypeKey);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "  EntityTypeKey:%ld, avalancheHeadID:%s, decisionSupportState:%ld, parentAlarmID:%s", 
	  alarm.assocEntityTypeKey, alarm.avalancheHeadID.in(), alarm.decisionSupportState, alarm.parentAlarmID.in());
	TA_Base_Core::AlarmDetailCorbaDef* newAlarm = new TA_Base_Core::AlarmDetailCorbaDef(alarm);
	newAlarm->alarmDescription = 
			CORBA::string_dup(
			TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription
			(
			newAlarm->messageTypeKey,  
			std::string(newAlarm->alarmParameterList.in())).c_str()
			);
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, " alarmComments:%s, alarmDescription:%s, alarmSeverity:%ld, alarmValue:%s", 
	  newAlarm->alarmComments.in(), newAlarm->alarmDescription.in(), newAlarm->alarmSeverity, newAlarm->alarmValue.in());
	try
	{
		SYSTEMTIME st, et;
		::GetSystemTime(&st); 		 					
		AlarmAccessAdaptor::getInstance().insertAlarm(*newAlarm, passedPrefilter);
	    
		if (passedPrefilter)
		{
	        if (alarm.messageTypeKey==m_FAS_Alarm_Type_Key)
			{
	            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"FAS Alarm triggered, entity key=%d",alarm.assocEntityKey);
				AlarmAccessAdaptor::getInstance().setLatestFASAlarm(alarm.assocEntityKey);
			}
		}

	 	::GetSystemTime(&et);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "insertAlarm: ST: %d:%d:%d:%d || ET: %d:%d:%d:%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, et.wHour, et.wMinute, et.wSecond, et.wMilliseconds );

		delete newAlarm;
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "insertAlarm: LocationKey=%ld, unknown exception",
            newAlarm->locationKey);
	}

	FUNCTION_EXIT;
}


/**
 * An alarm has been updated
 * @param alarm
 * 
 */
 /*-----------------------------------New Alarm Store-------------------------------------------*/
void AlarmStoreManager::updateAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	FUNCTION_ENTRY("updateAlarm");
	//TA_Base_Core::ThreadGuard guard(m_storeLock);
	int iRet=1000;
	//TA_Base_Bus::TA_SharedAlarm cs;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm,  alarmID:%s, assetName:%s, entityKey:%ld, messageType:%ld loc=%ld", 
		  alarmUpdate.alarmID.in(), alarmUpdate.assetName.in(), alarmUpdate.assocEntityKey, alarmUpdate.messageTypeKey,alarmUpdate.locationKey);
 
	if (alarmUpdate.alarmHasBeenAcknowledged)
	{ 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm,  ackby=:%s",alarmUpdate.alarmAcknowledgedBy.in());
	}
	
	if (alarmUpdate.alarmHasBeenClosed)
	{		 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm,  closeValue=:%s",alarmUpdate.closeValue.in());		
	} 
	
	if (alarmUpdate.commentsWereAdded)
	{
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm,  comment=:%s",alarmUpdate.alarmComments.in());	
	}
	try
	{
		AlarmAccessAdaptor::getInstance().updateAlarm(alarmUpdate);
		 
	}
    catch(...)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "updateAlarm: LocationKey=%ld, unknown exception",
            alarmUpdate.locationKey);
		 
	}

	FUNCTION_EXIT;
}

// void AlarmStoreManager::clearAlarms(unsigned long location)
// {
// 	try
// 	{
// 		AlarmAccessAdaptor::getInstance().removeLocationAlarms(location);
// 	}
// 	catch(...)
// 	{
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "RemoveLocations alarm: LocationKey=%ld, unknown exception", location);
// 	}
// }

//******************
// Private Methods *
//******************

bool AlarmStoreManager::sessionDetailsChanged(const std::string& session)
{
    FUNCTION_ENTRY("sessionDetailsChanged");
    // This notification says that the duty has changed for the provided sessionId. When this happens, the Alarm Store
    // needs to do one of two things:
    // 1) If the pre-filter contains a profile filter based on this session ID, then all indicies need to be reloaded
    // 2) If an index contains a profile filter based on this session ID, the that index needs to be reloaded

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sessionDetailsChanged. Session id changed. Param session id %s.", session.c_str());

	if (m_preFilterSet == NULL)
	{
		// Pre-filter not set - just ignore the duty change.
	    FUNCTION_EXIT;
		return false;
	}

	// TD 12710 Improve speed of sorting and filtering
	// Protect from other updates
	// TA_Base_Core::ThreadGuard guard(m_storeLock);

    // Check the pre-filter first
    AlarmFilterByProfile profileFilter(session);

    if (m_preFilterSet->isFilterPresent(profileFilter))
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Session details changed. Session id: %s. Completed deleting all alarms and return True.", session.c_str());

		FUNCTION_EXIT;
        return true;
    }

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
		"Session details changed. Session id: %s. Completed Loop through each index, and determine it has the specified profile as a filter.", session.c_str());

    FUNCTION_EXIT;
    return false;
}


void AlarmStoreManager::setSessionId(const std::string& sessionId)
{
	TA_Base_Core::ThreadGuard guard(m_SessionLock);
    m_sessionID = sessionId;
}

bool AlarmStoreManager::isAlarmVisible(TA_Base_Bus::AlarmDataItem* pAlarm)
{
	return passesPrefilter(pAlarm);
}
