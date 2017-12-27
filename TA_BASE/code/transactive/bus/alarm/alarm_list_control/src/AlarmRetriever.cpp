/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/AlarmRetriever.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is used by alarm actions to retrieve certain alarm details and
 * other classes that provide various functionality (eg check rights, retrieve objects from
 * the database etc).
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/AlarmListCtrl.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
	TA_Base_Core::ReEntrantThreadLockable  AlarmRetriever::m_actionLock;
    AlarmRetriever::AlarmRetriever()
        : m_alarmListCtrl(NULL),
          m_isActionRunning(false),
		  m_areSelAlarmsCached(false), m_areVisAlarmIDsCached(false)
    {
    }


    AlarmRetriever::~AlarmRetriever()
    {
    }


    void AlarmRetriever::clearCache()
    {
        TA_Base_Core::ThreadGuard guard(m_alarmListLock);
        
		m_areSelAlarmsCached = false;
        m_areVisAlarmIDsCached = false;
        CacheSelectedAlarms().swap(m_cacheSelectedAlarms);

		std::vector<std::string>().swap(m_selectedAlarms);
		std::vector<std::string>().swap(m_visibleAlarms);
	
		std::vector<TA_Base_Core::AlarmModifyItem*>().swap(m_selectedAlarmsModItem);
		std::vector<TA_Base_Core::AlarmModifyItem*>().swap(m_visibleAlarmsModItem);

    }


    std::string AlarmRetriever::getCurrentAction()
    {
        TA_Base_Core::ThreadGuard guard(m_actionLock);
        return m_actionName;
    }


    void AlarmRetriever::setAlarmListCtrl(AlarmListCtrl& alarmList)
    {
        m_alarmListCtrl = &alarmList;
    }

	void AlarmRetriever::setSelectionRetriever(TA_Base_Bus::AlarmSelectionWrap* pSelectionWrap)
	{
		m_pSelectionWrap = pSelectionWrap;
	}

    std::vector<std::string> AlarmRetriever::getSelectedAlarms()
    {
        TA_ASSERT(m_alarmListCtrl != NULL, "The Alarm list control must call setAlarmListCtrl() before the actions can start calling this method");

        TA_Base_Core::ThreadGuard guard(m_alarmListLock);

        if (!m_areSelAlarmsCached)
        {
            m_cacheSelectedAlarms = m_pSelectionWrap->getSelectedAlarms();
            m_areSelAlarmsCached = true;
        }

        return m_cacheSelectedAlarms;
    }

	std::vector<TA_Base_Core::AlarmModifyItem*> AlarmRetriever::getSelectedModItems()
	{
		TA_Base_Core::ThreadGuard guard(m_alarmListLock);

		if (m_selectedAlarmsModItem.size() != 0)
		{
			freeAlarmModifyItems(m_selectedAlarmsModItem);
		}

		m_selectedAlarmsModItem = m_pSelectionWrap->getSelAlarmModifyItems();
		
		return m_selectedAlarmsModItem;
	}

	std::vector<std::string> AlarmRetriever::getAllVisibleAlarms()
	{
		TA_Base_Core::ThreadGuard guard(m_alarmListLock);
		
		if (!m_areVisAlarmIDsCached)
		{
			m_visibleAlarms = m_pSelectionWrap->getAllVisibleAlarms();
			m_areVisAlarmIDsCached = true;
		}
	
		return m_visibleAlarms;
	}

	std::vector<TA_Base_Core::AlarmModifyItem*> AlarmRetriever::getAllVisibleModItems()
	{
		TA_Base_Core::ThreadGuard guard(m_alarmListLock);

		m_visibleAlarmsModItem = m_pSelectionWrap->getAllAlarmModifyItem();

		return m_visibleAlarmsModItem;
	}

	bool AlarmRetriever::deleteSharedAlarm(std::vector<std::string>& alarmIDs)
	{
		TA_ASSERT(m_alarmListCtrl != NULL, "The Alarm list control must call setAlarmListCtrl() before the actions can start calling this method");
		return m_alarmListCtrl->deleteSharedAlarm(alarmIDs);
	}
    RightsChecker& AlarmRetriever::getRightsChecker()
    {
        TA_ASSERT(m_alarmListCtrl != NULL, "The Alarm list control must call setAlarmListCtrl() before the actions can start calling this method");
        return m_alarmListCtrl->getRightsChecker();
    }


    DatabaseCache& AlarmRetriever::getDatabaseCache()
    {
        TA_ASSERT(m_alarmListCtrl != NULL, "The Alarm list control must call setAlarmListCtrl() before the actions can start calling this method");
        return m_alarmListCtrl->getDatabaseCache();
    }


    AlarmListCtrl& AlarmRetriever::getAlarmListControl() const
    {
        TA_ASSERT(m_alarmListCtrl != NULL, "The Alarm list control must call setAlarmListCtrl() before the actions can start calling this method");
        return *m_alarmListCtrl;
    }

    
    bool AlarmRetriever::isActionAlreadyRunning()
    {
        TA_Base_Core::ThreadGuard guard(m_actionLock);
        return m_isActionRunning;
    }


    void AlarmRetriever::setIsActionAlreadyRunning(bool isActionRunning, const std::string& actionName)
    {
        TA_Base_Core::ThreadGuard guard(m_actionLock);
        m_isActionRunning = isActionRunning;

        if (m_isActionRunning)
        {
            m_actionName = actionName;
        }
        else
        {
            m_actionName = "";
        }
    }

	//++Noel
	std::string AlarmRetriever::getStringAttr(std::string strAlarmID, STRING_ATTR subItem)
	{
		return m_pSelectionWrap->getItemString(strAlarmID, subItem);
	}

	unsigned long AlarmRetriever::getNumberAttr(std::string strAlarmID, NUMBER_ATTR subItem)
	{
		return m_pSelectionWrap->getItemNumber(strAlarmID, subItem);
	}	
	
	// This function returns a copy of the AlarmItem
	// Users must deallocate this alarm after use
	AlarmDataItem* AlarmRetriever::getAlarmItem(std::string& strAlarmID)
	{
		return m_pSelectionWrap->getItemCopyID(strAlarmID);
	}
	
	// This function return a new AlarmModifyItem
	// Users must deallocate this alarm after use
	TA_Base_Core::AlarmModifyItem* AlarmRetriever::getAlarmModifyItem(std::string& strAlarmID)
	{
		return m_pSelectionWrap->getAlarmModifyItem(strAlarmID);
	}
	
	// deallocate the alarm modify items
	// This function must be called to free the std::vector<TA_Base_Core::AlarmModifyItem*>
	void AlarmRetriever::freeAlarmModifyItems(std::vector<TA_Base_Core::AlarmModifyItem*>& alarmModifyList)
	{
		std::vector<TA_Base_Core::AlarmModifyItem*>::iterator itr = alarmModifyList.begin();
		for (itr; itr != alarmModifyList.end(); itr++)
		{
			delete (*itr);
			(*itr) = NULL;
		}
		alarmModifyList.clear();
	}

	void AlarmRetriever::getAvalanchedChildAlarms(std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms)
	{
		try
		{
			m_pSelectionWrap->getAvalanchedChildAlarms(avalanchedHeadID, vecChildAlarms);
// 			std::vector<TA_Base_Core::AlarmItem*> vecVisibleAlarms;
// 			vecVisibleAlarms = m_alarmListCtrl->getAllVisibleAlarms();
// 			if (vecVisibleAlarms.size() != 0)
// 			{
// 				// Iterate through the alarms and get those child alarms there are related to this head
// 				for ( std::vector<TA_Base_Core::AlarmItem*>::iterator itr = vecVisibleAlarms.begin(); itr!=vecVisibleAlarms.end(); ++itr )
// 				{
// 					if (('F' == *((*itr)->avalanche)) && (0 == avalanchedHeadID.compare((*itr)->avalancheHeadID)))
// 					{
// 						vecChildAlarms.push_back(std::string((*itr)->alarmID));
// 					}
// 				}
// 			}
// 			vecVisibleAlarms.clear();
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred while trying to retrieve all child avalanche alarms.");
		}
	}

	bool AlarmRetriever::isAlarmsVisible()
	{
		return m_pSelectionWrap->isAlarmsVisible();	
	}
} //End of TA_Base_Bus
